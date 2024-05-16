#pragma once
#include "../Engine/GameObject.h"

class SphereCollider;
class Aim;
class StateManager;
class PlayerWeapon;
class AnimationController;

enum class PLAYER_ANIMATION {
    IDLE = 0,
    RUN,
    RORING,
    BACK_STEP,
    DAMAGE_SMALL,
    DAMAGE_MID,
    DEAD,
    ATTACK1,
    ATTACK2,
    ATTACK3,
    STONE_ATTACK,
    MAX,
};

const int PLAYER_ANIMATION_DATA[(int)PLAYER_ANIMATION::MAX][2] {
    {0, 120},     //Idle
    { 548, 590 }, //Run
    { 120, 175 }, //Roring
    { 500, 546 }, //Back
    { 175, 210 }, //DamageSmall
    { 210, 260 }, //DamageMid
    { 260, 495 }, //Dead
    { 595, 665 }, //Attack1
    { 665, 720 }, //Attack2
    { 675, 720 }, //Attack3
    { 725, 825 }, //Stone
};

class Player : public GameObject
{
    int testModel_;

    int hModel_;                //モデル番号
    float gradually_;           //移動スピードの加減の値
    XMFLOAT3 playerMovement_;   //Normalizeの移動量を保存する用

    Aim* pAim_;
    
    void CalcRotate(XMFLOAT3 pos, float ratio);
public:
    Player(GameObject* parent);
    ~Player();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //Aim
    void TargetRotate(XMFLOAT3 pos, float ratio = 1.0f);
    void Rotate();
    void Rotate(float ratio);

    //移動
    XMFLOAT3 GetInputMove();        //Inputの移動量取得
    void FrontMove(float f = 1.0f); //前への移動
    void BackMove(float f = 1.0f); //前への移動
    void Move(float f = 1.0f);      //移動
    void CalcMove();                //Inputを考慮したやつ
    void CalcNoMove();              //Input考慮してない、滑るやつとかの計算用
    XMVECTOR GetDirectionVec();     //向いている方向（正規化済み
    void ResetMovement();

    int GetModelHandle() { return hModel_; }
    Aim* GetAim() { return pAim_; }
};

