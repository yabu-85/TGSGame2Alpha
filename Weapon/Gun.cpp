#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Engine/CapsuleCollider.h"
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

    //�����蔻�苗��
    static const float CALC_DISTANCE = 30.0f;

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
    transform_.rotate_.x = GameManager::GetPlayer()->GetAim()->GetRotate().x;

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

XMFLOAT3 Gun::CalculateBulletMovement(XMFLOAT3 top, XMFLOAT3 root, float bulletSpeed)
{
    // �ˏo�������v�Z���Đ��K��  (top - root)
    XMVECTOR vMove = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&top), XMLoadFloat3(&root)));

    // �e����ǉ��ݒ�(�e�Ɏ�ނ��������Ƃ��p)
    vMove *= bulletSpeed;

    // float3�^�ɖ߂�
    XMFLOAT3 move;
    XMStoreFloat3(&move, vMove);
    return move;
}

template <class T>
void Gun::ShootBullet(BulletType type)
{
    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 cameraVec = Float3Normalize(Float3Sub(Camera::GetTarget(), Camera::GetPosition()));
    float cameraDist = CalculationDistance(Camera::GetPosition(), Camera::GetTarget());

    RayCastData data;
    data.start = Camera::GetPosition();
    data.dir = cameraVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, CALC_DISTANCE + cameraDist));
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
    collid->size_ = XMFLOAT3(CALC_DISTANCE + cameraDist, CALC_DISTANCE + cameraDist, CALC_DISTANCE + cameraDist);
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

    //�J�[�\���œ�����Ȃ���������A�{���̋O�ՂŔ�����s��
    if (minIndex < 0) {
        //�R���W�����}�b�v�Ƃ̔���
        XMFLOAT3 gunVec = Float3Normalize(Float3Sub(gunTar, gunTop));
        data = RayCastData();
        data.start = gunTop;
        data.dir = gunVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, CALC_DISTANCE));
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
    EnemyBase* enemy = nullptr;
    if (minIndex >= 0) {
        gunTar = minEneHitPos;
        enemy = enemyList[minIndex];
    }
    ClearCollider();

    //�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[

    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
    bulletInfoList_[(int)type].coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;

    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->SetMove(move);
    pNewBullet->Shot(enemy, gunTar);
}