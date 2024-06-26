#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"
#include "../Other/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Enemy/EnemyBase.h"
#include "../Enemy/EnemyManager.h"

//�����[�h
//����
//�`������
//�`�����݃X�i�C�p�[�̏ꍇ��l�̂�

//�t�H�[�g�i�C�g��Remnant2�����킹�������ɂ���
//HPGauge�E�e�e���i5/30�j�E�V�[���h���X�^�~�i�E�A�C�e��
//���̑��̏��\��

namespace
{
    XMFLOAT3 handOffset = { 0.2f, 0.7f, 0.1f };      // �ړ���
    XMFLOAT3 modelScale = { 0.3f, 0.3f, 0.3f };      // ���f���T�C�Y
    std::string modelName = "Model/Rifle.fbx";       // ���f����

}

Gun::Gun(GameObject* parent)
    :GameObject(parent, "Gun"), hModel_(-1)
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    // �f�[�^�̃��[�h
    hModel_ = Model::Load(modelName);
    assert(hModel_ >= 0);

    Model::GetBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    //�v���C���[�̎�̈ʒu�܂Œ���
    transform_.position_ = handOffset;
    transform_.scale_ = modelScale;

    // �o���b�g�^�C�v��enum�T�C�Y�ŏ�����
    bulletInfoList_.resize(static_cast<int>(BulletType::MAX));
}

void Gun::Update()
{
    // �N�[���^�C�������炷
    for (auto& bullet : bulletInfoList_) { bullet.coolTime_--; }

    // �ʏ�ˌ�
    if (InputManager::IsCmd(InputManager::ATTACK) && bulletInfoList_[(int)BulletType::NORMAL].coolTime_ <= 0)
    {
        ShootBullet<Bullet_Normal>(BulletType::NORMAL);
    }

}

void Gun::Draw()
{
    transform_.rotate_.x = -GameManager::GetPlayer()->GetAim()->GetRotate().x;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void Gun::Release()
{
}

void Gun::OnCollision(GameObject* pTarget)
{
    // �G�ɓ��������Ƃ�
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        rayHit_ = true;
    }
}

template <class T>
void Gun::ShootBullet(BulletType type)
{
    //Bullet�C���X�^���X�쐬
    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
    bulletInfoList_[(int)type].coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;
    int killTimer = pNewBullet->GetBulletParameter().killTimer_;
    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    float calcDist = bulletSpeed * killTimer;

    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 cameraVec = Float3Normalize(Float3Sub(Camera::GetTarget(), Camera::GetPosition()));
    float cameraDist = CalculationDistance(Camera::GetPosition(), Camera::GetTarget());
    
    RayCastData data;
    data.start = Camera::GetPosition();
    data.dir = cameraVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist + cameraDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
    //�G�Ɠ����������A�ŒZ�������v�Z
    std::vector<EnemyBase*>& enemyList = EnemyManager::GetAllEnemy();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minEneHitPos = XMFLOAT3();

    //�R���C�_�[�o�^
    XMFLOAT3 centerPos = Float3Sub(Camera::GetPosition(), GetWorldPosition());
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&cameraVec));
    collid->size_ = XMFLOAT3(calcDist + cameraDist, calcDist + cameraDist, calcDist + cameraDist);
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    AddCollider(collid);

    for (int i = 0; i < enemyList.size(); i++) {
        XMFLOAT3 vec = Float3Sub(enemyList[i]->GetPosition(), data.start);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //�ǂɓ����鋗������Ȃ�����A�����蔻����
        rayHit_ = false;
        this->Collision(enemyList[i]);

        //�ŒZ�����œ�������
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minEneHitPos = collid->targetPos_;
        }
    }
    //�R���C�_�[������
    ClearCollider();

    //�G��ڕW�ɂ��Ă����ꍇ
    EnemyBase* enemy = nullptr;
    if (minIndex >= 0) {
        enemy = enemyList[minIndex];
    }

    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pNewBullet->SetMove(move);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->Shot(enemy, gunTar);
}