#include "GunBase.h"
#include "BulletBase.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Json/JsonReader.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"

GunBase::GunBase(GameObject* parent, const std::string& name)
    : GameObject(parent, name), hModel_(-1), pAimCursor_(nullptr), playerId_(0), coolTime_(0), rayHit_(false), 
    rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1), isFirstPerson_(false), 
    isPeeking_(false), peekTime_(0), reloadTime_(0), currentReloadTime_(0), magazineCount_(0), currentMagazineCount_(0),
    hPlayerModel_(-1), handBoneIndex_(-1), handPartIndex_(-1)
{
    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();
    hPlayerModel_ = pPlayer_->GetModelHandle();
    Model::GetPartBoneIndex(hPlayerModel_, "Weapon", &handPartIndex_, &handBoneIndex_);

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

void GunBase::ShotBullet(BulletBase* pBullet)
{
    coolTime_ = pBullet->GetBulletParameter().shotCoolTime_;
    float calcDist = pBullet->GetBulletParameter().speed_ * pBullet->GetBulletParameter().killTimer_;
    pBullet->SetPlayerId(playerId_);

    //色々情報計算
    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);
    XMFLOAT3 GunBaseTop = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    XMFLOAT3 GunBaseRoot = Model::GetBonePosition(hModel_, rootPartIndex_, rootBoneIndex_);
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

    float bulletSpeed = pBullet->GetBulletParameter().speed_;
    XMFLOAT3 move = Float3Sub(endTarget, GunBaseTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pBullet->SetMove(move);
    pBullet->SetPosition(GunBaseTop);
    pBullet->Shot(chara, GunBaseTar, minHitPos);
}

void GunBase::LoadGunJson(std::string fileName)
{
    //銃弾ごとの設定を読み込む
    JsonReader::Load("Json/Weapon.json");
    auto& gunSection = JsonReader::GetSection(fileName);

    //マガジン数の初期化
    magazineCount_ = gunSection["magazineCount"];
    currentMagazineCount_ = magazineCount_;

    //リロード時間の初期化
    reloadTime_ = gunSection["reloadTime"];
    currentReloadTime_ = 0;
}

void GunBase::SetGunHandPosition()
{
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;
}

void GunBase::Reload()
{
    currentReloadTime_--;

    //リロード官僚
    if (currentReloadTime_ <= 0) {
        currentMagazineCount_ = magazineCount_;
    }
}
