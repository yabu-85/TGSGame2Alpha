#include "Aim.h"
#include "Player.h"
#include "../Engine/Camera.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Fbx.h"
#include "../Engine/Model.h"
#include "../Stage/CollisionMap.h"
#include "../Other/InputManager.h"
#include "../Other/GameManager.h"
#include <vector>

namespace {
    bool FPSAimMode = true;

    const float UP_MOUSE_LIMIT = -80.0f;                     //回転限界値
    const float DOWN_MOUSE_LIMIT = 60.0f;                    //回転限界値
    
    const float COMPULSION_COMPLEMENT_DEFAULT = 0.06f;       //強制の補完具合デフォルトの
    const int COMPULSION_TIME_DEFAULT = 60;                  //強制から戻る時間
    
    const float MOUSE_SPEED_DEFAULT = 0.1f;                  //感度
    const float DISTANCE_HORIZONTAL_DEFAULT = 0.30f;         //どのくらい左右にずらすか
    const float DISTANCE_BEHIND_DEFAULT = 3.0f;              //どのくらい後ろから移すかのデフォルト値
    const float DISTANCE_HEIGHT_DEFAULT = 1.3f;              //Aimの高さ
    const float HEIGHT_RAY = 0.1f;                           //RayCastの値にプラスする高さ

    const XMVECTOR forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

}

Aim::Aim(GameObject* parent)
    : GameObject(parent, "Aim"), cameraPosition_{ 0,0,0 }, cameraTarget_{ 0,0,0 }, aimDirection_{ 0,0,0 }, cameraOffset_{ 0,0,0 },
    compulsionTarget_{ 0,0,0 }, compulsionPosisiton_{ 0,0,0 }, pPlayer_(nullptr), isMove_(true), isCompulsion_(false), compulsionTime_(0), 
    iterations_(0), sign_(1), range_(0), moveDistance_(0), distanceDecrease_(0), center_{ 0,0,0,0 }, shakeSpeed_(0), rangeDecrease_(0),
    shakeDirection_{ 1,0,0,0 }, isValid_(true), rotateShakeDirection_{ 1,0 }, rotateShakeDirKeep_{ 0, 0 }, rotateShakeTime_(0), distanceIncreaseAmount_(0.1f),
    rotateShakeTimeCalc_(-1), isRotateShakeReturn_(false), rotateShakeDirKeepSub_(XMFLOAT2())
{
    distanceHeight_ = DISTANCE_HEIGHT_DEFAULT;
    distanceTargetHeight_ = DISTANCE_HEIGHT_DEFAULT;
    distanceHorizontal_ = DISTANCE_HORIZONTAL_DEFAULT;
    distanceTargetHorizontal_ = DISTANCE_HORIZONTAL_DEFAULT;
    distanceBehind_ = DISTANCE_BEHIND_DEFAULT;
    distanceTargetBehind_ = DISTANCE_BEHIND_DEFAULT;

    static const float TARGET_BEHIND = 3.11f;
    static const float TARGET_HORIZONTAL = 0.0f;
    static const float TARGET_HEIGHT = 1.1f;
    distanceTargetBehind_ = TARGET_BEHIND;
    distanceTargetHorizontal_ = TARGET_HORIZONTAL;
    distanceTargetHeight_ = TARGET_HEIGHT;

    mouseSensitivity_ = MOUSE_SPEED_DEFAULT;
    compulsionComplement_ = COMPULSION_COMPLEMENT_DEFAULT;
}

Aim::~Aim()
{
}

void Aim::Initialize()
{
    pPlayer_ = static_cast<Player*>(GetParent());
    DefaultAim();

}

void Aim::Update()
{
    if (!IsValid()) return;
    
    //デバッグ用
    if (Input::IsKeyDown(DIK_5)) isMove_ = !isMove_;
    
    if (Input::IsKey(DIK_1)) distanceTargetHeight_ += 0.1f;
    if (Input::IsKey(DIK_2)) distanceTargetHeight_ -= 0.1f;
    if (Input::IsKey(DIK_3)) distanceTargetBehind_ += 0.1f;
    if (Input::IsKey(DIK_4)) distanceTargetBehind_ -= 0.1f;

    if (compulsionTime_ > 0) {
        //強制移動
        if (isCompulsion_) {
            Compulsion();
            isCompulsion_ = false;
            return;
        }
    }

    if (isMove_) CalcMouseMove();
   
    if (FPSAimMode) {
        FPSAim();
        return;
    }

    DefaultAim();

    Direct3D::playerCamX = transform_.rotate_.x;
    Direct3D::playerCamY = transform_.rotate_.y;

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

void Aim::SetCameraRotateShake(const CameraRotateShakeInfo& info)
{
    rotateShakeDirection_ = info.rotate;
    rotateShakeTime_ = info.time;
    rotateShakeTimeCalc_ = info.time;
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

void Aim::FPSAim()
{
    CameraRotateShake();

    //Eyeポジション設定
    int eyeBone, eyePart;
    int eyeBone1, eyePart1;
    int hFPSModel_ = pPlayer_->GetFPSModelHandle();
    Model::GetPartBoneIndex(hFPSModel_, "eye", &eyePart, &eyeBone);
    Model::GetPartBoneIndex(hFPSModel_, "eye.001", &eyePart1, &eyeBone1);
    XMFLOAT3 eyePos = Model::GetBoneAnimPosition(hFPSModel_, eyePart, eyeBone);
    XMFLOAT3 eyePos1 = Model::GetBoneAnimPosition(hFPSModel_, eyePart1, eyeBone1);

    int playerID = pPlayer_->GetPlayerId();
    Camera::SetPosition(eyePos, playerID);
    Camera::SetTarget(eyePos1, playerID);

    XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMMATRIX mView = mRotX * mRotY;
    XMVECTOR caDire = XMVector3TransformNormal(forwardVector, mView);
    XMStoreFloat3(&aimDirection_, -caDire);

    Direct3D::playerCamX = transform_.rotate_.x;
    Direct3D::playerCamY = transform_.rotate_.y;
}

void Aim::DefaultAim()
{
    CameraRotateShake();

    XMFLOAT3 plaPos = pPlayer_->GetPosition();
    XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(transform_.rotate_.x));
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMMATRIX mView = mRotX * mRotY;

    distanceBehind_ += (distanceTargetBehind_ - distanceBehind_) * distanceIncreaseAmount_;
    distanceHorizontal_ += (distanceTargetHorizontal_ - distanceHorizontal_) * distanceIncreaseAmount_;
    distanceHeight_ += (distanceTargetHeight_ - distanceHeight_) * distanceIncreaseAmount_;

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
    XMVECTOR camPos = caTarget + forwardV * distanceBehind_;
    XMStoreFloat3(&cameraPosition_, camPos);

    //カメラShake
    CameraShake();

    //RayCastしてその値を上書きする
    RayCastStage();
    
    //カメラ情報をセット
    Camera::SetPosition(cameraPosition_, pPlayer_->GetPlayerId());
    Camera::SetTarget(cameraTarget_, pPlayer_->GetPlayerId());
}

void Aim::Compulsion()
{
    XMStoreFloat3(&cameraPosition_, (XMVectorLerp(XMLoadFloat3(&cameraPosition_), XMLoadFloat3(&compulsionPosisiton_), compulsionComplement_)));
    XMStoreFloat3(&cameraTarget_, (XMVectorLerp(XMLoadFloat3(&cameraTarget_), XMLoadFloat3(&compulsionTarget_), compulsionComplement_)));

    XMVECTOR dir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    distanceBehind_ = XMVectorGetX(XMVector3Length(dir));

    RayCastStage();
    Camera::SetPosition(cameraPosition_, pPlayer_->GetPlayerId());
    Camera::SetTarget(cameraTarget_, pPlayer_->GetPlayerId());

    //強制移動時のRotateを求める
    dir = XMVector3Normalize(dir);
    float rotationY = atan2f(XMVectorGetX(dir), XMVectorGetZ(dir));
    float rotationX = -asinf(XMVectorGetY(dir));
    transform_.rotate_.x = compulsionPosisiton_.x = XMConvertToDegrees(rotationX);
    transform_.rotate_.y = compulsionPosisiton_.y = XMConvertToDegrees(rotationY);

    //Rotateの結果を使ってAimDirectionを計算してセット
    XMVECTOR direction = CalculationVectorDirection(transform_.rotate_);
    XMStoreFloat3(&aimDirection_, -direction);
}

void Aim::BackCompulsion()
{
    CalcMouseMove();

    //強制時の視点の距離を求める
    XMVECTOR dir = XMLoadFloat3(&cameraPosition_) - XMLoadFloat3(&cameraTarget_);
    float dist = XMVectorGetX(XMVector3Length(dir));
    distanceBehind_ = distanceBehind_ - ((dist - distanceTargetBehind_) / (float)compulsionTime_);

    //戻り中のPositionとTargetを計算する
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

    //計算結果を使ってRayCastするそれをセット
    RayCastStage();
    Camera::SetPosition(cameraPosition_, pPlayer_->GetPlayerId());
    Camera::SetTarget(cameraTarget_, pPlayer_->GetPlayerId());

    //強制移動時のRotate・Directionを求める
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
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(cameraPosition_, &data);

    //レイ当たった・判定距離内だったら
    if (data.dist <= distanceBehind_ + HEIGHT_RAY) {
        distanceBehind_ = data.dist - HEIGHT_RAY;
    }
    
    XMVECTOR camPos = XMLoadFloat3(&cameraTarget_) + (vDir * distanceBehind_);
    XMStoreFloat3(&cameraPosition_, camPos);
}

void Aim::CalcMouseMove()
{
    //マウスの移動量を取得
    XMFLOAT3 mouseMove = XMFLOAT3();
    int playerId = pPlayer_->GetPlayerId();

#if PC_CTRL
    if (playerId == 0) mouseMove = Input::GetMouseMove();
    else {
        static const float STICK_SPEED = 20.0f;
        XMFLOAT3 rStickMove = Input::GetPadStickR(0);
        mouseMove = XMFLOAT3(rStickMove.x * STICK_SPEED, -(rStickMove.y * STICK_SPEED), 0.0f);
    }
#else
    static const float STICK_SPEED = 20.0f;
    XMFLOAT3 rStickMove = Input::GetPadStickR(playerId);
    mouseMove = XMFLOAT3(rStickMove.x * STICK_SPEED, -(rStickMove.y * STICK_SPEED), 0.0f);
#endif

    XMFLOAT2 move = XMFLOAT2(mouseMove.x * mouseSensitivity_, mouseMove.y * mouseSensitivity_);
    transform_.rotate_.y += move.x; //横方向の回転
    transform_.rotate_.x -= move.y; //縦方向の回転
    if (transform_.rotate_.x <= UP_MOUSE_LIMIT) transform_.rotate_.x = UP_MOUSE_LIMIT;
    if (transform_.rotate_.x >= DOWN_MOUSE_LIMIT) transform_.rotate_.x = DOWN_MOUSE_LIMIT;

    //RotateShakeの処理もやる
    if (rotateShakeTimeCalc_ >= 0) {
        if (rotateShakeDirKeep_.x == 0.0f) return;
        bool sing = signbit(move.y);

        //RotateShake抑制の値
        if (sing) {
            rotateShakeDirKeep_.x += move.y;
            if (rotateShakeDirKeep_.x > 0.0f) rotateShakeDirKeep_.x = 0.0f;
        }
        else { 
            rotateShakeDirKeep_.x -= move.y;
            if (rotateShakeDirKeep_.x < 0.0f) rotateShakeDirKeep_.x = 0.0f;
        }

    }

}

void Aim::CameraShake()
{
    if (iterations_ <= -1) return;

    //カメラシェイク終了戻る処理
    if (iterations_ == 0) {
        float dist = XMVectorGetX(XMVector3Length(center_));
        XMVECTOR vec = -XMVector3Normalize(center_);

        //目標地点につくよ
        if (dist < moveDistance_ * shakeSpeed_) {
            moveDistance_ = dist;
            iterations_--;
            center_ = XMVectorZero();
            sign_ = 1.0f;
        }

        vec *= moveDistance_ * shakeSpeed_;
        center_ += vec;

        cameraPosition_.x += center_.m128_f32[0];
        cameraPosition_.y += center_.m128_f32[1];
        cameraPosition_.z += center_.m128_f32[2];
        cameraTarget_.x += center_.m128_f32[0];
        cameraTarget_.y += center_.m128_f32[1];
        cameraTarget_.z += center_.m128_f32[2];
        return;
    }

    //移動方向・移動目標場所を計算
    XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(transform_.rotate_.y));
    XMVECTOR shakeDirection = XMVector3Normalize(XMVector3TransformNormal((shakeDirection_), mRotY));
    XMVECTOR nextPosition = center_ + shakeDirection;

    //方向調べて移動させる
    XMVECTOR directionVector = nextPosition - center_;
    directionVector = XMVector3NormalizeEst(directionVector) * moveDistance_ * shakeSpeed_;
    center_ += directionVector;

    //同じ方向か調べる
    bool sign2 = (center_.m128_f32[0] + center_.m128_f32[1] + center_.m128_f32[2] / 3) > 0.0f;
    bool sign3 = (directionVector.m128_f32[0] + directionVector.m128_f32[1] + directionVector.m128_f32[2] / 3) > 0;

    //目標地点を超えたか調べる
    float cen = XMVectorGetX(XMVector3Length(center_));
    if (sign2 == sign3 && cen > range_) {
        iterations_--;
        sign_ *= -1.0f;
        shakeDirection_ *= -1.0f;
        center_ = XMVector3Normalize(center_) * range_;
       
        moveDistance_ *= distanceDecrease_;
        range_ *= rangeDecrease_;
    }

    cameraPosition_.x += center_.m128_f32[0];
    cameraPosition_.y += center_.m128_f32[1];
    cameraPosition_.z += center_.m128_f32[2];
    cameraTarget_.x += center_.m128_f32[0];
    cameraTarget_.y += center_.m128_f32[1];
    cameraTarget_.z += center_.m128_f32[2];
}

void Aim::CameraRotateShake()
{
    //戻る処理
    static const float REM_TIME = 5.0f;
    if (rotateShakeTimeCalc_ <= 0) {
        if (!isRotateShakeReturn_) return;

        XMFLOAT2 move = XMFLOAT2();
        move.x = rotateShakeDirKeepSub_.x / REM_TIME;
        move.y = rotateShakeDirKeepSub_.y / REM_TIME;

        transform_.rotate_.x -= move.x;
        transform_.rotate_.y -= move.y;
        rotateShakeDirKeep_.x -= move.x;
        rotateShakeDirKeep_.y -= move.y;

        rotateShakeTimeCalc_--;
        if (rotateShakeTimeCalc_ <= -5) {
            isRotateShakeReturn_ = false;
            rotateShakeDirKeep_ = XMFLOAT2(0.0f, 0.0f);
            rotateShakeDirKeepSub_ = XMFLOAT2(0.0f, 0.0f);
        }
        return;
    }

    rotateShakeTimeCalc_--;
    XMFLOAT2 move = XMFLOAT2();
    move.x = rotateShakeDirection_.y / (float)rotateShakeTime_;
    move.y = rotateShakeDirection_.x / (float)rotateShakeTime_;
    
    transform_.rotate_.x += move.x;
    transform_.rotate_.y += move.y;
    rotateShakeDirKeep_.x += move.x;
    rotateShakeDirKeep_.y += move.y;
    rotateShakeDirKeepSub_ = rotateShakeDirKeep_;
}
