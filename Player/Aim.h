#pragma once
#include "../Engine/GameObject.h"

struct CameraShakeInfo {
    int iterat;             //������
    float range;            //�ړ�����
    float range_decrease;   //�ړ����������i1.0f�̏ꍇ�ω����Ȃ�
    float speed;            //�ړ��X�s�[�h
    float speed_decrease;   //�ړ��X�s�[�h�����i1.0f�̏ꍇ�ω����Ȃ�

    CameraShakeInfo() : iterat(0), range(0.0f), range_decrease(1.0f), speed(0.0f), speed_decrease(1.0f) {};
    CameraShakeInfo(int ite, float ran, float spe) : iterat(ite), range(ran), range_decrease(1.0f), speed(spe), speed_decrease(1.0f) {};
    CameraShakeInfo(int ite, float ran, float ranD, float spe, float speD) : iterat(ite), range(ran), range_decrease(ranD), speed(spe), speed_decrease(speD) {};
};

class Player;

class Aim : public GameObject
{
    bool isValid_;                  //�S�Ă̋������~�߂�
    bool isMove_;                   //�G�C���𓮂������ǂ���
    bool isCompulsion_;             //�����I�Ɉړ��������Ԃ�
    
    int compulsionTime_;            //��������߂�̂Ɋ|���鎞��
    int iterations_;                //������

    float sign_;                    //�v�Z�pCameraShake
    float shakeSpeed_;              //Shake���鑬��
    float range_;                   //Shake���鋭���i�����j
    float moveDistance_;            //�v�Z�prange�̒l������
    float distanceDecrease_;        //�������Ƃ�speed�����n
    float rangeDecrease_;           //�������Ƃ�range�����n
    XMVECTOR center_;
    XMVECTOR shakeDirection_;

    float distanceHeight_;          //Aim�̍���
    float distanceHorizontal_;      //���E�ɂ��炷����
    float distanceBehind_;          //��납��f������
    float distanceTargetBehind_;    //��납��f���ڕW����
    float mouseSensitivity_;        //�}�E�X���x
    float compulsionComplement_;    //�����ړ��̕⊮�

    XMFLOAT3 cameraTarget_;         //�J�����̏œ_�ڕW
    XMFLOAT3 cameraPosition_;       //�J�����̏ꏊ�ڕW
    XMFLOAT3 compulsionTarget_;     //�������̃J�����̏œ_�ڕW
    XMFLOAT3 compulsionPosisiton_;  //�������̃J�����̏ꏊ�ڕW
    XMFLOAT3 aimDirection_;         //���݂̎��_�Ɋ�Â����i�s�����x�N�g��
    XMFLOAT3 cameraOffset_;         //�J�����̈ړ���

    Player* pPlayer_;

    void DefaultAim();      //�ʏ��Aim�̌v�Z
    void Compulsion();      //�����ړ����̌v�Z
    void BackCompulsion();  //�����ړ�����ʏ�Ɉڂ�Ƃ��̌v�Z

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

    //�i�s�����x�N�g����Aim�����擾
    XMFLOAT3 GetAimDirection() { return aimDirection_; }

    //���W���w�肵�ă|�W�V�����Əœ_��ݒ肷��
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar);
    //��������߂鎞�ԂƁA�⊮�̋����w�肷��o�[�W����
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar, int returnTime, float complement);
    //���⊮�����ǂ���
    bool IsCompulsion() { return isCompulsion_; }

    //�����񐔁A�������鋗���A�X�s�[�h�̌����n�A�X�s�[�h
    void SetCameraShake(const CameraShakeInfo& info);
    void SetCameraShakeDirection(XMVECTOR v) { shakeDirection_ = v; }

};