#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Model.h"
#include "../Engine/Direct3D.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"

namespace
{
    XMFLOAT3 handOffset = { 0.3f, 0.5f, 0.1f };      // 移動量
    XMFLOAT3 modelScale = { 0.3f, 0.3f, 0.3f };         // モデルサイズ
    std::string modelName = "Model/Rifle.fbx";         // モデル名
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
    // データのロード
    hModel_ = Model::Load(modelName);
    assert(hModel_ >= 0);

    //プレイヤーの手の位置まで調整
    transform_.position_ = handOffset;
    transform_.scale_ = modelScale;

    // バレットタイプのenumサイズで初期化
    bulletInfoList_.resize(static_cast<int>(BulletType::MAX));
}

void Gun::Update()
{
    // クールタイムを減らす
    for (auto& bullet : bulletInfoList_) { bullet.coolTime_--; }

    // 通常射撃
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
    // 射出方向を計算して正規化  (top - root)
    XMVECTOR vMove = XMVector3Normalize(XMVectorSubtract(XMLoadFloat3(&top), XMLoadFloat3(&root)));

    // 弾速を追加設定(銃に種類が増えたとき用)
    vMove *= bulletSpeed;

    // float3型に戻す
    XMFLOAT3 move;
    XMStoreFloat3(&move, vMove);
    return move;
}

template <class T>
void Gun::ShootBullet(BulletType type)
{
    // これは高頻度で処理するので、わかりにくいけどFindではなく親をたどって生成（Aim->Player->PlayScene）
    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());

    // パラメータ設定
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