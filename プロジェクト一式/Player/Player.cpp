#include "Player.h"
#include "Aim.h"
#include "../Engine/Model.h"
#include "../Engine/Image.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/CapsuleCollider.h"
#include "../State/PlayerState.h"
#include "../State/StateManager.h"
#include "../Other/InputManager.h"
#include "../Stage/CollisionMap.h"
#include "../Other/GameManager.h"
#include "../UI/AimCursor.h"
#include "../UI/HealthGauge.h"
#include "../UI/FixedHealthGauge.h"
#include "../Character/DamageSystem.h"
#include "../Weapon/Gun.h"
#include "../Weapon/SniperGun.h"
#include "../Animation/AnimationController.h"
#include "../Json/JsonReader.h"
#include "../Scene/PlayScene.h"

namespace {
    const float stopGradually = 0.21f;      //移動スピードの加減の値止まるとき
    const float moveGradually = 0.09f;      //移動スピードの加減の値移動時
    const float maxmoveSpeed_ = 0.7f;       //最大移動スピード
    const float rotateRatio = 0.1f;         //通常時のRotateRatio
    const float JumpPower = 0.10f;          //ジャンプの強さ
    const float WorldGravity = 0.005f;      //世界の重力
    const float PlayerHeight = 1.3f;        //プレイヤーの頭の位置（Stageとの判定で使う
    const float PlayerWaist = 0.4f;         //プレイヤーの上れる高さ
    const int DAMAGE_DRAW_TIME = 30;        //ダメージエフェクト表示時間

    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 10.0f, 50.0f);

    //Orientのデータ
    int upOrientBoneSize = 0;
    const std::pair<std::string, std::string> boneNameList[] = {
    { "upper_arm.R",    "" },
    { "forearm.R",      "upper_arm.R" },
    { "hand.R",         "upper_arm.R" },
    { "palm.02.R",      "upper_arm.R" },
    { "Weapon",         "upper_arm.R" },
    { "f_middle.01.R",  "upper_arm.R" },

    { "upper_arm.L",    "" },
    { "forearm.L",      "upper_arm.L" },
    { "hand.L",         "upper_arm.L" },
    { "palm.02.L",      "upper_arm.L" },
    { "f_middle.01.L",  "upper_arm.L" },

    { "spine.004",  ""},
    { "spine.005",  "spine.004" },
    { "eye",        "spine.004" },
    { "eye.001",    "spine.004" },
    };
    
}

Player::Player(GameObject* parent)
    : Character(parent, "Player"), pAim_(nullptr), pGunBase_(nullptr), pStateManager_(nullptr), pCapsuleCollider_(nullptr),
    playerMovement_(0, 0, 0), gradually_(0.0f), climbPos_(XMFLOAT3()), isFly_(true), isClimb_(false), isCreative_(false), 
    gravity_(0.0f), moveSpeed_(0.0f), playerId_(0), bonePart_(-1), lowerBodyRotate_(0.0f), healthGaugeDrawTime_(0), damageDrawTime_(0),
    pUpAnimationController_(nullptr), pDownAnimationController_(nullptr), pFpsAnimationController_(nullptr), isActionReady_(true),
    hUpModel_(-1), hDownModel_(-1), hFPSModel_(-1), hPict_(-1)
{
    for (int i = 0; i < 15; i++) upListIndex_[i] = -1;
    for (int i = 0; i < 8; i++) downListIndex_[i] = -1;
}

Player::~Player()
{
}

void Player::Initialize()
{
    //モデル読み込みとAssert
    hUpModel_ = Model::Load("Model/gunFighterUp.fbx");
    hDownModel_ = Model::Load("Model/gunFighterDown.fbx");
    hFPSModel_ = Model::Load("Model/gunFighterFPS.fbx");
    assert(hUpModel_ >= 0);
    assert(hDownModel_ >= 0);
    assert(hFPSModel_ >= 0);

    hPict_ = Image::Load("Image/damage.png");
    assert(hPict_ >= 0);
    Image::SetFullScreenTransform(hPict_);

    //Orient登録
    bonePart_ = Model::GetPartIndex(hUpModel_, "thigh.L");
    upOrientBoneSize = (int)sizeof(boneNameList) / sizeof(boneNameList[0]);
    for (int i = 0; i < upOrientBoneSize;i++) {
        upListIndex_[i] = Model::AddOrientRotateBone(hUpModel_, boneNameList[i].first, boneNameList[i].second);
        Model::AddOrientRotateBone(hFPSModel_, boneNameList[i].first, boneNameList[i].second);
    }

    //アニメーションデータのセットフレームはヘッダに書いてる
    pUpAnimationController_ = new AnimationController(hUpModel_, this);
    pDownAnimationController_ = new AnimationController(hDownModel_, this);
    pFpsAnimationController_ = new AnimationController(hFPSModel_, this);
    for (int i = 0; i < (int)PLAYER_ANIMATION::MAX; i++) pUpAnimationController_->AddAnim(PLAYER_ANIMATION_DATA[i][0], PLAYER_ANIMATION_DATA[i][1]);
    for (int i = 0; i < (int)PLAYER_ANIMATION::MAX; i++) pDownAnimationController_->AddAnim(PLAYER_ANIMATION_DATA[i][0], PLAYER_ANIMATION_DATA[i][1]);
    for (int i = 0; i < (int)PLAYER_ANIMATION::MAX; i++) pFpsAnimationController_->AddAnim(PLAYER_ANIMATION_DATA[i][0], PLAYER_ANIMATION_DATA[i][1]);

    pUpAnimationController_->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
    pDownAnimationController_->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
    pFpsAnimationController_->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);

    //PlayerIDセット
    if (GameManager::GetPlayer(0)) playerId_ = 1;
    GameManager::SetPlayer(this, playerId_);

    //パラメータセット
    objectType_ = OBJECT_TYPE::Player;
    SetBodyRange(0.2f);
    SetBodyWeight(1.0f);
    SetBodyHeightHalf(1.0f);
    
    //PlayerSetting読み込み
    JsonReader::Load("Json/GameSetting.json");
    int hp = 0;
    if (playerId_ == 0) {
        auto& section = JsonReader::GetSection("Player1");
        hp = (int)section["hp"];
    }
    else {
        auto& section = JsonReader::GetSection("Player2");
        hp = (int)section["hp"];
    }

    SetMaxHP(hp);
    SetHP(hp);
    moveSpeed_ = 0.07f;
    GameManager::playerSpeed = moveSpeed_;

    Model::SetTransform(hUpModel_, transform_);
    Model::SetTransform(hFPSModel_, transform_);
    Model::SetTransform(hDownModel_, transform_);

    //HealthGauge
    pFixedHealthGauge_ = new FixedHealthGauge(this, XMFLOAT2(3.0f, 2.0f));
    pFixedHealthGauge_->SetOffSetPosition(XMFLOAT2(-0.8f, -0.8f));

    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.0f, 1.7f));

    //Aim / AimCursor
    pAim_ = Instantiate<Aim>(this);
    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    //Gun
    pGunBase_ = Instantiate<Gun>(this);
    //pGunBase_ = Instantiate<SniperGun>(this);
    
    pStateManager_ = new StateManager(this);
    pStateManager_->AddState(new PlayerIdle(pStateManager_));
    pStateManager_->AddState(new PlayerMove(pStateManager_));
    pStateManager_->AddState(new PlayerJump(pStateManager_));
    pStateManager_->AddState(new PlayerClimb(pStateManager_));
    pStateManager_->AddState(new PlayerDead(pStateManager_));
    pStateManager_->ChangeState("Idle");

    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    pCapsuleCollider_ = new CapsuleCollider(XMFLOAT3(0.0f, 0.65f, 0.0f), 0.3f, 0.65f, vec);
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Stage);
    AddCollider(pCapsuleCollider_);

    for (int i = 0; i < 2; i++) {
        pSphereCollider_[i] = new SphereCollider(XMFLOAT3(), pCapsuleCollider_->size_.x);
        pSphereCollider_[i]->pGameObject_ = this;
    }
    pSphereCollider_[0]->center_ = XMFLOAT3(pCapsuleCollider_->center_.x, pCapsuleCollider_->center_.y - (pCapsuleCollider_->height_ * 0.5f), pCapsuleCollider_->center_.z);
    pSphereCollider_[1]->center_ = XMFLOAT3(pCapsuleCollider_->center_.x, pCapsuleCollider_->center_.y + (pCapsuleCollider_->height_ * 0.5f), pCapsuleCollider_->center_.z);

}

void Player::Update()
{
    //アニメーションの再生
    //銃の覗き込みはGunBaseから
    //ReloadはGunBaseから
    //着地はPlayerUpdateの中から
    //それ以外はPlayerStateの中からしている

    if (Input::IsKeyDown(DIK_F)) {
        Model::AddBlend(hDownModel_, 400, 630, 1.0f, true, 1.0f, 0.01f);
        Model::AddBlend(hUpModel_, 400, 630, 1.0f, true, 1.0f, 0.01f);
        Model::SetBlend(hDownModel_, true);
        Model::SetBlend(hUpModel_, true);
    }

    //アニメーション
    Model::Update(hUpModel_);
    Model::Update(hFPSModel_);
    Model::Update(hDownModel_);

    //AnimCtrl
    pUpAnimationController_->Update();
    pDownAnimationController_->Update();
    pFpsAnimationController_->Update();
    
    //Orient上下視点/腰下
    orientRotateX_ = -pAim_->GetRotate().x;
    for (int i = 0; i < upOrientBoneSize; i++) Model::SetOrietnRotateBone(hUpModel_, upListIndex_[i], XMFLOAT3(orientRotateX_, 0.0f, 0.0f));
    for (int i = 0; i < upOrientBoneSize; i++) Model::SetOrietnRotateBone(hFPSModel_, upListIndex_[i], XMFLOAT3(orientRotateX_, 0.0f, 0.0f));

    //AimCursor
    if(pGunBase_) pAimCursor_->SetAccuracyParce(pGunBase_->GetAccuracy());
    pAimCursor_->Update();

    //デバッグ用
#if 0
    if (Input::IsKeyDown(DIK_Z)) transform_.position_ = START_POS;
    if (Input::IsKeyDown(DIK_M)) isCreative_ = !isCreative_;
    if (isCreative_) {
        if (Input::IsKey(DIK_SPACE)) playerMovement_.y += moveSpeed_ * 0.5f;
        else if (Input::IsKey(DIK_F)) playerMovement_.y -= moveSpeed_ * 0.5f;
        if (InputManager::CmdWalk(0)) CalcMove();
        else CalcNoMove();
        Move();

        XMFLOAT3 targetRot = Float3Add(transform_.position_, pAim_->GetAimDirection());
        TargetRotate(targetRot, 1.0f);

        moveSpeed_ = GameManager::playerSpeed;
        GameManager::playerClimb = isClimb_;
        GameManager::playerFaly = isFly_;
        return;
    }
#endif

    //ダメージエフェクト
    if (damageDrawTime_ > 0) {
        damageDrawTime_--;
        Image::SetAlpha(hPict_, (int)(255.0f * ((float)damageDrawTime_ / (float)DAMAGE_DRAW_TIME) ));
    }

    //Dead判定
    if (IsHealthZero() && pStateManager_->GetName() != "Dead") pStateManager_->ChangeState("Dead");

    //ステート
    pStateManager_->Update();

    //空中にいる
    if (isFly_) {

        //登り処理いったんなし
        /*
        if (InputManager::IsCmd(InputManager::JUMP, playerId_) && !IsClimb()) {
            CheckWallClimb();
            if(IsClimb()) pStateManager_->ChangeState("Climb");
        }
        */

        //重力処理
        gravity_ += WorldGravity;
        transform_.position_.y -= gravity_;
        
        StageRoofBounce();
        StageFloarBounce(0.0f, -1.0f);
        StageFloarBounce();
        StageWallBounce();

        //着地した
        if (!isFly_) {
            GetDownAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE);

            if (InputManager::CmdWalk(playerId_)) pStateManager_->ChangeState("Move");
            else pStateManager_->ChangeState("Idle");
        }
    }
    
    //地上・登り状態じゃないとき、地面に立っているか判定
    if (!isFly_ && !isClimb_) {
        isFly_ = true;
        StageWallBounce();
        StageRoofBounce();
        StageFloarBounce(0.2f);
    }
    
    ReflectCharacter();
    TargetRotate(Float3Add(transform_.position_, pAim_->GetAimDirection()), 1.0f);
    
    //Weapon用にここでSet
    Model::SetTransform(hFPSModel_, transform_);
    Model::SetTransform(hDownModel_, transform_);
    Model::SetTransform(hUpModel_, transform_);

    //GameManager情報
    moveSpeed_ = GameManager::playerSpeed;
    GameManager::playerClimb = isClimb_;
    GameManager::playerFaly = isFly_;
}

void Player::Draw()
{
    //自分の画面 & FPSの場合 & 影表示は全身
    if (GameManager::GetDrawIndex() == playerId_ && pAim_->IsAimFps() && Direct3D::GetCurrentShader() != Direct3D::SHADER_SHADOWMAP) {
        //Aimの差分に合わせて表示させる
        Transform t = transform_;
        XMFLOAT3 subAim = pAim_->GetFPSSubY();
        t.position_ = Float3Add(t.position_, subAim);
        Model::SetTransform(hFPSModel_, t);
        Model::Draw(hFPSModel_);

        //差分無しで上書き
        Model::SetTransform(hFPSModel_, transform_);

        //ダメージエフェクト
        if (damageDrawTime_ > 0) {
            Image::Draw(hPict_);
        }
    }
    //相手の表示
    else {
        Model::SetTransform(hUpModel_, transform_);
        Model::Draw(hUpModel_);

        CalcDownBodyRotate();
        Transform downT = transform_;
        downT.rotate_.y += lowerBodyRotate_;
        Model::SetTransform(hDownModel_, downT);
        Model::Draw(hDownModel_);
        
        //HealthGauge表示
        if (healthGaugeDrawTime_ > 0 && Direct3D::GetCurrentShader() != Direct3D::SHADER_SHADOWMAP) {
            healthGaugeDrawTime_--;
            float r = (float)GetHP() / (float)GetMaxHP();
            pHealthGauge_->SetParcent(r);
            pHealthGauge_->Draw(GameManager::GetDrawIndex());
        }
    
        //CollisionDraw
#ifdef _DEBUG
        if (Direct3D::GetCurrentShader() != Direct3D::SHADER_SHADOWMAP) CollisionDraw();
#endif

    }

}

void Player::Release()
{
}

void Player::OnDamageDealt(const DamageInfo& damageInfo)
{
    if (damageInfo.name == "P_Bullet") {
        if (playerId_ == 0) GameManager::GetPlayer(1)->SetHealthGaugeDrawTime(30);
        else GameManager::GetPlayer(0)->SetHealthGaugeDrawTime(30);
    }
}

void Player::OnDamageReceived(const DamageInfo& damageInfo)
{
    damageDrawTime_ = DAMAGE_DRAW_TIME;

}

void Player::CalcDownBodyRotate()
{
    static const float rotMaxY = 60.0f;     //回転制限
    static const float rotYBack = 100.0f;   //後ろ向き判定
    static const float rotRatio = 0.2f;     //回転スピード
    
    //歩いてる＆足ついてる
    if (InputManager::CmdWalk(playerId_) && !isFly_) {
        //-180 ～ 180の間に 
        while (transform_.rotate_.y <= -180.0f) transform_.rotate_.y += 360.0f;
        while (transform_.rotate_.y >= 180.0f) transform_.rotate_.y -= 360.0f;

        //移動中なら移動方向から向く方向計算
        float rotYYY = CalculationRotateXY(GetInputMove()).y - transform_.rotate_.y;

        //-180 ～ 180の間に 
        if (rotYYY >= 180.0f) rotYYY = -180.0f - (180.0f - rotYYY);
        if (rotYYY <= -180.0f) rotYYY = 180.0f + (180.0f + rotYYY);

        //後ろ向きの時は向きを反転させる
        if (rotYYY >= rotYBack || rotYYY <= -rotYBack) {
            if (rotYYY > 0) rotYYY -= 180.0f;
            else rotYYY += 180.0f;
        }

        //回転量制限
        if (rotYYY >= rotMaxY) rotYYY = rotMaxY;
        else if (rotYYY <= -rotMaxY) rotYYY = -rotMaxY;

        lowerBodyRotate_ = lowerBodyRotate_ + ((rotYYY - lowerBodyRotate_) * rotRatio);
        return;
    }

    //正面を向くように戻す
    lowerBodyRotate_ = lowerBodyRotate_ + ((0.0f - lowerBodyRotate_) * rotRatio);

}

//private関数：Rotateの計算する--------------------------------
void Player::CalcRotate(XMFLOAT3 pos, float ratio)
{
    XMFLOAT2 a = XMFLOAT2(sinf(XMConvertToRadians(transform_.rotate_.y)), cosf(XMConvertToRadians(transform_.rotate_.y)));
    XMVECTOR vA = XMVector2Normalize(XMLoadFloat2(&a));
    XMFLOAT2 b = XMFLOAT2(pos.x, pos.z );
    XMVECTOR vB = XMVector2Normalize(XMLoadFloat2(&b));
    XMStoreFloat2(&a, vA);
    XMStoreFloat2(&b, vB);
    float cross = a.x * b.y - a.y * b.x;
    float dot = a.x * b.x + a.y * b.y;
    transform_.rotate_.y += XMConvertToDegrees(-atan2f(cross, dot) * ratio);
}

void Player::TargetRotate(XMFLOAT3 pos, float ratio) { CalcRotate(Float3Sub(pos, transform_.position_), ratio); }
void Player::Rotate() { Rotate(rotateRatio); }
void Player::Rotate(float ratio) { CalcRotate(GetInputMove(), ratio); }

//-----------------------------------------------Rotate

XMFLOAT3 Player::GetInputMove()
{
    XMFLOAT3 fMove = { 0,0,0 };
    if (InputManager::CmdWalk(playerId_)) {
        gradually_ = moveGradually;

        XMFLOAT3 aimDirection = pAim_->GetAimDirection();
        aimDirection.y = 0.0f;
        aimDirection = Float3Normalize(aimDirection);
        int inputId = playerId_;

        //キーボード
        if (GameManager::IsPCCtrl()) {
            if (playerId_ == GameManager::GetPCCtrlNumber()) {
                if (InputManager::IsCmd(InputManager::MOVE_UP, playerId_)) {
                    fMove.x += aimDirection.x;
                    if (isCreative_) fMove.y += aimDirection.y;
                    fMove.z += aimDirection.z;
                }
                if (InputManager::IsCmd(InputManager::MOVE_LEFT, playerId_)) {
                    fMove.x -= aimDirection.z;
                    fMove.z += aimDirection.x;
                }
                if (InputManager::IsCmd(InputManager::MOVE_DOWN, playerId_)) {
                    fMove.x -= aimDirection.x;
                    if (isCreative_) fMove.y -= aimDirection.y;
                    fMove.z -= aimDirection.z;
                }
                if (InputManager::IsCmd(InputManager::MOVE_RIGHT, playerId_)) {
                    fMove.x += aimDirection.z;
                    fMove.z -= aimDirection.x;
                }
            }
            //コントローラー
            else {
                static const float DEAD_ZONE = 0.1f;
                XMFLOAT3 lMove = Input::GetPadStickL(0);
                if (lMove.y > DEAD_ZONE) {  //前
                    fMove.x += (aimDirection.x * abs(lMove.y));
                    fMove.z += (aimDirection.z * abs(lMove.y));
                }
                if (lMove.x < -DEAD_ZONE) { //左
                    fMove.x -= (aimDirection.z * abs(lMove.x));
                    fMove.z += (aimDirection.x * abs(lMove.x));
                }
                if (lMove.y < -DEAD_ZONE) { //下
                    fMove.x -= (aimDirection.x * abs(lMove.y));
                    fMove.z -= (aimDirection.z * abs(lMove.y));
                }
                if (lMove.x > DEAD_ZONE) {  //右
                    fMove.x += (aimDirection.z * abs(lMove.x));
                    fMove.z -= (aimDirection.x * abs(lMove.x));
                }
            }
        }
        //コントローラー
        else {
            static const float DEAD_ZONE = 0.1f;
            XMFLOAT3 lMove = Input::GetPadStickL(playerId_);
            if (lMove.y > DEAD_ZONE) {  //前
                fMove.x += (aimDirection.x * abs(lMove.y));
                fMove.z += (aimDirection.z * abs(lMove.y));
            }
            if (lMove.x < -DEAD_ZONE) { //左
                fMove.x -= (aimDirection.z * abs(lMove.x));
                fMove.z += (aimDirection.x * abs(lMove.x));
            }
            if (lMove.y < -DEAD_ZONE) { //下
                fMove.x -= (aimDirection.x * abs(lMove.y));
                fMove.z -= (aimDirection.z * abs(lMove.y));
            }
            if (lMove.x > DEAD_ZONE) {  //右
                fMove.x += (aimDirection.z * abs(lMove.x));
                fMove.z -= (aimDirection.x * abs(lMove.x));
            }
        }
    }

    XMVECTOR vMove = XMLoadFloat3(&fMove);
    vMove = XMVector3Normalize(vMove);
    XMStoreFloat3(&fMove, vMove);

    return fMove;
}

void Player::Move(float f)
{
    transform_.position_.x += ((playerMovement_.x * moveSpeed_) * f);
    transform_.position_.y += ((playerMovement_.y * moveSpeed_) * f);
    transform_.position_.z += ((playerMovement_.z * moveSpeed_) * f);
}

XMVECTOR Player::GetDirectionVec()
{
    XMVECTOR vMove = { 0.0, 0.0, 1.0, 0.0 };
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    vMove = XMVector3TransformCoord(vMove, mRotY);
    vMove = XMVector3Normalize(vMove);
    return vMove;
}

void Player::CalcMove()
{
    gradually_ = stopGradually;

    XMFLOAT3 fMove = GetInputMove();
    XMFLOAT3 move = { ((fMove.x - playerMovement_.x) * gradually_) , ((fMove.y - playerMovement_.y) * gradually_) , ((fMove.z - playerMovement_.z) * gradually_) };
    playerMovement_ = { playerMovement_.x + move.x , playerMovement_.y + move.y , playerMovement_.z + move.z };

    //MaxSpeed超えていたら正規化・MaxSpeedの値にする
    float currentSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&playerMovement_)));
    if (currentSpeed > maxmoveSpeed_) {
        XMVECTOR vMove;
        vMove = XMLoadFloat3(&playerMovement_);
        vMove = XMVector3Normalize(vMove);
        vMove *= maxmoveSpeed_;
        XMStoreFloat3(&playerMovement_, vMove);
    }
}

void Player::CalcNoMove()
{
    XMFLOAT3 move = { 0,0,0 };
    gradually_ = stopGradually;
    move = { ((move.x - playerMovement_.x) * gradually_) , ((move.y - playerMovement_.y) * gradually_) , ((move.z - playerMovement_.z) * gradually_) };
    playerMovement_ = { playerMovement_.x + move.x , playerMovement_.y + move.y , playerMovement_.z + move.z };
}

bool Player::StageFloarBounce(float perDist, float calcHeight)
{
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeight, transform_.position_.z);
    rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
    XMFLOAT3 pos = XMFLOAT3(transform_.position_.x, transform_.position_.y + calcHeight, transform_.position_.z);
    GameManager::GetCollisionMap()->CellFloarRayCast(pos, &rayData);
    if (rayData.dist <= PlayerHeight + perDist) {
        transform_.position_.y += PlayerHeight - rayData.dist;
        gravity_ = 0.0f;
        isFly_ = false;
        return true;
    }
    
    return false;
}

bool Player::StageWallBounce()
{
    //下
    XMVECTOR push = XMVectorZero(); 
    bool hit = GameManager::GetCollisionMap()->CellSphereVsTriangle(pSphereCollider_[0], push);

    //上
    push = XMVectorZero();
    if (GameManager::GetCollisionMap()->CellSphereVsTriangle(pSphereCollider_[1], push)) hit = true;
    
    return hit;
}

bool Player::StageRoofBounce()
{
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerWaist, transform_.position_.z);
    rayData.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
    GameManager::GetCollisionMap()->CellFloarRayCast(transform_.position_, &rayData);
    float calcSize = PlayerHeight - PlayerWaist;
    if (rayData.hit && rayData.dist < calcSize) {
        transform_.position_.y -= calcSize - rayData.dist;
        
        //誤差
        transform_.position_.y -= 0.05f;

        gravity_ = 0.0f;
        return true;
    }
    return false;
}

bool Player::IsReadyJump()
{
    return (!IsClimb() && !IsFly());
}

void Player::Jump()
{
    isFly_ = true;
    gravity_= -JumpPower;

}

void Player::WallClimb()
{
    XMFLOAT3 move = Float3Sub(climbPos_, transform_.position_);
    float dist = sqrtf(move.x * move.x + move.y * move.y + move.z * move.z);
    const float climdmoveSpeed_ = 0.1f;

    if (dist > climdmoveSpeed_) move = Float3Multiply(Float3Normalize(move), climdmoveSpeed_);
    transform_.position_ = Float3Add(transform_.position_, move);

    move = Float3Sub(climbPos_, transform_.position_);
    dist = sqrtf(move.x * move.x + move.y * move.y + move.z * move.z);
    if (dist <= 0.1f) {
        transform_.position_ = climbPos_;
        isClimb_ = false;
    }
}

void Player::CheckWallClimb()
{
    //判定方向取得
    XMFLOAT3 move = XMFLOAT3();
    if (InputManager::CmdWalk(playerId_)) move = GetInputMove();
    else XMStoreFloat3(&move, GetDirectionVec());

    XMFLOAT3 calcCellPos = Float3Add(move, transform_.position_);
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y, transform_.position_.z);
    rayData.dir = move;
    GameManager::GetCollisionMap()->CellWallRayCast(calcCellPos, &rayData);

    //ちょいタス
    const float playerHarf = 0.5f + 0.3f;
    if (rayData.hit && rayData.dist <= playerHarf) {
        rayData = RayCastData();
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);

        const float climbedHeightDist = 1.5f;
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + climbedHeightDist, transform_.position_.z);
        rayData.start = Float3Add(rayData.start, Float3Multiply(move, playerHarf));
        calcCellPos = rayData.start;
        GameManager::GetCollisionMap()->CellFloarRayCast(calcCellPos, &rayData);

        const float climbedDistance = 0.5f;
        if (rayData.hit && rayData.dist <= climbedDistance) {
            XMFLOAT3 nPos = rayData.start;
            nPos.y -= rayData.dist;
            isFly_ = false;
            isClimb_ = true;
            climbPos_ = nPos;
            gravity_= 0.0f;
        }
    }
}