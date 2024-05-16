#include "Aim.h"
#include "Player.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Image.h"
#include "../Other/InputManager.h"
#include <vector>
#include "../Engine/Fbx.h"

namespace {
    static const float UP_MOUSE_LIMIT = -60.0f;
    static const float DOWN_MOUSE_LIMIT = 60.0f;
    
    static const float COMPULSION_COMPLEMENT_DEFAULT = 0.06f;       //�����̕⊮��f�t�H���g��
    static const int COMPULSION_TIME_DEFAULT = 60;                  //��������߂鎞��
    
    static const float MOUSE_SPEED = 0.05f;                         //���x
    static const float DISTANCE_BEHIND_DEFAULT = 5.0f;              //�ǂ̂��炢��납��ڂ����̃f�t�H���g�l
    static const float HEIGHT_RAY = 0.1f;                           //RayCast�̒l�Ƀv���X���鍂��
    float HEIGHT_DISTANCE = 1.5f;                                   //Aim�̍���

    static const float MAX_CAMERA_OFFSET = 5.0f;                    //cameraOffset�̍ő勗��
    static const float SUPRESS = 0.002f;                            //AimMove����Offset�̒l��}������l
    static const float MOVE_SUPRESS = 0.20f;                        //�������̗}���̒l
    static const float STOP_SUPRESS = 0.25f;                        //�~�܂鎞�̗}���̒l
    
    //�^�[�Q�b�g�֌W�[�[�[�[�[�[�[�[�[�[�[�[�[�[
    static const float TARGET_RANGE = 25.0f;                        //�^�[�Q�b�g�̗L���͈�
    static const float FOV_RADIAN = XMConvertToRadians(120) / 2.0f; //�^�[�Q�b�g�̗L���͈�
    static const float TARGET_RATIO = 0.3f;                         //�^�[�Q�b�g���̉�]��

    static const int TARGET_CHANGE_COOLTIME = 10;                   //TargetChange�̃N�[���_�E��
    static const float TARGET_CHANGE_VALUE = 40.0f;                 //�ǂ̂��炢�}�E�X�������΂�����
}

Aim::Aim(GameObject* parent)
    : GameObject(parent, "Aim"), cameraPosition_{ 0,0,0 }, cameraTarget_{ 0,0,0 }, aimDirection_{ 0,0,0 }, cameraOffset_{ 0,0,0 },
    compulsionTarget_{ 0,0,0 }, compulsionPosisiton_{ 0,0,0 }, pPlayer_(nullptr), 
    isMove_(true), isCompulsion_(false), compulsionTime_(0), iterations_(0), sign_(1), range_(0), moveDistance_(0),
    distanceDecrease_(0), center_{ 0,0,0,0 }, shakeSpeed_(0), rangeDecrease_(0),
    shakeDirection_{ 1,0,0,0 }, isValid_(true)
{
    mouseSensitivity = MOUSE_SPEED;
    targetDistanceBehind_ = DISTANCE_BEHIND_DEFAULT;
    distanceBehind_ = targetDistanceBehind_;
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
    if (!IsValid()) return;

    //�f�o�b�O�p
    if (Input::IsKey(DIK_1)) HEIGHT_DISTANCE += 0.1f;
    if (Input::IsKey(DIK_2)) HEIGHT_DISTANCE -= 0.1f;
    if (Input::IsKey(DIK_3)) targetDistanceBehind_ += 0.1f;
    if (Input::IsKey(DIK_4)) targetDistanceBehind_ -= 0.1f;

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
    XMVECTOR direction = CalculationVectorDirection(transform_.rotate_);
    XMStoreFloat3(&aimDirection_, -direction);

    //�v���C���[�̈ʒu���J�����̏œ_�Ƃ���        
    XMFLOAT3 plaPos = pPlayer_->GetPosition();
    cameraTarget_ = { plaPos.x + cameraOffset_.x, plaPos.y + HEIGHT_DISTANCE, plaPos.z + cameraOffset_.z };

    CameraShake();

    //RayCast�̑O�ɏ�������
    distanceBehind_ = distanceBehind_ + ((targetDistanceBehind_ - distanceBehind_) * 0.1f);
    XMVECTOR camPos = XMLoadFloat3(&cameraTarget_) + (direction * distanceBehind_);
    XMStoreFloat3(&cameraPosition_, camPos);

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

    OutputDebugStringA(std::to_string(distanceBehind_).c_str());
    OutputDebugString("\n");

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
    distanceBehind_ = distanceBehind_ - ((dist - targetDistanceBehind_) / (float)compulsionTime_);

    OutputDebugStringA(std::to_string(distanceBehind_).c_str());
    OutputDebugString("\n");

    //�߂蒆��Position��Target���v�Z����
    XMFLOAT3 position = pPlayer_->GetPosition();
    XMFLOAT3 target = { position.x + cameraOffset_.x, position.y + HEIGHT_DISTANCE, position.z + cameraOffset_.z };
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
    return;

    RayCastData data;
    XMFLOAT3 start = cameraTarget_;
    XMVECTOR vDir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    vDir = XMVector3Normalize(vDir);
    XMFLOAT3 dir = XMFLOAT3();
    XMStoreFloat3(&dir, vDir);
    data.start = start;
    data.dir = dir;
    float min = 0.0f;

    //���C���������E���苗������������
    if (min <= (targetDistanceBehind_)) {
        distanceBehind_ = min - HEIGHT_RAY;
    }

    XMVECTOR camPos = XMLoadFloat3(&cameraTarget_) + (vDir * distanceBehind_);
    XMStoreFloat3(&cameraPosition_, camPos);

}

void Aim::CalcMouseMove()
{
    XMFLOAT3 mouseMove = Input::GetMouseMove(); //�}�E�X�̈ړ��ʂ��擾
    transform_.rotate_.y += mouseMove.x * mouseSensitivity; //�������̉�]
    transform_.rotate_.x -= mouseMove.y * mouseSensitivity; //�c�����̉�]
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
