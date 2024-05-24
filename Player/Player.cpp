#include "Player.h"
#include "Aim.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/CapsuleCollider.h"
#include "../Stage/Stage.h"

#include "../Other/InputManager.h"
#include "../Engine/Text.h"
#include "../Engine/Input.h"
#include "../Engine/Direct3D.h"

namespace {
    float moveSpeed = 0.08f;          //移動スピード
    const float stopGradually = 0.21f;      //移動スピードの加減の値止まるとき
    const float moveGradually = 0.09f;      //移動スピードの加減の値移動時
    const float maxMoveSpeed = 0.7f;        //最大移動スピード
    const float rotateRatio = 0.1f;         //通常時のRotateRatio

    const float JumpPower = 0.09f;
    const float gravity = 0.0015f;

    const float PlayerHeightSize = 1.3f;    //一番上
    const float PlayerHeadSize = 0.3f;      //首のとこ
    const float PlayerBesideSize = 0.3f;    //横判定サイズ
    const float CheckDistance[2] = { 0.3f, PlayerHeightSize };  //足元、頭

    //デバッグ用
    float gra = 0.0f;
    bool isGround = false;
    bool isFly = false;
    bool isCollider = true; //当たり判定するかどうか
    Text* pText = nullptr;

    CapsuleCollider* pCollid = nullptr;
}

Player::Player(GameObject* parent)
    : GameObject(parent, "Player"), hModel_(-1), pAim_(nullptr), playerMovement_(0, 0, 0), gradually_(0.0f), testModel_(-1)
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

    moveSpeed = 0.05f;
    Direct3D::playerSpeed = moveSpeed;

    transform_.position_.y = 10.0f;
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
    pAim_ = Instantiate<Aim>(this);

#if 1
    XMVECTOR vec = { 1.0f, 0.0f, 0.0f, 0.0f };
    pCollid = new CapsuleCollider(XMFLOAT3(), 1.0f, 3.0f, vec);
    AddCollider(pCollid);
#else
    AddCollider(new SphereCollider(XMFLOAT3(), 1.0f));
#endif

}

void Player::Update()
{
    if (Input::IsKeyDown(DIK_T)) isFly = !isFly;
    moveSpeed = Direct3D::playerSpeed;

    if (InputManager::CmdWalk()) {
        CalcMove();
        Rotate();
    }
    else {
        CalcNoMove();
    }
    if (Input::IsKey(DIK_SPACE)) playerMovement_.y += 0.1f;
    else if (Input::IsKey(DIK_F)) playerMovement_.y -= 0.1f;
    Move();
    
    //重力
    if (!isFly) {
        if (isGround && InputManager::IsCmdDown(InputManager::JUMP)) gra = -JumpPower;
        gra += gravity;
        if(gra > 0.0f) transform_.position_.y -= gra;
    }

    if (transform_.position_.y < 0.0f) {
        gra = 0;
        transform_.position_.y = 0.0f;
    }

    /*
    Stage* pStage = (Stage*)FindObject("Stage");
    isGround = false;
    //上
    if(gra < 0.0f) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
        Model::RayCastSurface(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y = transform_.position_.y - PlayerHeightSize - rayData.dist;
            gra = 0.0f;
        }
    }
    //下
    else {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        Model::RayCast(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y += PlayerHeightSize - rayData.dist;
            gra = 0.0f;
            isGround = true;
        }
    }
    //右
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.x += rayData.dist - PlayerBesideSize;
            }
        }
    }
    //左
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(-1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.x -= rayData.dist - PlayerBesideSize;
            }
        }
    }
    //前
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.z += rayData.dist - PlayerBesideSize;
            }
        }
    }
    //後ろ
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, -1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.z -= rayData.dist - PlayerBesideSize;
                OutputDebugString("後ろ\n");
            }
        }
    }
    
    */
    //ステージ当たり判定
    


    //デバッグ用
    if (Input::IsKeyDown(DIK_Z)) transform_.position_ = XMFLOAT3(0.0f, 10.0f, 0.0f);
    if (Input::IsKeyDown(DIK_T)) isCollider = !isCollider;

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

void Player::Draw()
{
    Direct3D::PlayerPosition = transform_.position_;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    /*当たり判定のやつ
    Transform tra = transform_;
    tra.position_.y += PlayerHeightSize;
    tra.scale_ = XMFLOAT3(0.1f, 0.1f, 0.1f);
    Model::SetTransform(testModel_, tra);
    Model::Draw(testModel_);

    tra.position_ = transform_.position_;
    tra.position_.y += PlayerHeightSize - PlayerHeadSize;
    Model::SetTransform(testModel_, tra);
    Model::Draw(testModel_);

    tra.position_ = transform_.position_;
    tra.position_.y += CheckDistance[0];
    Model::SetTransform(testModel_, tra);
    Model::Draw(testModel_);
    */
    
    CollisionDraw();

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
            if(isFly) fMove.y += aimDirection.y;
            fMove.z += aimDirection.z;
        }
        if (InputManager::IsCmd(InputManager::MOVE_LEFT)) {
            fMove.x -= aimDirection.z;
            fMove.z += aimDirection.x;
        }
        if (InputManager::IsCmd(InputManager::MOVE_DOWN)) {
            fMove.x -= aimDirection.x;
            if (isFly) fMove.y -= aimDirection.y;
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
    transform_.position_.x += ((playerMovement_.x * moveSpeed) * f);
    transform_.position_.y += ((playerMovement_.y * moveSpeed) * f);
    transform_.position_.z += ((playerMovement_.z * moveSpeed) * f);
}

void Player::CalcMove()
{
    gradually_ = stopGradually;

    XMFLOAT3 fMove = GetInputMove();
    XMFLOAT3 move = { ((fMove.x - playerMovement_.x) * gradually_) , ((fMove.y - playerMovement_.y) * gradually_) , ((fMove.z - playerMovement_.z) * gradually_) };
    playerMovement_ = { playerMovement_.x + move.x , playerMovement_.y + move.y , playerMovement_.z + move.z };

    //MaxSpeed超えていたら正規化・MaxSpeedの値にする
    float currentSpeed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&playerMovement_)));
    if (currentSpeed > maxMoveSpeed) {
        XMVECTOR vMove;
        vMove = XMLoadFloat3(&playerMovement_);
        vMove = XMVector3Normalize(vMove);
        vMove *= maxMoveSpeed;
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