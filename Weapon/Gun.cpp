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

}

Gun::Gun(GameObject* parent)
    :GameObject(parent, "Gun"), hModel_(-1), pPlayer_(nullptr), pAimCursor_(nullptr), playerId_(0)
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
    transform_.scale_ = modelScale;

}

void Gun::Update()
{
    // クールタイムを減らす
    bulletInfo_.coolTime_--;

    // 通常射撃
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_) && bulletInfo_.coolTime_ <= 0)
    {
        ShootBullet<Bullet_Normal>(BulletType::NORMAL);

        CameraShakeInfo shakeInfo = CameraShakeInfo(1, 0.05f, 0.1f);
        XMVECTOR shakeDir = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        pPlayer_->GetAim()->SetCameraShake(shakeInfo);
        pPlayer_->GetAim()->SetCameraShakeDirection(shakeDir);

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
    bulletInfo_.coolTime_ = pNewBullet->GetBulletParameter().shotCoolTime_;
    int killTimer = pNewBullet->GetBulletParameter().killTimer_;
    float bulletSpeed = pNewBullet->GetBulletParameter().speed_;
    float calcDist = bulletSpeed * killTimer;

    //色々情報計算
    XMFLOAT3 gunTop = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
    XMFLOAT3 cameraVec = Float3Normalize(Float3Sub(Camera::GetTarget(playerId_), Camera::GetPosition(playerId_)));
    float cameraDist = CalculationDistance(Camera::GetPosition(playerId_), Camera::GetTarget(playerId_));
    
    //コリジョンマップで着弾地点計算
    RayCastData data;
    data.start = Camera::GetPosition(playerId_);
    data.dir = cameraVec;
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
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&cameraVec));
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
        data.dir = gunVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        gunTar = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

        //コライダー情報セット
        XMFLOAT3 gunRoot = Model::GetBonePosition(hModel_, rootBoneIndex_, rootPartIndex_);
        centerPos = Float3Sub(gunRoot, GetWorldPosition());
        collid->SetCenter(centerPos);
        collid->SetVector(XMLoadFloat3(&gunVec));

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
    }
    ClearCollider();

    //敵を目標にしていた場合
    Character* chara = nullptr;
    if (minIndex >= 0) {
        gunTar = minHitPos;
        chara = charaList[minIndex];
    }

    XMFLOAT3 move = Float3Sub(gunTar, gunTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pNewBullet->SetMove(move);
    pNewBullet->SetPosition(gunTop);
    pNewBullet->Shot(chara, gunTar);
}