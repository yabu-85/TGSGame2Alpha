#pragma once
#include "../Character/Character.h"

class Aim;
class StateManager;

class Player : public Character
{
    bool isFly_;                //�󒆂��ǂ���
    bool isClimb_;              //�o���Ԃ��ǂ���
    bool isCreative_;           //�N���G�C�e�B�u���[�h

    int hModel_;                //���f���ԍ�

    int waistPart_;         //�e�X�gPart�̃C���f�b�N�X
    int waistListIndex_[8];    //�e�X�gBone�̃C���f�b�N�X
    float waistRotateY_;    //�e�X�g����z�肵���{�[����RotateY

    float gradually_;           //�ړ��X�s�[�h�̉����̒l
    float moveSpeed_;           //�ړ��X�s�[�h
    float gravity_;             //y���ړ���
   
    XMFLOAT3 playerMovement_;   //Normalize�̈ړ��ʂ�ۑ�����p
    XMFLOAT3 climbPos_;         //�o��ڕW���W

    Aim* pAim_;
    StateManager* pStateManager_;

    //�֐�
    void CalcRotate(XMFLOAT3 pos, float ratio);

public:
    Player(GameObject* parent);
    ~Player() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

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
    void StageFloarBounce(float perDist = 0.0f, float calcHeight = 0.0f);
    void StageWallBounce();
    void StageRoofBounce();

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

