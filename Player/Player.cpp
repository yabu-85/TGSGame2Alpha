#include "Player.h"
#include "Aim.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/CapsuleCollider.h"
#include "../State/PlayerState.h"
#include "../State/StateManager.h"
#include "../Other/InputManager.h"
#include "../Stage/CollisionMap.h"
#include "../Stage/StageEditor.h"
#include "../Other/GameManager.h"
#include "../UI/HealthGauge.h"
#include "../Character/DamageSystem.h"
#include "../Weapon/Gun.h"
#include "../Weapon/SniperGun.h"

namespace {
    const float stopGradually = 0.21f;      //移動スピードの加減の値止まるとき
    const float moveGradually = 0.09f;      //移動スピードの加減の値移動時
    const float maxmoveSpeed_ = 0.7f;        //最大移動スピード
    const float rotateRatio = 0.1f;         //通常時のRotateRatio

    const float JumpPower = 0.12f;
    const float WorldGravity = 0.005f;
    const float PlayerHeight = 1.3f;
    const float PlayerWaist = 0.4f;

    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 10.0f, 50.0f);

    int orientBoneSize = 0;
    int downOrientBoneSize = 0;

}

Player::Player(GameObject* parent)
    : Character(parent, "Player"), hModel_(-1), pAim_(nullptr), pGunBase_(nullptr), pStateManager_(nullptr), pCapsuleCollider_(nullptr),
    playerMovement_(0, 0, 0), gradually_(0.0f), climbPos_(XMFLOAT3()), isFly_(true), isClimb_(false), isCreative_(false), gravity_(0.0f), moveSpeed_(0.0f),
    playerId_(0), waistPart_(-1), waistRotateY_(0.0f), hFPSModel_(-1)
{
    for (int i = 0; i < 8; i++) waistListIndex_[i] = -1;

}

Player::~Player()
{
}

void Player::Initialize()
{
    hModel_ = Model::Load("Model/desiFiter.fbx");
    assert(hModel_ >= 0);

    hFPSModel_ = Model::Load("Model/gunFiterFPS.fbx");
    assert(hFPSModel_ >= 0);

    //Orient登録
#if 1
    waistPart_ = Model::GetPartIndex(hModel_, "thigh.L");
    std::pair<std::string, std::string> boneNameList[] = {
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
        { "eye.001",    "spine.004" },  //15
    };
    std::pair<std::string, std::string> boneDownNameList[] = {
        { "thigh.R",    ""},
        { "shin.R",     "thigh.R" },
        { "foot.R",     "thigh.R" },
        { "toe.R",      "thigh.R" },

        { "thigh.L",    ""},
        { "shin.L",     "thigh.L" },
        { "foot.L",     "thigh.L" },
        { "toe.L",      "thigh.L" },
    };

    orientBoneSize = (int)sizeof(boneNameList) / sizeof(boneNameList[0]);
    downOrientBoneSize = (int)sizeof(boneDownNameList) / sizeof(boneDownNameList[0]);

    for (int i = 0; i < orientBoneSize;i++) {
        waistListIndex_[i] = Model::AddOrientRotateBone(hModel_, boneNameList[i].first, boneNameList[i].second);
        Model::AddOrientRotateBone(hFPSModel_, boneNameList[i].first, boneNameList[i].second);
    }
    for (int i = 0; i < downOrientBoneSize; i++) {
        downListIndex_[i] = Model::AddOrientRotateBone(hModel_, boneDownNameList[i].first, boneDownNameList[i].second);
    }
#endif

    //PlayerIDセット
    if (GameManager::GetPlayer(0)) playerId_ = 1;
    GameManager::SetPlayer(this, playerId_);

    Model::SetTransform(hModel_, transform_);
    Model::SetTransform(hFPSModel_, transform_);

    //パラメータセット
    objectType_ = OBJECT_TYPE::Player;
    SetBodyRange(0.2f);
    SetBodyWeight(1.0f);
    SetBodyHeightHalf(1.0f);
    
    SetMaxHP(100);
    SetHP(100);
    
    pHealthGauge_ = new HealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.0f, 1.7f));
    moveSpeed_ = 0.07f;

    pAim_ = Instantiate<Aim>(this);
    pGunBase_ = Instantiate<SniperGun>(this);
    
    pStateManager_ = new StateManager(this);
    pStateManager_->AddState(new PlayerWait(pStateManager_));
    pStateManager_->AddState(new PlayerMove(pStateManager_));
    pStateManager_->AddState(new PlayerJump(pStateManager_));
    pStateManager_->AddState(new PlayerClimb(pStateManager_));
    pStateManager_->ChangeState("Wait");

    XMVECTOR vec = { 0.0f, 1.0f, 0.0f, 0.0f };
    pCapsuleCollider_ = new CapsuleCollider(XMFLOAT3(0.0f, 0.65f, 0.0f), 0.3f, 0.65f, vec);
    pCapsuleCollider_->typeList_.push_back(OBJECT_TYPE::Stage);
    AddCollider(pCapsuleCollider_);

    Direct3D::playerSpeed = moveSpeed_;
}

void Player::Update()
{
    //デバッグ用
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

        moveSpeed_ = Direct3D::playerSpeed;
        Direct3D::PlayerPosition = transform_.position_;
        Direct3D::playerClimb = isClimb_;
        Direct3D::playerFaly = isFly_;
        return;
    }

    //Orientテスト
#if 1
    //上下視点
    waistRotateX_ = -pAim_->GetRotate().x;
    for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, 0.0f, 0.0f));
    for (int i = 0; i < orientBoneSize; i++) Model::SetOrietnRotateBone(hFPSModel_, waistListIndex_[i], XMFLOAT3(waistRotateX_, 0.0f, 0.0f));
    //腰下
    for (int i = 0; i < downOrientBoneSize; i++) Model::SetOrietnRotateBone(hModel_, downListIndex_[i], XMFLOAT3(0.0f, waistRotateY_, 0.0f));
#endif

    //ステート
    pStateManager_->Update();

    //空中にいる
    if (isFly_) {

        //登り処理
        if (InputManager::IsCmd(InputManager::JUMP, playerId_) && !IsClimb()) {
            CheckWallClimb();
            if(IsClimb()) pStateManager_->ChangeState("Climb");
        }

        //重力処理
        gravity_ += WorldGravity;
        transform_.position_.y -= gravity_;
        
        StageRoofBounce();
        StageFloarBounce(0.0f, -1.0f);
        StageFloarBounce();
        StageWallBounce();
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
    moveSpeed_ = Direct3D::playerSpeed;
    Direct3D::PlayerPosition = transform_.position_;
    Direct3D::playerClimb = isClimb_;
    Direct3D::playerFaly = isFly_;

    //Weapon用にここでSet
    Model::SetTransform(hModel_, transform_);
    Model::SetTransform(hFPSModel_, transform_);
}

void Player::Draw()
{
    //Shadowシェーダーの時
    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_SHADOWMAP) {
        return;
    }

    //自分自身の表示（アニメーション進めるために映らない場所でDraw
    if (GameManager::GetDrawIndex() == playerId_) {
        Model::SetTransform(hFPSModel_, transform_);
        Model::Draw(hFPSModel_);
    }
    //相手の表示
    else {
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);

        //HealthGauge表示
        if (healthGaugeDrawTime_ > 0) {
            healthGaugeDrawTime_--;
            float r = (float)GetHP() / (float)GetMaxHP();
            pHealthGauge_->SetParcent(r);
            pHealthGauge_->Draw(GameManager::GetDrawIndex());
        }
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

#if PC_CTRL
        if (playerId_ == 0) {
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
#else
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
#endif
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
    //修正箇所

    XMVECTOR push = XMVectorZero(); 
    SphereCollider* collid = new SphereCollider(XMFLOAT3(), pCapsuleCollider_->size_.x);
    collid->pGameObject_ = this;
    collid->center_ = XMFLOAT3(pCapsuleCollider_->center_.x, pCapsuleCollider_->center_.y - (pCapsuleCollider_->height_ * 0.5f), pCapsuleCollider_->center_.z);
    bool hit = GameManager::GetCollisionMap()->CellSphereVsTriangle(collid, push);

    //上
    collid->center_ = XMFLOAT3(pCapsuleCollider_->center_.x, pCapsuleCollider_->center_.y + (pCapsuleCollider_->height_ * 0.5f), pCapsuleCollider_->center_.z);
    push = XMVectorZero();
    if (GameManager::GetCollisionMap()->CellSphereVsTriangle(collid, push)) hit = true;

    delete collid;   
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