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

    Model::GetPartBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    //�v���C���[�̎�̈ʒu�܂Œ���
    transform_.position_ = handOffset;
    transform_.scale_ = modelScale;

}

void Gun::Update()
{
    // �N�[���^�C�������炷
    bulletInfo_.coolTime_--;

    // �ʏ�ˌ�
    if (InputManager::IsCmd(InputManager::ATTACK, 0) && bulletInfo_.coolTime_ <= 0)
    {
        ShootBullet<Bullet_Normal>(BulletType::NORMAL);
    }

}

void Gun::Draw()
{
    transform_.rotate_.x = GetParent()->GetRotate().x;

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
    bulletInfo_.coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;
    int killTimer = pNewBullet->GetBulletParameter().killTimer_;
    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    float calcDist = bulletSpeed * killTimer;

    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 cameraVec = Float3Normalize(Float3Sub(Camera::GetTarget(0), Camera::GetPosition(0)));
    float cameraDist = CalculationDistance(Camera::GetPosition(0), Camera::GetTarget(0));
    
    RayCastData data;
    data.start = Camera::GetPosition(0);
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
    XMFLOAT3 centerPos = Float3Sub(Camera::GetPosition(0), GetWorldPosition());
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&cameraVec));
    collid->size_ = XMFLOAT3(calcDist + cameraDist, calcDist + cameraDist, calcDist + cameraDist);
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    collid->typeList_.push_back(OBJECT_TYPE::Player);
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

    //Aim�œ�����Ȃ���������A�{���̋O�ՂŔ�����s��
    if (minIndex < 0) {
        //�R���W�����}�b�v�Ƃ̔���
        XMFLOAT3 gunVec = Float3Normalize(Float3Sub(gunTar, gunTop));
        data = RayCastData();
        data.start = gunTop;
        data.dir = gunVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

        //�R���C�_�[���Z�b�g
        XMFLOAT3 gunRoot = Model::GetBonePosition(hModel_, rootBoneIndex_, rootPartIndex_);
        centerPos = Float3Sub(gunRoot, GetWorldPosition());
        collid->SetCenter(centerPos);
        collid->SetVector(XMLoadFloat3(&gunVec));

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
    }
    ClearCollider();

    //�G��ڕW�ɂ��Ă����ꍇ
    EnemyBase* enemy = nullptr;
    if (minIndex >= 0) {
        gunTar = minEneHitPos;
        enemy = enemyList[minIndex];
    }

    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pNewBullet->SetMove(move);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->Shot(enemy, gunTar);
}