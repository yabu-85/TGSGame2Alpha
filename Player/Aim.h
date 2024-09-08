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

struct CameraRotateShakeInfo {
    XMFLOAT2 rotate;    //回転量
    int time;           //かかる時間    

    CameraRotateShakeInfo() : rotate{ 0,0 }, time(0) {};
    CameraRotateShakeInfo(XMFLOAT2 r, int t) : rotate(r), time(t) {};
};

class Player;

class Aim : public GameObject
{
    //フラグ
    bool isFps_;                    //全ての挙動を止める
    bool isMove_;                   //エイムを動かすかどうか
    bool isCompulsion_;             //強制的に移動させる状態か
    bool isRotateShakeReturn_;      //RotateShakeの戻り処理する

    //FPSModeで使う
    int eyeBone, eyePart;
    int eyeBone1, eyePart1;

    //CameraShakeで使う
    int iterations_;                //反復回数
    float sign_;                    //計算用CameraShake
    float shakeSpeed_;              //Shakeする速さ
    float range_;                   //Shakeする強さ（長さ）
    float moveDistance_;            //計算用rangeの値を入れる
    float distanceDecrease_;        //反復ごとのspeed減衰地
    float rangeDecrease_;           //反復ごとのrange減衰地
    XMVECTOR center_;               //カメラShake計算用
    XMVECTOR shakeDirection_;       //カメラShake方向

    //RotateShakeで使う
    int rotateShakeTime_;           //回転にかかる時間
    int rotateShakeTimeCalc_;       //時間計測用
    XMFLOAT2 rotateShakeDirection_; //カメラShake方向
    XMFLOAT2 rotateShakeDirKeep_;   //カメラShake移動量保存
    XMFLOAT2 rotateShakeDirKeepSub_;//カメラShake移動量計算用

    //強制移動で使う
    int compulsionTime_;            //強制から戻るのに掛かる時間
    float compulsionComplement_;    //強制移動の補完具合
    XMFLOAT3 compulsionTarget_;     //強制時のカメラの焦点目標
    XMFLOAT3 compulsionPosisiton_;  //強制時のカメラの場所目標

    //Aimの基本情報
    float mouseSensitivity_;        //マウス感度
    float distanceHeight_;          //Aimの高さ
    float distanceHorizontal_;      //左右にずらす距離
    float distanceBehind_;          //後ろから映す距離
    float distanceTargetHeight_;    //高さ目標距離
    float distanceTargetHorizontal_;//左右目標距離
    float distanceTargetBehind_;    //距離目標距離
    float distanceIncreaseAmount_;  //高さ/左右/距離３つの増加量
    XMFLOAT3 cameraOffset_;         //カメラの移動量
    XMFLOAT3 cameraTarget_;         //カメラの焦点目標
    XMFLOAT3 cameraPosition_;       //カメラの場所目標
    XMFLOAT3 aimDirection_;         //現在の視点に基づいた進行方向ベクトル

    Player* pPlayer_;

    void FPSAim();          //FPS用
    void DefaultAim();      //三人称視点の計算
    void Compulsion();      //強制移動時の計算
    void BackCompulsion();  //強制移動から通常に移るときの計算

    void RayCastStage();
    void CalcMouseMove();
    void CameraShake();
    void CameraRotateShake();

public:
    Aim(GameObject* parent);
    ~Aim();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //座標を指定してポジションと焦点を設定する
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar);
    //強制から戻る時間と、補完の強さ指定するバージョン
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar, int returnTime, float complement);
    
    /// <summary>
    /// カメラの位置ごと動かす
    /// 反復回数、反復する距離、スピードの減衰地、スピード
    /// </summary>
    /// <param name="info"></param>
    void SetCameraShake(const CameraShakeInfo& info);
    void SetCameraShakeDirection(XMVECTOR v) { shakeDirection_ = v; }

    /// <summary>
    /// カメラを回転させる、X＝左右、Y＝上下
    /// 回転量、かかる時間
    /// </summary>
    /// <param name="info"></param>
    void SetCameraRotateShake(const CameraRotateShakeInfo& info);
    void SetCameraRotateReturn(bool b) { isRotateShakeReturn_ = b; }

    //----------------------------------------------------------------------------

    void SetAimFps(bool b) { isFps_ = b; }
    bool IsAimFps() { return isFps_; }
    void SetAimMove(bool b) { isMove_ = b; };
    bool IsAimMove() { return isMove_; }

    void SetDistanceIncreaseAmount(float f) { distanceIncreaseAmount_ = f; }
    void SetAimSensitivity(float parcent);
    
    XMFLOAT3 GetAimDirection() { return aimDirection_; }
    bool IsCompulsion() { return isCompulsion_; }

};