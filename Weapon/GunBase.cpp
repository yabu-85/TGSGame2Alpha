#include "GunBase.h"
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
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"
#include "../Scene/TestScene.h"

GunBase::GunBase(GameObject* parent, const std::string& name)
    : GameObject(parent, name), hModel_(-1), pPlayer_(nullptr), pAimCursor_(nullptr), playerId_(0), coolTime_(0), rayHit_(false), 
    rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1)
{
    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();

}

GunBase::~GunBase()
{
}

void GunBase::Initialize()
{
}

void GunBase::Update()
{
}

void GunBase::Draw()
{
}

void GunBase::Release()
{
}

void GunBase::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos ||
        pTarget->GetObjectName().find("Player") != std::string::npos)
    {
        rayHit_ = true;
    }
}

template <class T>
void GunBase::ShootBullet()
{
    //Bulletインスタンス作成
    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
    coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;
    float calcDist = pNewBullet->GetBulletParameter().speed_ * pNewBullet->GetBulletParameter().killTimer_;
    pNewBullet->SetPlayerId(playerId_);

    //色々情報計算
    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);

    XMFLOAT3 GunBaseTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 GunBaseRoot = Model::GetBonePosition(hModel_, rootBoneIndex_, rootPartIndex_);
    XMFLOAT3 cameraVec = Float3Sub(cameraTar, cameraPos);
    float cameraDist = CalculationDistance(cameraVec);
    cameraVec = Float3Normalize(cameraVec);

    //ブレの方向計算をする
    static const float BURE_POWER = 0.3f;
    float bure = pAimCursor_->GetBurePower();
    XMFLOAT3 bureMove = XMFLOAT3();
    bureMove.x = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    XMMATRIX matRotX = XMMatrixRotationX(bureMove.x);
    XMMATRIX matRotY = XMMatrixRotationY(bureMove.y);
    XMMATRIX matRotZ = XMMatrixRotationZ(bureMove.z);
    XMVECTOR vecDir = XMVector3TransformCoord(XMLoadFloat3(&cameraVec), matRotX * matRotY * matRotZ);
    XMFLOAT3 shotVec = XMFLOAT3();
    XMStoreFloat3(&shotVec, vecDir);

    //コリジョンマップで着弾地点計算
    float weaponDist = CalculationDistance(GunBaseTop, GunBaseRoot);
    RayCastData data;
    data.start = Float3Add(cameraTar, Float3Multiply(cameraVec, weaponDist));
    data.dir = shotVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist + cameraDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 GunBaseTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    //対象にあたったか最短距離を計算
    std::vector<Character*>charaList = CharacterManager::GetCharacterList();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minHitPos = XMFLOAT3();

    //コライダー登録
    XMFLOAT3 centerPos = Float3Sub(cameraPos, GetWorldPosition());
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&data.dir));
    collid->size_ = XMFLOAT3(calcDist + cameraDist, calcDist + cameraDist, calcDist + cameraDist);
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    collid->typeList_.push_back(OBJECT_TYPE::Player);
    AddCollider(collid);

    for (int i = 0; i < charaList.size(); i++) {
        //親はスキップ
        if (charaList[i] == pPlayer_) continue;

        //距離計算
        XMFLOAT3 vec = Float3Sub(charaList[i]->GetPosition(), data.start);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //壁に当たる距離じゃないから、当たり判定やる
        rayHit_ = false;
        this->Collision(charaList[i]);

        //最短距離で当たった
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minHitPos = collid->targetPos_;
        }
    }

    //Aimで当たらなかったから、本来の軌跡で判定を行う
    if (minIndex < 0) {
        //コリジョンマップとの判定
        XMFLOAT3 GunBaseVec = Float3Normalize(Float3Sub(GunBaseTar, GunBaseTop));
        data = RayCastData();
        data.start = GunBaseTop;

        vecDir = XMVector3TransformCoord(XMLoadFloat3(&GunBaseVec), matRotX * matRotY * matRotZ);
        XMStoreFloat3(&shotVec, vecDir);
        
        data.dir = shotVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        GunBaseTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
    }
    ClearCollider();

    XMFLOAT3 endTarget = GunBaseTar;

    //敵を目標にしていた場合
    Character* chara = nullptr;
    //速度とか関係ない一瞬で到達する弾ならGunBaseで判定するだけで済むから、残しておく
    if (minIndex >= 0) {
        chara = charaList[minIndex];
        endTarget = minHitPos;
    }

    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    XMFLOAT3 move = Float3Sub(endTarget, GunBaseTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pNewBullet->SetMove(move);
    pNewBullet->SetPosition(GunBaseTop);
    pNewBullet->Shot(chara, GunBaseTar, minHitPos);
}