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
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"
#include "../Scene/TestScene.h"

namespace {
    XMFLOAT3 handOffset = { 0.2f, 0.7f, 0.1f };      // 移動量

}

Gun::Gun(GameObject* parent)
    : GameObject(parent, "Gun"), hModel_(-1), pPlayer_(nullptr), pAimCursor_(nullptr), playerId_(0), coolTime_(0), rayHit_(false), 
    rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1)
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();

    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    //プレイヤーの手の位置まで調整
    transform_.position_ = handOffset;

}

void Gun::Update()
{
    // クールタイムを減らす
    coolTime_--;
    pAimCursor_->Update();

    // 通常射撃
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_))
    {
        //まだクールタイム中
        if (coolTime_ > 0) return;

        ShootBullet<Bullet_Normal>(BulletType::NORMAL);
        pAimCursor_->Shot();

        CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 1.0f), 3);
        pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
        pPlayer_->GetAim()->SetCameraRotateReturn(false);

        //ヒットエフェクト
        EFFEKSEERLIB::gEfk->AddEffect("GUNSHOT", "Particle/gunShot.efk");
        EFFEKSEERLIB::EFKTransform t;
        Transform transform;
        transform.position_ = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
        transform.rotate_ = Float3Add(pPlayer_->GetRotate(), transform_.rotate_);
        transform.rotate_.y += +180.0f;
        transform.rotate_.x = -transform.rotate_.x;
        transform.scale_ = transform_.scale_;
        DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
        t.isLoop = false;   //繰り返し
        t.maxFrame = 20;    //80フレーム
        t.speed = 1.0;      //スピード
        mt = EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
    }
    else {
        //連続で打つのやめた時だけ、RotateShake戻り処理をTrueに
        if(coolTime_ >= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

    }

}

void Gun::Draw()
{
    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void Gun::Release()
{
}

void Gun::OnCollision(GameObject* pTarget)
{
    // 敵に当たったとき
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos ||
        pTarget->GetObjectName().find("Player") != std::string::npos)
    {
        rayHit_ = true;
    }
}

template <class T>
void Gun::ShootBullet(BulletType type)
{
    //Bulletインスタンス作成
    BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
    coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;
    float calcDist = pNewBullet->GetBulletParameter().speed_ * pNewBullet->GetBulletParameter().killTimer_;
    pNewBullet->SetPlayerId(playerId_);

    //色々情報計算
    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 gunRoot = Model::GetBonePosition(hModel_, rootBoneIndex_, rootPartIndex_);
    XMFLOAT3 cameraVec = Float3Sub(Camera::GetTarget(playerId_), Camera::GetPosition(playerId_));
    float cameraDist = CalculationDistance(cameraVec);
    cameraVec = Float3Normalize(cameraVec);

    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);

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
    float weaponDist = CalculationDistance(gunTop, gunRoot);
    RayCastData data;
    data.start = Float3Add(Camera::GetTarget(playerId_), Float3Multiply(cameraVec, weaponDist));
    data.dir = shotVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist + cameraDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //ーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーーー
    //対象にあたったか最短距離を計算
    std::vector<Character*>charaList = CharacterManager::GetCharacterList();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minHitPos = XMFLOAT3();

    //コライダー登録
    XMFLOAT3 centerPos = Float3Sub(Camera::GetPosition(playerId_), GetWorldPosition());
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
        XMFLOAT3 gunVec = Float3Normalize(Float3Sub(gunTar, gunTop));
        data = RayCastData();
        data.start = gunTop;

        vecDir = XMVector3TransformCoord(XMLoadFloat3(&gunVec), matRotX * matRotY * matRotZ);
        XMStoreFloat3(&shotVec, vecDir);
        
        data.dir = shotVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

        /*即到達弾のため残しておく
        //コライダー情報セット
        centerPos = Float3Sub(gunRoot, GetWorldPosition());
        collid->SetCenter(centerPos);
        collid->SetVector(XMLoadFloat3(&data.dir));

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
        */
        
    }
    ClearCollider();

    //敵を目標にしていた場合
    Character* chara = nullptr;
    /*  速度とか関係ない一瞬で到達する弾ならGunで判定するだけで済むから、残しておく
    if (minIndex >= 0) {
        chara = charaList[minIndex];
    }
    */

    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pNewBullet->SetMove(move);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->Shot(chara, gunTar, minHitPos);
}