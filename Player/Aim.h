#pragma once
#include "../Engine/GameObject.h"

struct CameraShakeInfo {
    int iterat;             //反復回数
    float range;            //移動距離
    float range_decrease;   //移動距離減衰（1.0fの場合変化しない
    float speed;            //移動スピード
    float speed_decrease;   //移動スピード減衰（1.0fの場合変化しない

    CameraShakeInfo() : iterat(0), range(0.0f), range_decrease(1.0f), speed(0.0f), speed_decrease(1.0f) {};
    CameraShakeInfo(int ite, float ran, float spe) : iterat(ite), range(ran), range_decrease(1.0f), speed(spe), speed_decrease(1.0f) {};
    CameraShakeInfo(int ite, float ran, float ranD, float spe, float speD) : iterat(ite), range(ran), range_decrease(ranD), speed(spe), speed_decrease(speD) {};
};

class Player;

class Aim : public GameObject
{
    bool isValid_;                  //全ての挙動を止める
    bool isMove_;                   //エイムを動かすかどうか
    bool isCompulsion_;             //強制的に移動させる状態か
    
    int compulsionTime_;            //強制から戻るのに掛かる時間
    int iterations_;                //反復回数

    float sign_;                    //計算用CameraShake
    float shakeSpeed_;              //Shakeする速さ
    float range_;                   //Shakeする強さ（長さ）
    float moveDistance_;            //計算用rangeの値を入れる
    float distanceDecrease_;        //反復ごとのspeed減衰地
    float rangeDecrease_;           //反復ごとのrange減衰地
    XMVECTOR center_;
    XMVECTOR shakeDirection_;

    float distanceHeight_;          //Aimの高さ
    float distanceHorizontal_;      //左右にずらす距離
    float distanceBehind_;          //後ろから映す距離
    float distanceTargetBehind_;    //後ろから映す目標距離
    float mouseSensitivity_;        //マウス感度
    float compulsionComplement_;    //強制移動の補完具合

    XMFLOAT3 cameraTarget_;         //カメラの焦点目標
    XMFLOAT3 cameraPosition_;       //カメラの場所目標
    XMFLOAT3 compulsionTarget_;     //強制時のカメラの焦点目標
    XMFLOAT3 compulsionPosisiton_;  //強制時のカメラの場所目標
    XMFLOAT3 aimDirection_;         //現在の視点に基づいた進行方向ベクトル
    XMFLOAT3 cameraOffset_;         //カメラの移動量

    Player* pPlayer_;

    void DefaultAim();      //通常のAimの計算
    void Compulsion();      //強制移動時の計算
    void BackCompulsion();  //強制移動から通常に移るときの計算

    void RayCastStage();
    void CalcMouseMove();
    void CameraShake();

public:
    Aim(GameObject* parent);
    ~Aim();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    void SetValid(bool b) { isValid_ = b; }
    bool IsValid() { return isValid_; };
    void SetAimMove(bool b) { isMove_ = b; };
    bool IsAimMove() { return isMove_; }

    //進行方向ベクトルのAim情報を取得
    XMFLOAT3 GetAimDirection() { return aimDirection_; }

    //座標を指定してポジションと焦点を設定する
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar);
    //強制から戻る時間と、補完の強さ指定するバージョン
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar, int returnTime, float complement);
    //今補完中かどうか
    bool IsCompulsion() { return isCompulsion_; }

    //反復回数、反復する距離、スピードの減衰地、スピード
    void SetCameraShake(const CameraShakeInfo& info);
    void SetCameraShakeDirection(XMVECTOR v) { shakeDirection_ = v; }

};