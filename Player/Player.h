#pragma once
#include "../Engine/GameObject.h"

class Aim;
class StateManager;

class Player : public GameObject
{
    bool isFly_;                //空中かどうか
    bool isClimb_;              //登り状態かどうか
    bool isCreative_;           //クリエイティブモード

    int testModel_;             //デバッグ用
    int hModel_;                //モデル番号

    float gradually_;           //移動スピードの加減の値
    float moveSpeed_;           //移動スピード
    float gravity_;             //y軸移動量
   
    XMFLOAT3 playerMovement_;   //Normalizeの移動量を保存する用
    XMFLOAT3 climbPos_;         //登る目標座標

    Aim* pAim_;
    StateManager* pStateManager_;

    //関数
    void CalcRotate(XMFLOAT3 pos, float ratio);

public:
    Player(GameObject* parent);
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //Camera
    void TargetRotate(XMFLOAT3 pos, float ratio = 1.0f);
    void Rotate();
    void Rotate(float ratio);

    //移動
    XMFLOAT3 GetInputMove();        //Inputの移動量取得
    XMVECTOR GetDirectionVec();     //向いている方向取得
    void Move(float f = 1.0f);      //移動
    void CalcMove();                //Inputを考慮したやつ
    void CalcNoMove();              //Input考慮してない、滑るやつとかの計算用

    //当たり判定
    void StageFloarBounce();
    void StageWallBounce();

    //Jump
    bool IsReadyJump();
    void Jump();

    //Climb
    void CheckWallClimb();
    void WallClimb();

    bool IsFly() { return isFly_; }
    bool IsClimb() { return isClimb_; }

    int GetModelHandle() { return hModel_; }
    Aim* GetAim() { return pAim_; }

};

