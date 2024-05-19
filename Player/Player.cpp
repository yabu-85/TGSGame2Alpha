#include "Player.h"
#include "Aim.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Stage/Stage.h"

#include "../Other/InputManager.h"
#include "../Engine/Text.h"
#include "../Engine/Input.h"

#include "../Engine/Direct3D.h"

namespace {
    const float moveSpeed = 0.08f;          //�ړ��X�s�[�h
    const float stopGradually = 0.21f;      //�ړ��X�s�[�h�̉����̒l�~�܂�Ƃ�
    const float moveGradually = 0.09f;      //�ړ��X�s�[�h�̉����̒l�ړ���
    const float maxMoveSpeed = 0.7f;        //�ő�ړ��X�s�[�h
    const float rotateRatio = 0.1f;         //�ʏ펞��RotateRatio

    const float JumpPower = 0.09f;
    const float gravity = 0.0015f;

    const float PlayerHeightSize = 1.3f;    //��ԏ�
    const float PlayerHeadSize = 0.3f;      //��̂Ƃ�
    const float PlayerBesideSize = 0.3f;    //������T�C�Y
    const float CheckDistance[2] = { 0.3f, PlayerHeightSize };  //�����A��

    //�f�o�b�O�p
    bool isCollider = true; //�����蔻�肷�邩�ǂ���
    Text* pText = nullptr;
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

    transform_.position_.y = 10.0f;
    Model::SetAnimFrame(hModel_, 0, 100, 1.0f);
    pAim_ = Instantiate<Aim>(this);

    pText = new Text();
    pText->Initialize();

}

void Player::Update()
{
    if (InputManager::CmdWalk()) {
        CalcMove();
        Rotate();
    }
    else {
        CalcNoMove();
    }
    Move();
    
    //�d��
    static float gra = 0.0f;
    static bool isGround = false;
    if (isGround && InputManager::IsCmdDown(InputManager::JUMP)) gra = -JumpPower;
    gra += gravity;
    transform_.position_.y -= gra;

    //�X�e�[�W�����蔻��
    Stage* pStage = (Stage*)FindObject("Stage");
    isGround = false;
    //��
    if(gra < 0.0f) {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, 1.0f, 0.0f);
        Model::RayCastSurface(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y -= PlayerHeightSize - rayData.dist;
            gra = 0.0f;
            OutputDebugString("��\n");
        }
    }
    //��
    else {
        RayCastData rayData = RayCastData();
        rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + PlayerHeightSize, transform_.position_.z);
        rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
        Model::RayCast(pStage->GetModelHandle(), &rayData);
        if (rayData.hit && rayData.dist < PlayerHeightSize) {
            transform_.position_.y += PlayerHeightSize - rayData.dist;
            gra = 0.0f;
            OutputDebugString("��\n");
            isGround = true;
        }
    }
    //�E
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.x += rayData.dist - PlayerBesideSize;
                OutputDebugString("�E\n");
            }
        }
    }
    //��
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(-1.0f, 0.0f, 0.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.x -= rayData.dist - PlayerBesideSize;
                OutputDebugString("��\n");
            }
        }
    }
    //�O
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, 1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.z += rayData.dist - PlayerBesideSize;
                OutputDebugString("�O\n");
            }
        }
    }
    //���
    {
        for (int i = 0; i < 2; i++) {
            RayCastData rayData = RayCastData();
            rayData.start = XMFLOAT3(transform_.position_.x, transform_.position_.y + CheckDistance[i], transform_.position_.z);
            rayData.dir = XMFLOAT3(0.0f, 0.0f, -1.0f);
            Model::RayCast(pStage->GetModelHandle(), &rayData);
            if (rayData.hit && rayData.dist < PlayerBesideSize) {
                transform_.position_.z -= rayData.dist - PlayerBesideSize;
                OutputDebugString("���\n");
            }
        }
    }
    


    //�f�o�b�O�p
    if (Input::IsKeyDown(DIK_Z)) transform_.position_ = XMFLOAT3(0.0f, 10.0f, 0.0f);
    if (Input::IsKeyDown(DIK_T)) isCollider = !isCollider;

}

void Player::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    /*
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

    //�f�o�b�O�p
    CollisionDraw();

    pText->Draw(30, 30, (int)transform_.position_.x);
    pText->Draw(30, 70, (int)transform_.position_.y);
    pText->Draw(30, 110, (int)transform_.position_.z);
    */
    

}

void Player::Release()
{
}

//private�֐��FRotate�̌v�Z����--------------------------------
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
            fMove.z += aimDirection.z;
        }
        if (InputManager::IsCmd(InputManager::MOVE_LEFT)) {
            fMove.x -= aimDirection.z;
            fMove.z += aimDirection.x;
        }
        if (InputManager::IsCmd(InputManager::MOVE_DOWN)) {
            fMove.x -= aimDirection.x;
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

void Player::FrontMove(float f)
{
    XMVECTOR vMove = { 0.0, 0.0, 1.0, 0.0 };
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    vMove = XMVector3TransformCoord(vMove, mRotY);
    vMove = XMVector3Normalize(vMove);
    vMove = XMVector3Normalize(vMove + XMLoadFloat3(&playerMovement_));
    XMFLOAT3 move{};
    XMStoreFloat3(&move, vMove);

    transform_.position_.x += ((move.x * moveSpeed) * f);
    transform_.position_.z += ((move.z * moveSpeed) * f);
}

void Player::BackMove(float f)
{
    XMVECTOR vMove = { 0.0, 0.0, -1.0, 0.0 };
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    vMove = XMVector3TransformCoord(vMove, mRotY);
    vMove = XMVector3Normalize(vMove);
    vMove = XMVector3Normalize(vMove + XMLoadFloat3(&playerMovement_));
    XMFLOAT3 move{};
    XMStoreFloat3(&move, vMove);

    transform_.position_.x += ((move.x * moveSpeed) * f);
    transform_.position_.z += ((move.z * moveSpeed) * f);
}

void Player::Move(float f)
{
    transform_.position_.x += ((playerMovement_.x * moveSpeed) * f);
    transform_.position_.z += ((playerMovement_.z * moveSpeed) * f);
}

XMVECTOR Player::GetDirectionVec()
{
    XMVECTOR vMove = { 0.0, 0.0, 1.0, 0.0 };
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    vMove = XMVector3TransformCoord(vMove, mRotY);
    vMove = XMVector3Normalize(vMove);
    return vMove;
}

void Player::ResetMovement()
{
    playerMovement_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void Player::CalcMove()
{
    gradually_ = stopGradually;

    XMFLOAT3 fMove = GetInputMove();
    XMFLOAT3 move = { ((fMove.x - playerMovement_.x) * gradually_) , 0.0f , ((fMove.z - playerMovement_.z) * gradually_) };
    playerMovement_ = { playerMovement_.x + move.x , 0.0f , playerMovement_.z + move.z };

    //MaxSpeed�����Ă����琳�K���EMaxSpeed�̒l�ɂ���
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
    move = { ((move.x - playerMovement_.x) * gradually_) , 0.0f , ((move.z - playerMovement_.z) * gradually_) };
    playerMovement_ = { playerMovement_.x + move.x , 0.0f , playerMovement_.z + move.z };
}