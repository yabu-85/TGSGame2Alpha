#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"

namespace
{
    XMFLOAT3 handOffset = { 0.3f, 0.5f, 0.1f };      // �ړ���
    XMFLOAT3 modelScale = { 0.3f, 0.3f, 0.3f };         // ���f���T�C�Y
    std::string modelName = "Model/Rifle.fbx";         // ���f����
}

Gun::Gun(GameObject* parent)
    :GameObject(parent, "Gun"), hModel_(-1), moveDirection_{ 0,0,0 }
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
    Player* pPlayer = static_cast<Player*>(FindObject("Player"));
    transform_.rotate_.x = -pPlayer->GetAim()->GetRotate().x;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void Gun::Release()
{
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
    // ����͍��p�x�ŏ�������̂ŁA�킩��ɂ�������Find�ł͂Ȃ��e�����ǂ��Đ����iAim->Player->PlayScene�j
    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());

    // �p�����[�^�ݒ�
    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    bulletInfoList_[(int)type].coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;

    XMFLOAT3 GunTop = Model::GetBonePosition(hModel_, "Top");
    XMFLOAT3 GunRoot = Model::GetBonePosition(hModel_, "Root");
    XMFLOAT3 move = CalculateBulletMovement(GunTop, GunRoot, bulletSpeed);
    moveDirection_ = move;

    pNewBullet->SetPosition(GunTop);
    pNewBullet->SetMove(move);
    pNewBullet->Shot();
}