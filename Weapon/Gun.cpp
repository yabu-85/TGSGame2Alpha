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

//リロード
//反動
//覗き込み
//覗き込みスナイパーの場合一人称に

//フォートナイトとRemnant2を合わせた感じにする
//HPGauge・銃弾数（5/30）・シールドかスタミナ・アイテム
//その他の情報表示

namespace
{
    XMFLOAT3 handOffset = { 0.2f, 0.7f, 0.1f };      // 移動量
    XMFLOAT3 modelScale = { 0.3f, 0.3f, 0.3f };      // モデルサイズ
    std::string modelName = "Model/Rifle.fbx";       // モデル名

    //当たり判定距離
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
    // データのロード
    hModel_ = Model::Load(modelName);
    assert(hModel_ >= 0);

    Model::GetBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

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
    transform_.rotate_.x = GameManager::GetPlayer()->GetAim()->GetRotate().x;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void Gun::Release()
{
}

void Gun::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos)
    {
        rayHit_ = true;
    }
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
    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 cameraVec = Float3Normalize(Float3Sub(Camera::GetTarget(), Camera::GetPosition()));
    float cameraDist = CalculationDistance(Camera::GetPosition(), Camera::GetTarget());

    RayCastData data;
    data.start = Camera::GetPosition();
    data.dir = cameraVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, CALC_DISTANCE + cameraDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    //敵と当たったか、最短距離を計算
    std::vector<EnemyBase*>& enemyList = EnemyManager::GetAllEnemy();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minEneHitPos = XMFLOAT3();

    //コライダー登録
    XMFLOAT3 centerPos = Float3Sub(Camera::GetPosition(), GetWorldPosition());
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&cameraVec));
    collid->size_ = XMFLOAT3(CALC_DISTANCE + cameraDist, CALC_DISTANCE + cameraDist, CALC_DISTANCE + cameraDist);
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    AddCollider(collid);

    for (int i = 0; i < enemyList.size(); i++) {
        XMFLOAT3 vec = Float3Sub(enemyList[i]->GetPosition(), data.start);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //壁に当たる距離じゃないから、当たり判定やる
        rayHit_ = false;
        this->Collision(enemyList[i]);

        //最短距離で当たった
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minEneHitPos = collid->targetPos_;
        }
    }

    //カーソルで当たらなかったから、本来の軌跡で判定を行う
    if (minIndex < 0) {
        //コリジョンマップとの判定
        XMFLOAT3 gunVec = Float3Normalize(Float3Sub(gunTar, gunTop));
        data = RayCastData();
        data.start = gunTop;
        data.dir = gunVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, CALC_DISTANCE));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

        //コライダー情報セット
        XMFLOAT3 gunRoot = Model::GetBonePosition(hModel_, rootBoneIndex_, rootPartIndex_);
        centerPos = Float3Sub(gunRoot, GetWorldPosition());
        collid->SetCenter(centerPos);
        collid->SetVector(XMLoadFloat3(&gunVec));

        for (int i = 0; i < enemyList.size(); i++) {
            XMFLOAT3 vec = Float3Sub(enemyList[i]->GetPosition(), data.start);
            float hitDist = CalculationDistance(vec);
            if (hitDist > data.dist) continue;

            //壁に当たる距離じゃないから、当たり判定やる
            rayHit_ = false;
            this->Collision(enemyList[i]);

            //最短距離で当たった
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

    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー

    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
    bulletInfoList_[(int)type].coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;

    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->SetMove(move);
    pNewBullet->Shot(enemy, gunTar);
}