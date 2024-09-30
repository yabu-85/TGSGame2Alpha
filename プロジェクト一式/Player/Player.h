#pragma once
#include "../Character/Character.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

class Aim;
class GunBase;
class AimCursor;
class StateManager;
class CapsuleCollider;
class FixedHealthGauge;
class AnimationController;
class SphereCollider;

enum class PLAYER_ANIMATION {
    IDLE = 0,
    PEEK_START,
    PEEK_END,
    PEEK,
    RUN,
    RELOAD,
    DEAD,
    JUMP,
    JUMP_START,
    JUMP_END,
    MAX,
};

const int PLAYER_ANIMATION_DATA[(int)PLAYER_ANIMATION::MAX][2]{
    {0, 120},     //Idle
    {120, 140},   //PeekStart
    {260, 280},   //PeekEnd
    {140, 260},   //Peek
    {290, 326},   //Run
    {340, 390},   //Reload
    {400, 630},   //Dead
    {651, 651},   //Jump
    {640, 651},   //JumpStart
    {659, 677},   //JumpEnd
};

class Player : public Character
{
    bool isFly_;                //�󒆂��ǂ���
    bool isClimb_;              //�o���Ԃ��ǂ���
    bool isCreative_;           //�N���G�C�e�B�u���[�h
    bool isActionReady_;        //�ړ���W�����v�\��Ԃ�

    int playerId_;              //�v���C���[�ԍ�
    int hUpModel_;              //�㔼�g���f���ԍ�
    int hDownModel_;            //�����g���f���ԍ�
    int hFPSModel_;             //�㔼�gFPS�p���f��

    int hPict_;                 //�_���[�W�G�t�F�N�g
    int healthGaugeDrawTime_;   //�����HealthGauge�`�掞��
    int damageDrawTime_;        //�_���[�W�G�t�F�N�g�\������

    int bonePart_;              //Part�̃C���f�b�N�X
    int upListIndex_[15];       //Bone�̃C���f�b�N�X
    int downListIndex_[8];      //Bone�̃C���f�b�N�X
    float orientRotateX_;       //�{�[����RotateX�i�㔼�g
    float lowerBodyRotate_;     //�����g�̉�]Y

    float gradually_;           //�ړ��X�s�[�h�̉����̒l
    float moveSpeed_;           //�ړ��X�s�[�h
    float gravity_;             //y���ړ���
   
    XMFLOAT3 playerMovement_;   //Normalize�̈ړ��ʂ�ۑ�����p
    XMFLOAT3 climbPos_;         //�o��ڕW���W

    Aim* pAim_;
    GunBase* pGunBase_;
    AimCursor* pAimCursor_;
    StateManager* pStateManager_;
    CapsuleCollider* pCapsuleCollider_;
    FixedHealthGauge* pFixedHealthGauge_;
    AnimationController* pUpAnimationController_;
    AnimationController* pDownAnimationController_;
    AnimationController* pFpsAnimationController_;

    SphereCollider* pSphereCollider_[2];            //�����蔻��Ŏg�p�i�J�v�Z���̏㉺�Ƒz�肵�Ďg��

    //�֐�
    void CalcDownBodyRotate();
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
    bool IsActionReady() { return isActionReady_; }

    //�t���O�Z�b�g
    void SetClimb(bool b) { isClimb_ = b; }
    void SetActionReady(bool b) { isActionReady_ = b; }

    int GetPlayerId() { return playerId_; }
    int GetUpModelHandle() { return hUpModel_; }
    int GetDownModelHandle() { return hDownModel_; }
    int GetFPSModelHandle() { return hFPSModel_; }

    Aim* GetAim() { return pAim_; }
    GunBase* GetGun() { return pGunBase_; }
    AimCursor* GetAimCursor() { return pAimCursor_; }
    StateManager* GetStateManager() { return pStateManager_; }
    FixedHealthGauge* GetFixedHealthGauge() { return pFixedHealthGauge_; }
    AnimationController* GetUpAnimationController() { return pUpAnimationController_; }
    AnimationController* GetDownAnimationController() { return pDownAnimationController_; }
    AnimationController* GetFpsAnimationController() { return pFpsAnimationController_; }

};

