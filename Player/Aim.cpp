#include "Aim.h"
#include "Player.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Image.h"
#include "../Engine/Fbx.h"
#include "../Stage/CollisionMap.h"
#include "../Other/InputManager.h"
#include <vector>

namespace {
    static const float UP_MOUSE_LIMIT = -80.0f;                     //��]���E�l
    static const float DOWN_MOUSE_LIMIT = 80.0f;                    //��]���E�l
    
    static const float COMPULSION_COMPLEMENT_DEFAULT = 0.06f;       //�����̕⊮��f�t�H���g��
    static const int COMPULSION_TIME_DEFAULT = 60;                  //��������߂鎞��
    
    static const float MOUSE_SPEED_DEFAULT = 0.1f;                  //���x
    static const float DISTANCE_HORIZONTAL_DEFAULT = 0.8f;         //�ǂ̂��炢���E�ɂ��炷��
    static const float DISTANCE_BEHIND_DEFAULT = 3.0f;              //�ǂ̂��炢��납��ڂ����̃f�t�H���g�l
    static const float DISTANCE_HEIGHT_DEFAULT = 1.3f;              //Aim�̍���
    static const float HEIGHT_RAY = 0.1f;                           //RayCast�̒l�Ƀv���X���鍂��
}

Aim::Aim(GameObject* parent)
    : GameObject(parent, "Aim"), cameraPosition_{ 0,0,0 }, cameraTarget_{ 0,0,0 }, aimDirection_{ 0,0,0 }, cameraOffset_{ 0,0,0 },
    compulsionTarget_{ 0,0,0 }, compulsionPosisiton_{ 0,0,0 }, pPlayer_(nullptr), isMove_(true), isCompulsion_(false), compulsionTime_(0), 
    iterations_(0), sign_(1), range_(0), moveDistance_(0), distanceDecrease_(0), center_{ 0,0,0,0 }, shakeSpeed_(0), rangeDecrease_(0),
    shakeDirection_{ 1,0,0,0 }, isValid_(true)
{
    distanceHeight_ = DISTANCE_HEIGHT_DEFAULT;
    distanceHorizontal_ = DISTANCE_HORIZONTAL_DEFAULT;
    distanceBehind_ = DISTANCE_BEHIND_DEFAULT;
    distanceTargetBehind_ = DISTANCE_BEHIND_DEFAULT;

    mouseSensitivity_ = MOUSE_SPEED_DEFAULT;
    compulsionComplement_ = COMPULSION_COMPLEMENT_DEFAULT;
}

Aim::~Aim()
{
}

void Aim::Initialize()
{
    pPlayer_ = static_cast<Player*>(FindObject("Player"));
    DefaultAim();

}

void Aim::Update()
{
    //�f�o�b�O�p
    if (Input::IsKeyDown(DIK_5)) isValid_ = !isValid_;
    if (Input::IsKey(DIK_1)) distanceHeight_ += 0.1f;
    if (Input::IsKey(DIK_2)) distanceHeight_ -= 0.1f;
    if (Input::IsKey(DIK_3)) distanceTargetBehind_ += 0.1f;
    if (Input::IsKey(DIK_4)) distanceTargetBehind_ -= 0.1f;
    if (!IsValid()) return;

    if (InputManager::IsCmd(InputManager::AIM)) {
        distanceBehind_ = 0.5f;
        distanceHorizontal_ = 0.35f;
        distanceHeight_ = 1.15f;
        mouseSensitivity_ = (MOUSE_SPEED_DEFAULT * 0.7f);
    }
    else if(InputManager::IsCmdUp(InputManager::AIM)) {
        distanceBehind_ = DISTANCE_BEHIND_DEFAULT;
        distanceHorizontal_ = DISTANCE_HORIZONTAL_DEFAULT;
        distanceHeight_ = DISTANCE_HEIGHT_DEFAULT;
        mouseSensitivity_ = MOUSE_SPEED_DEFAULT;
    }

    if (compulsionTime_ > 0) {
        //�����ړ�
        if (isCompulsion_) {
            Compulsion();
            isCompulsion_ = false;
            return;
        }
    }

    if (isMove_) {
        CalcMouseMove();
    }

    DefaultAim();
}

void Aim::Draw()
{
}

void Aim::Release()
{
}

void Aim::SetCameraShake(const CameraShakeInfo& info)
{
    iterations_ = info.iterat;
    range_ = info.range;
    moveDistance_ = info.range;
    shakeSpeed_ = info.speed;
    distanceDecrease_ = info.speed_decrease;
    rangeDecrease_ = info.range_decrease;
    
    center_ = XMVectorZero();
    sign_ = 1.0f;

}

void Aim::SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar)
{
    compulsionPosisiton_ = pos;
    compulsionTarget_ = tar;
    isCompulsion_ = true;

    compulsionComplement_ = COMPULSION_COMPLEMENT_DEFAULT;
    compulsionTime_ = COMPULSION_TIME_DEFAULT;
}

void Aim::SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar, int returnTime, float complement)
{
    SetCompulsion(pos, tar);

    compulsionTime_ = returnTime;
    compulsionComplement_ = complement;
}

//------------------------------------private--------------------------------------------

void Aim::DefaultAim()
{
    XMFLOAT3 plaPos = pPlayer_->GetPosition();
    CameraShake();

    XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMMATRIX mView = mRotX * mRotY;

    const XMVECTOR forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
    XMVECTOR caDire = XMVector3TransformNormal(forwardVector, mView);
    XMStoreFloat3(&aimDirection_, -caDire);

    const XMVECTOR rightVector = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
    caDire = XMVector3TransformNormal(rightVector, mRotY);
    XMFLOAT3 camDir = XMFLOAT3();
    XMStoreFloat3(&camDir, caDire);
    cameraTarget_.x = plaPos.x - (camDir.x * distanceHorizontal_);
    cameraTarget_.y = plaPos.y + distanceHeight_;
    cameraTarget_.z = plaPos.z - (camDir.z * distanceHorizontal_);

    XMVECTOR caTarget = XMLoadFloat3(&cameraTarget_);
    XMVECTOR forwardV = XMVector3TransformNormal(forwardVector, mView);
    distanceBehind_ = distanceBehind_ + ((distanceTargetBehind_ - distanceBehind_) * 0.1f);
    XMVECTOR camPos = caTarget + (forwardV * distanceBehind_);
    XMStoreFloat3(&cameraPosition_, camPos);
    XMStoreFloat3(&cameraTarget_, caTarget);

    //RayCast���Ă��̒l���㏑������
    RayCastStage();

    //�J���������Z�b�g
    Camera::SetPosition(cameraPosition_);
    Camera::SetTarget(cameraTarget_);
}

void Aim::Compulsion()
{
    XMStoreFloat3(&cameraPosition_, (XMVectorLerp(XMLoadFloat3(&cameraPosition_), XMLoadFloat3(&compulsionPosisiton_), compulsionComplement_)));
    XMStoreFloat3(&cameraTarget_, (XMVectorLerp(XMLoadFloat3(&cameraTarget_), XMLoadFloat3(&compulsionTarget_), compulsionComplement_)));

    XMVECTOR dir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    distanceBehind_ = XMVectorGetX(XMVector3Length(dir));

    RayCastStage();
    Camera::SetPosition(cameraPosition_);
    Camera::SetTarget(cameraTarget_);

    //�����ړ�����Rotate�����߂�
    dir = XMVector3Normalize(dir);
    float rotationY = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));
    float rotationX = -asinf(XMVectorGetY(dir));
    transform_.rotate_.x = compulsionPosisiton_.x = XMConvertToDegrees(rotationX);
    transform_.rotate_.y = compulsionPosisiton_.y = XMConvertToDegrees(rotationY);

    //Rotate�̌��ʂ��g����AimDirection���v�Z���ăZ�b�g
    XMVECTOR direction = CalculationVectorDirection(transform_.rotate_);
    XMStoreFloat3(&aimDirection_, -direction);
}

void Aim::BackCompulsion()
{
    CalcMouseMove();

    //�������̎��_�̋��������߂�
    XMVECTOR dir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    float dist = XMVectorGetX(XMVector3Length(dir));
    distanceBehind_ = distanceBehind_ - ((dist - distanceTargetBehind_) / (float)compulsionTime_);

    //�߂蒆��Position��Target���v�Z����
    XMFLOAT3 position = pPlayer_->GetPosition();
    XMFLOAT3 target = { position.x + cameraOffset_.x, position.y + distanceHeight_, position.z + cameraOffset_.z };
    XMVECTOR camPos = XMLoadFloat3(&target) + (CalculationVectorDirection(transform_.rotate_)  * distanceBehind_);
    XMStoreFloat3(&position, camPos);

    XMVECTOR vPos = XMLoadFloat3(&cameraPosition_);
    vPos = vPos - ((vPos - XMLoadFloat3(&position)) / (float)compulsionTime_);
    XMStoreFloat3(&cameraPosition_, vPos);

    XMVECTOR vTar = XMLoadFloat3(&cameraTarget_);
    vTar = vTar - ((vTar - XMLoadFloat3(&target)) / (float)compulsionTime_);
    XMStoreFloat3(&cameraTarget_, vTar);

    //�v�Z���ʂ��g����RayCast���邻����Z�b�g
    RayCastStage();
    Camera::SetPosition(cameraPosition_);
    Camera::SetTarget(cameraTarget_);

    //�����ړ�����Rotate�EDirection�����߂�
    dir = XMVector3Normalize(dir);
    float rotationY = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));
    float rotationX = -asinf(XMVectorGetY(dir));
    transform_.rotate_.x = XMConvertToDegrees(rotationX);
    transform_.rotate_.y = XMConvertToDegrees(rotationY);
    aimDirection_ = CalculationDirection(transform_.rotate_);
    aimDirection_ = Float3Multiply(aimDirection_, -1.0f);
}

void Aim::RayCastStage()
{
    RayCastData data;
    XMFLOAT3 start = cameraTarget_;
    XMVECTOR vDir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    vDir = XMVector3Normalize(vDir);
    XMFLOAT3 dir = XMFLOAT3();
    XMStoreFloat3(&dir, vDir);
    data.start = start;
    data.dir = dir;
    CollisionMap* cMap = static_cast<CollisionMap*>(FindObject("CollisionMap"));
    cMap->RaySelectCellVsSegment(cameraPosition_, &data);

    //���C���������E���苗������������
    if (data.dist <= distanceBehind_ + HEIGHT_RAY) {
        distanceBehind_ = data.dist - HEIGHT_RAY;
    }

    XMVECTOR camPos = XMLoadFloat3(&cameraTarget_) + (vDir * distanceBehind_);
    XMStoreFloat3(&cameraPosition_, camPos);
}

void Aim::CalcMouseMove()
{
    XMFLOAT3 mouseMove = Input::GetMouseMove(); //�}�E�X�̈ړ��ʂ��擾
    transform_.rotate_.y += mouseMove.x * mouseSensitivity_; //�������̉�]
    transform_.rotate_.x -= mouseMove.y * mouseSensitivity_; //�c�����̉�]
    if (transform_.rotate_.x <= UP_MOUSE_LIMIT) transform_.rotate_.x = UP_MOUSE_LIMIT;
    if (transform_.rotate_.x >= DOWN_MOUSE_LIMIT) transform_.rotate_.x = DOWN_MOUSE_LIMIT;

}

void Aim::CameraShake()
{
    if (iterations_ <= 0) return;

    //�J�����V�F�C�N�I���߂鏈��
    if (iterations_ == 1) {
        float dist = XMVectorGetX(XMVector3Length(center_));
        XMVECTOR vec = -XMVector3Normalize(center_);

        //�ڕW�n�_�ɂ���
        if (dist < moveDistance_ * shakeSpeed_) {
            moveDistance_ = dist;
            iterations_--;
            center_ = XMVectorZero();
            sign_ = 1.0f;
        }

        vec *= moveDistance_ * shakeSpeed_;
        center_ += vec;

        cameraTarget_.x += center_.m128_f32[0];
        cameraTarget_.y += center_.m128_f32[1];
        cameraTarget_.z += center_.m128_f32[2];
        return;
    }

    //�ړ������E�ړ��ڕW�ꏊ���v�Z
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMVECTOR shakeDirection = XMVector3Normalize(XMVector3TransformNormal((shakeDirection_), mRotY));
    XMVECTOR nextPosition = center_ + shakeDirection;

    //�������ׂĈړ�������
    XMVECTOR directionVector = nextPosition - center_;
    directionVector = XMVector3NormalizeEst(directionVector) * moveDistance_ * shakeSpeed_;
    center_ += directionVector;

    //�������������ׂ�
    bool sign2 = (center_.m128_f32[0] + center_.m128_f32[1] + center_.m128_f32[2] / 3) > 0.0f;
    bool sign3 = (directionVector.m128_f32[0] + directionVector.m128_f32[1] + directionVector.m128_f32[2] / 3) > 0;

    //�ڕW�n�_�𒴂��������ׂ�
    float cen = XMVectorGetX(XMVector3Length(center_));
    if (sign2 == sign3 && cen > range_) {
        iterations_--;
        sign_ *= -1.0f;
        shakeDirection_ *= -1.0f;
        center_ = XMVector3Normalize(center_) * range_;
       
        moveDistance_ *= distanceDecrease_;
        range_ *= rangeDecrease_;
    }

    cameraTarget_.x += center_.m128_f32[0];
    cameraTarget_.y += center_.m128_f32[1];
    cameraTarget_.z += center_.m128_f32[2];
}
