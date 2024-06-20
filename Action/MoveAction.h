#pragma once
#include "BaseAction.h"
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;
struct RouteData;

//pTarget��null�o�Ȃ��ꍇ�́ApTarget�̕����ֈړ�
//pTarget��null�Ȃ�targetPos�ɕ����Ɉړ�
class MoveAction : public BaseAction
{
protected:
    bool isInRange_;    //Target�ɂ��Ă�����true
    float moveSpeed_;   //�ړ��X�s�[�h
    float moveRange_;   //�ړ��I������͈̔�
    XMFLOAT3 targetPos_;//�^�[�Q�b�g�̏ꏊ

public:
    MoveAction(GameObject* obj, float speed, float range);
    virtual ~MoveAction() override {};
    virtual void Update() override;

    //���̓G���悯��悤��move���v�Z
    void CalcDodge(XMVECTOR& move);

    void SetMoveSpeed(float speed) { moveSpeed_ = speed; }
    void SetMoveRange(float range) { moveRange_ = range; }
    
    //�ړ����I������ATarget�̈ʒu�ɂ������ړ��I�����
    bool IsInRange() { return isInRange_; }

    //Astar�̏ꍇUpdate�Ă΂Ȃ��ƈړ����Ȃ�
    virtual void SetTarget(XMFLOAT3 target) { targetPos_ = target; }
};

//Astar�̈ړ�
//TargetList�̌��̃f�[�^���ڕW�n�_
//�o�H�̍X�V��UpdatePath���g���ASetTarget��IsOutTarget�Ŏg��
class AstarMoveAction : public MoveAction {
    std::vector<RouteData> targetList_; //���̌o�H
    XMFLOAT3 lastTarget_;               //����TargetChara�̃|�W�V����
    int handle_;

public:
    AstarMoveAction(GameObject* obj, float speed, float range);
    void Update() override;

    //�ړ���߂�����
    void StopMove() { targetList_.clear(); }

    //���̖ڕW�̈ʒu�������Ȃ肷������true
    bool IsOutTarget(float range);

    //�V�����o�H�����߂�
    void UpdatePath(XMFLOAT3 target);

    //�f�o�b�O�p�̌o�H�\��
    void Draw();
};

//Target�n�_�������ADirection�̕����Ɉړ�����
//Target��ڕW�n�_�ł͂Ȃ��A���������߂邽�߂Ɏg�����߁ATarget�̒n�_�͒ʂ�߂���
class OrientedMoveAction : public MoveAction {
    XMVECTOR targetDirection_;  //�^�[�Q�b�g�ւ̃x�N�g��
    XMVECTOR direction_;    //�ړ��̌���
    XMVECTOR move_;         //�ړ���

public:
    OrientedMoveAction(GameObject* obj, float speed);
    ~OrientedMoveAction() override {};
    void Update() override;
    void SetDirection(XMVECTOR vec) { direction_ = vec; }
    void SetTarget(XMFLOAT3 target) override;

    //Direction���t��
    void InverseDirection();

};