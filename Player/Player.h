#pragma once
#include "../Character/Character.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

class Aim;
class GunBase;
class StateManager;
class CapsuleCollider;
class FixedHealthGauge;
class AnimationController;

enum class PLAYER_ANIMATION {
    IDLE = 0,
    PEEK_START,
    PEEK,
    PEEK_END,
    RUN,
    RELOAD,
    DEAD,
    MAX,
};

const int PLAYER_ANIMATION_DATA[(int)PLAYER_ANIMATION::MAX][2]{
    {0, 120},     //Idle
    {120, 140},   //PeekStart
    {140, 260},   //Peek
    {260, 280},   //PeekEnd
    {290, 325},   //Run
    {340, 390},   //Reload
    {400, 630},   //Dead
};

class Player : public Character
{
    bool isFly_;                //空中かどうか
    bool isClimb_;              //登り状態かどうか
    bool isCreative_;           //クリエイティブモード

    int hModel_;                //モデル番号
    int hFPSModel_;             //FPS表示用のモデル
    int hPict_;                 //ダメージエフェクト
    int playerId_;              //プレイヤー番号

    int healthGaugeDrawTime_;   //そのままー
    int damageDrawTime_;        //ダメージエフェクト表示時間

    int waistPart_;             //テストPartのインデックス
    int waistListIndex_[15];    //テストBoneのインデックス
    int downListIndex_[8];      //リストインデックス
    float waistRotateX_;        //テスト腰を想定したボーンのRotateX
    float waistRotateY_;        //テスト腰を想定したボーンのRotateY

    float gradually_;           //移動スピードの加減の値
    float moveSpeed_;           //移動スピード
    float gravity_;             //y軸移動量
   
    XMFLOAT3 playerMovement_;   //Normalizeの移動量を保存する用
    XMFLOAT3 climbPos_;         //登る目標座標

    Aim* pAim_;
    GunBase* pGunBase_;
    StateManager* pStateManager_;
    CapsuleCollider* pCapsuleCollider_;
    FixedHealthGauge* pFixedHealthGauge_;
    AnimationController* pAnimationController_;
    AnimationController* pFpsAnimationController_;

    //関数
    void CalcRotate(XMFLOAT3 pos, float ratio);

public:
    Player(GameObject* parent);
    ~Player() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //ダメージを与えた時
    void OnDamageDealt(const DamageInfo& damageInfo) override;

    //ダメージを与えられた時
    void OnDamageReceived(const DamageInfo& damageInfo) override;

    //HealthGauge表示時間セット
    void SetHealthGaugeDrawTime(int i) { healthGaugeDrawTime_ = i; }

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

    //当たり判定(perDist=許容範囲)
    bool StageFloarBounce(float perDist = 0.0f, float calcHeight = 0.0f);
    bool StageWallBounce();
    bool StageRoofBounce();

    //Jump
    bool IsReadyJump();
    void Jump();

    //Climb
    void CheckWallClimb();
    void WallClimb();

    //フラグ
    bool IsFly() { return isFly_; }
    bool IsClimb() { return isClimb_; }
    void SetClimb(bool b) { isClimb_ = b; }

    int GetModelHandle() { return hModel_; }
    int GetFPSModelHandle() { return hFPSModel_; }
    int GetPlayerId() { return playerId_; }

    Aim* GetAim() { return pAim_; }
    GunBase* GetGun() { return pGunBase_; }
    StateManager* GetStateManager() { return pStateManager_; }
    FixedHealthGauge* GetFixedHealthGauge() { return pFixedHealthGauge_; }
    AnimationController* GetAnimationController() { return pAnimationController_; }
    AnimationController* GetFpsAnimationController() { return pFpsAnimationController_; }

};

