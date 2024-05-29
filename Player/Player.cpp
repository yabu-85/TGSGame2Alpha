#include "Player.h"
#include "Aim.h"
#include "../State/StateBase.h"
#include "../State/PlayerState.h"
#include "../State/StateManager.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"

#include "../Other/InputManager.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"
#include "../Engine/StageEditor.h"

#include "../Stage/CollisionMap.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/CapsuleCollider.h"

namespace {
    const float stopGradually = 0.21f;      //移動スピードの加減の値止まるとき
    const float moveGradually = 0.09f;      //移動スピードの加減の値移動時
    const float maxmoveSpeed_ = 0.7f;        //最大移動スピード
    const float rotateRatio = 0.1f;         //通常時のRotateRatio

    const float JumpPower = 0.12f;
    const float WorldGravity = 0.005f;
    const float PlayerHeightSize = 1.3f;

    const XMFLOAT3 start = XMFLOAT3(50.0f, 10.0f, 50.0f);
    CollisionMap* pCMap = nullptr;

}

Player::Player(GameObject* parent)
    : GameObject(parent, "Player"), hModel_(-1), pAim_(nullptr), playerMovement_(0, 0, 0), gradually_(0.0f), testModel_(-1), climbPos_(XMFLOAT3()),
    isFly_(true), isClimb_(false), isCreative_(false), gravity_(0.0f), moveSpeed_(0.0f), pStateManager_(nullptr)
{
}

Player::~Player()
{
}

void Player::Initialize()
{
    hModel_ = Model::Load("Model/desiFiter.fbx");
    assert(hModel_ >= 0);
    testModel_ = Model::Load("DebugCollision/SphereCollider.fbx");
    assert(testModel_ >= 0);

    moveSpeed_ = 0.15f;
    Direct3D::playerSpeed = moveSpeed_;

    transform_.position_ = start;
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
    pAim_ = Instantiate<Aim>(this);

    pStateManager_ = new StateManager(this);
    pStateManager_->AddState(new PlayerWait(pStateManager_));
    pStateManager_->AddState(new PlayerMove(pStateManager_));
    pStateManager_->AddState(new PlayerJump(pStateManager_));
    pStateManager_->AddState(new PlayerClimb(pStateManager_));
    pStateManager_->ChangeState("Wait");

    StageEditor::SetPlayer(this);
    pCMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    assert(pCMap);

#if 0
    XMVECTOR vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    CapsuleCollider* pCollid = new CapsuleCollider(XMFLOAT3(), 1.0f, 3.0f, vec);
    AddCollider(pCollid);
#else
    AddCollider(new SphereCollider(XMFLOAT3(0.0f, 0.5f, 0.0f), 0.5f));
#endif

}

void Player::Update()
{
    if (Input::IsKeyDown(DIK_Z)) transform_.position_ = start;
    if (Input::IsKeyDown(DIK_M)) isCreative_ = !isCreative_;
    if (isCreative_) {
        if (Input::IsKey(DIK_SPACE)) playerMovement_.y += moveSpeed_ * 0.5f;
        else if (Input::IsKey(DIK_F)) playerMovement_.y -= moveSpeed_ * 0.5f;
        if (InputManager::CmdWalk()) CalcMove();
        else CalcNoMove();
        Move();
        Rotate();

        moveSpeed_ = Direct3D::playerSpeed;
        Direct3D::PlayerPosition = transform_.position_;
        Direct3D::playerClimb = isClimb_;
        Direct3D::playerFaly = isFly_;
        return;
    }

    //空中にいる
    if (isFly_) {
        //登り処理
        if (InputManager::IsCmd(InputManager::JUMP) && !IsClimb()) {
            CheckWallClimb();
            if(IsClimb()) pStateManager_->ChangeState("Climb");
        }

        //重力処理
        if (!IsClimb()) {
            gravity_ += WorldGravity;
            transform_.position_.y -= gravity_;
        }

        StageFloarBounce();
    }
    else {
        float addPos = -1.0f;
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeightSize, transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
            XMFLOAT3 pos = transform_.position_;
            pos.y += addPos;
            pCMap->CellFloarRayCast(pos, &rayData);

            //この値より地面と離れると浮いていると判定される
            const float WalkFalDist = 0.2f;
            if (rayData.hit && rayData.dist < WalkFalDist + PlayerHeightSize) {
                transform_.position_.y += PlayerHeightSize - rayData.dist;
            }
            else {
                isFly_ = true;
            }
            addPos = 0.0f;
        }
    }

    pStateManager_->Update();
    StageWallBounce();

    moveSpeed_ = Direct3D::playerSpeed;
    Direct3D::PlayerPosition = transform_.position_;
    Direct3D::playerClimb = isClimb_;
    Direct3D::playerFaly = isFly_;

    //カプセル
#if 0
    if (!GetAllColliderList().empty()) {
        CapsuleCollider* pCollid = static_cast<CapsuleCollider*>(GetAllColliderList().front());
        if (pCollid) return;
       
        if (Input::IsKey(DIK_NUMPAD7)) {
            XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
        if (Input::IsKey(DIK_NUMPAD8)) {
            XMMATRIX rotationMatrix = XMMatrixRotationX(XMConvertToRadians(-1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
        if (Input::IsKey(DIK_NUMPAD4)) {
            XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
        if (Input::IsKey(DIK_NUMPAD5)) {
            XMMATRIX rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
        if (Input::IsKey(DIK_NUMPAD1)) {
            XMMATRIX rotationMatrix = XMMatrixRotationZ(XMConvertToRadians(1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
        if (Input::IsKey(DIK_NUMPAD2)) {
            XMMATRIX rotationMatrix = XMMatrixRotationZ(XMConvertToRadians(-1.0f));
            XMVECTOR rotatedDirection = XMVector3Transform(pCollid->direction_, rotationMatrix);
            pCollid->direction_ = rotatedDirection;
        }
    }
#endif

}

void Player::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    //CollisionDraw();
}

void Player::Release()
{
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

void Player::TargetRotate(XMFLOAT3 pos, float ratio) { CalcRotate(XMFLOAT3(pos.x - transform_.position_.x, 0.0f, pos.z - transform_.position_.z), ratio); }
void Player::Rotate() { Rotate(rotateRatio); }
void Player::Rotate(float ratio) { CalcRotate(GetInputMove(), ratio); }

//-----------------------------------------------Rotate

XMFLOAT3 Player::GetInputMove()
{
    XMFLOAT3 fMove = { 0,0,0 };
    if (InputManager::CmdWalk()) {
        gradually_ = moveGradually;

        XMFLOAT3 aimDirection = pAim_->GetAimDirection();
        if (InputManager::IsCmd(InputManager::MOVE_UP)) {
            fMove.x += aimDirection.x;
            if (isCreative_) fMove.y += aimDirection.y;
            fMove.z += aimDirection.z;
        }
        if (InputManager::IsCmd(InputManager::MOVE_LEFT)) {
            fMove.x -= aimDirection.z;
            fMove.z += aimDirection.x;
        }
        if (InputManager::IsCmd(InputManager::MOVE_DOWN)) {
            fMove.x -= aimDirection.x;
            if (isCreative_) fMove.y -= aimDirection.y;
            fMove.z -= aimDirection.z;
        }
        if (InputManager::IsCmd(InputManager::MOVE_RIGHT)) {
            fMove.x += aimDirection.z;
            fMove.z -= aimDirection.x;
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

void Player::StageFloarBounce()
{
    float addPos = -1.0f;
    for (int i = 0; i < 2; i++) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        XMFLOAT3 pos = transform_.position_;
        pos.y += addPos;
        pCMap->CellFloarRayCast(pos, &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y += PlayerHeightSize - rayData.dist;
            gravity_= 0.0f;
            isFly_ = false;
            return;
        }
        addPos = 0.0f;
    }
}

void Player::StageWallBounce()
{
    SphereCollider* collid = static_cast<SphereCollider*>(colliderList_.front());
    XMVECTOR push = XMVectorZero();
    pCMap->CellSphereVsTriangle(collid, push);
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
    if (InputManager::CmdWalk()) move = GetInputMove();
    else XMStoreFloat3(&move, GetDirectionVec());

    XMFLOAT3 calcCellPos = Float3Add(move, transform_.position_);
    RayCastData rayData = RayCastData();
    rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y, transform_.position_.z);
    rayData.dir = move;
    pCMap->CellWallRayCast(calcCellPos, &rayData);

    //ちょいタス
    const float playerHarf = 0.5f + 0.3f;
    if (rayData.hit && rayData.dist <= playerHarf) {
        rayData = RayCastData();
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);

        const float climbedHeightDist = 1.5f;
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + climbedHeightDist, transform_.position_.z);
        rayData.start = Float3Add(rayData.start, Float3Multiply(move, playerHarf));
        calcCellPos = rayData.start;
        pCMap->CellFloarRayCast(calcCellPos, &rayData);

        const float climbedDistance = 0.5f;
        if (rayData.hit && rayData.dist <= climbedDistance) {
            XMFLOAT3 nPos = rayData.start;
            nPos.y -= rayData.dist;
            isClimb_ = true;
            climbPos_ = nPos;
            gravity_= 0.0f;
        }
    }
}
