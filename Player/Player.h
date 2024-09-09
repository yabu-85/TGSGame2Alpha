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
    bool isFly_;                //�󒆂��ǂ���
    bool isClimb_;              //�o���Ԃ��ǂ���
    bool isCreative_;           //�N���G�C�e�B�u���[�h

    int hModel_;                //���f���ԍ�
    int hFPSModel_;             //FPS�\���p�̃��f��
    int hPict_;                 //�_���[�W�G�t�F�N�g
    int playerId_;              //�v���C���[�ԍ�

    int healthGaugeDrawTime_;   //���̂܂܁[
    int damageDrawTime_;        //�_���[�W�G�t�F�N�g�\������

    int waistPart_;             //�e�X�gPart�̃C���f�b�N�X
    int waistListIndex_[15];    //�e�X�gBone�̃C���f�b�N�X
    int downListIndex_[8];      //���X�g�C���f�b�N�X
    float waistRotateX_;        //�e�X�g����z�肵���{�[����RotateX
    float waistRotateY_;        //�e�X�g����z�肵���{�[����RotateY

    float gradually_;           //�ړ��X�s�[�h�̉����̒l
    float moveSpeed_;           //�ړ��X�s�[�h
    float gravity_;             //y���ړ���
   
    XMFLOAT3 playerMovement_;   //Normalize�̈ړ��ʂ�ۑ�����p
    XMFLOAT3 climbPos_;         //�o��ڕW���W

    Aim* pAim_;
    GunBase* pGunBase_;
    StateManager* pStateManager_;
    CapsuleCollider* pCapsuleCollider_;
    FixedHealthGauge* pFixedHealthGauge_;
    AnimationController* pAnimationController_;
    AnimationController* pFpsAnimationController_;

    //�֐�
    void CalcRotate(XMFLOAT3 pos, float ratio);

public:
    Player(GameObject* parent);
    ~Player() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //�_���[�W��^������
    void OnDamageDealt(const DamageInfo& damageInfo) override;

    //�_���[�W��^����ꂽ��
    void OnDamageReceived(const DamageInfo& damageInfo) override;

    //HealthGauge�\�����ԃZ�b�g
    void SetHealthGaugeDrawTime(int i) { healthGaugeDrawTime_ = i; }

    //Camera
    void TargetRotate(XMFLOAT3 pos, float ratio = 1.0f);
    void Rotate();
    void Rotate(float ratio);

    //�ړ�
    XMFLOAT3 GetInputMove();        //Input�̈ړ��ʎ擾
    XMVECTOR GetDirectionVec();     //�����Ă�������擾
    void Move(float f = 1.0f);      //�ړ�
    void CalcMove();                //Input���l���������
    void CalcNoMove();              //Input�l�����ĂȂ��A�����Ƃ��̌v�Z�p

    //�����蔻��(perDist=���e�͈�)
    bool StageFloarBounce(float perDist = 0.0f, float calcHeight = 0.0f);
    bool StageWallBounce();
    bool StageRoofBounce();

    //Jump
    bool IsReadyJump();
    void Jump();

    //Climb
    void CheckWallClimb();
    void WallClimb();

    //�t���O
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

