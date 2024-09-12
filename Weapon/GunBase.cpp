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
#include "../Other/InputManager.h"

//プレイヤーのアニメーション（リロード
#include "../Animation/AnimationController.h"
#include "../State/StateManager.h"

GunBase::GunBase(GameObject* parent, const std::string& name)
    : GameObject(parent, name), hModel_(-1), playerId_(0), coolTime_(0), rayHit_(false), rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1),
    isPeeking_(false), peekTime_(0), reloadTime_(0), currentReloadTime_(0), magazineCount_(0), currentMagazineCount_(0), hPlayerModel_(-1), handBoneIndex_(-1), 
    handPartIndex_(-1), animTime_(0), currentPeekTime_(0), peekZoom_(0.0f), hipFireAccuracy_(0.0f), aimingAccuracy_(0.0f), accuracyDecrease_(0.0f), 
    accuracyRecovery_(0.0f), currentAccuracy_(0.0f)
{
    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();
    hPlayerModel_ = pPlayer_->GetUpModelHandle();
    Model::GetPartBoneIndex(hPlayerModel_, "Weapon", &handPartIndex_, &handBoneIndex_);
}

GunBase::~GunBase()
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

float GunBase::GetAccuracy()
{
    if (isPeeking_) return aimingAccuracy_;
    return currentAccuracy_ + hipFireAccuracy_;
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

    //のぞき込み時間
    peekTime_ = gunSection["reloadTime"];
    currentPeekTime_ = peekTime_;

    //ズーム
    peekZoom_ = gunSection["peekValue"];
    peekTime_ = gunSection["peekTime"];

    //集弾率
    hipFireAccuracy_ = gunSection["hipFireAccuracy"];
    aimingAccuracy_ = gunSection["aimingAccuracy"];
    accuracyDecrease_ = gunSection["accuracyDecrease"];
    accuracyRecovery_ = gunSection["accuracyRecovery"];
    maxAccuracy_ = gunSection["maxAccuracy"];

}

void GunBase::SetGunHandPosition()
{
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;
}

void GunBase::EnterReload()
{
    //reloadTimeに合わせて、アニメーションの再生速度計算して再生
    int reloadS = pPlayer_->GetAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).startFrame;
    int reloadE = pPlayer_->GetAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).endFrame;
    int reloadAnimTime = (reloadE - reloadS);
    float animSpeed = (float)reloadAnimTime / (float)reloadTime_;
    pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);
    pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);

}

bool GunBase::Reload()
{
    currentReloadTime_--;

    //リロード完了
    if (currentReloadTime_ <= 0) {
        currentMagazineCount_ = magazineCount_;
        FinishedReload();
        return true;
    }

    return false;
}

void GunBase::FinishedReload()
{
    Model::SetAnimFrame(hModel_, 0, 0, 0.0f);

    //プレイヤーのStateがIdleかJumpだったら
    std::string stateName = pPlayer_->GetStateManager()->GetName();
    if (stateName == "Idle" || /*修正箇所*/ stateName == "Move") {
        pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
        pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
    }
}

//プレイヤーのアニメーションのかかる時間と武器に持たせたかかる時間で計算してAnimationSpeedを計算してやる
//ただし、Animation開始とかのコードはPlayerStateに置くこと
//IsCmdDownとIsCmdUpはなるべく使うな
//ジャンプ中のアニメーションやけど、カメラを遅れてついてくるようにするだけでいいけど
//再生するアニメーションはジャンプにする、他のAnimは止めていいかな、それとジャンプ中は覗き込みできないようにする
void GunBase::Peeking()
{
    bool playerClimb = pPlayer_->IsClimb();
    if (playerClimb || currentReloadTime_ > 0) {
        isPeeking_ = false;
        currentPeekTime_++;

        if (currentPeekTime_ >= peekTime_) {
            currentPeekTime_ = peekTime_;
        }

        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetFovAngleParcent(zoom, playerId_);
        return;
    }

    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        currentPeekTime_--;

        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetFovAngleParcent(zoom, playerId_);

        //覗き込み完了
        if (currentPeekTime_ <= 0) {
            currentPeekTime_ = 0;
            isPeeking_ = true;
        }
    }
    else {
        isPeeking_ = false;
        currentPeekTime_++;

        if (currentPeekTime_ >= peekTime_) {
            currentPeekTime_ = peekTime_;
        }
        
        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetFovAngleParcent(zoom, playerId_);
    }

}

void GunBase::ShotVFX()
{
    //ショットエフェクト
    EFFEKSEERLIB::EFKTransform t;
    Transform transform;
    transform.position_ = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    transform.rotate_ = transform_.rotate_;
    transform.rotate_.x = -transform.rotate_.x;
    transform.rotate_.y += 180.0f;
    transform.scale_ = transform_.scale_;
    DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
    t.isLoop = false;   //繰り返し
    t.maxFrame = 20;    //80フレーム
    t.speed = 1.0;      //スピード
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
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
    XMFLOAT3 bureMove = XMFLOAT3();
    float accuracy = GetAccuracy();
    bureMove.x = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
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
    XMFLOAT3 stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

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
        XMFLOAT3 GunBaseVec = Float3Normalize(Float3Sub(stageHitPos, GunBaseTop));
        data = RayCastData();
        data.start = GunBaseTop;

        vecDir = XMVector3TransformCoord(XMLoadFloat3(&GunBaseVec), matRotX * matRotY * matRotZ);
        XMStoreFloat3(&shotVec, vecDir);

        data.dir = shotVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
    }
    ClearCollider();

    XMFLOAT3 endTarget = stageHitPos;

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
    pBullet->Shot(chara, stageHitPos, minHitPos);
}

void GunBase::ShotFPSBullet(BulletBase* pBullet)
{
    float bulletSpeed = pBullet->GetBulletParameter().speed_;
    float calcDist = bulletSpeed * pBullet->GetBulletParameter().killTimer_;
    coolTime_ = pBullet->GetBulletParameter().shotCoolTime_;
    pBullet->SetPlayerId(playerId_);

    //カメラ情報計算
    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);
    XMFLOAT3 cameraVec = Float3Sub(cameraTar, cameraPos);
    cameraVec = Float3Normalize(cameraVec);

    //ブレの方向計算をする
    static const float BURE_POWER = 0.3f;
    float accuracy = GetAccuracy();
    XMFLOAT3 bureMove = XMFLOAT3();
    bureMove.x = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    XMMATRIX matRotX = XMMatrixRotationX(bureMove.x);
    XMMATRIX matRotY = XMMatrixRotationY(bureMove.y);
    XMMATRIX matRotZ = XMMatrixRotationZ(bureMove.z);
    XMVECTOR vecDir = XMVector3TransformCoord(XMLoadFloat3(&cameraVec), matRotX * matRotY * matRotZ);
    XMFLOAT3 shotVec = XMFLOAT3();
    XMStoreFloat3(&shotVec, vecDir);

    //コリジョンマップで着弾地点計算
    RayCastData data;
    data.start = cameraPos;
    data.dir = shotVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //表示用の情報セット
    XMFLOAT3 GunBaseTop = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    XMFLOAT3 drawMove = Float3Sub(stageHitPos, GunBaseTop);
    drawMove = Float3Multiply(Float3Normalize(drawMove), bulletSpeed);
    pBullet->SetDrawPosition(GunBaseTop);
    pBullet->SetDrawMove(drawMove);

    //計算用の情報セット
    XMFLOAT3 move = Float3Sub(stageHitPos, cameraPos);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);
    pBullet->SetPosition(cameraPos);
    pBullet->SetMove(move);

    pBullet->Shot(nullptr, stageHitPos, XMFLOAT3());

}