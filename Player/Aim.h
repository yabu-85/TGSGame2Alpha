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

struct CameraRotateShakeInfo {
    XMFLOAT2 rotate;    //��]��
    int time;           //�����鎞��    

    CameraRotateShakeInfo() : rotate{ 0,0 }, time(0) {};
    CameraRotateShakeInfo(XMFLOAT2 r, int t) : rotate(r), time(t) {};
};

class Player;

class Aim : public GameObject
{
    //�t���O
    bool isFps_;                    //�S�Ă̋������~�߂�
    bool isMove_;                   //�G�C���𓮂������ǂ���
    bool isCompulsion_;             //�����I�Ɉړ��������Ԃ�
    bool isRotateShakeReturn_;      //RotateShake�̖߂菈������

    //FPSMode�Ŏg��
    int eyeBone, eyePart;
    int eyeBone1, eyePart1;

    //CameraShake�Ŏg��
    int iterations_;                //������
    float sign_;                    //�v�Z�pCameraShake
    float shakeSpeed_;              //Shake���鑬��
    float range_;                   //Shake���鋭���i�����j
    float moveDistance_;            //�v�Z�prange�̒l������
    float distanceDecrease_;        //�������Ƃ�speed�����n
    float rangeDecrease_;           //�������Ƃ�range�����n
    XMVECTOR center_;               //�J����Shake�v�Z�p
    XMVECTOR shakeDirection_;       //�J����Shake����

    //RotateShake�Ŏg��
    int rotateShakeTime_;           //��]�ɂ����鎞��
    int rotateShakeTimeCalc_;       //���Ԍv���p
    XMFLOAT2 rotateShakeDirection_; //�J����Shake����
    XMFLOAT2 rotateShakeDirKeep_;   //�J����Shake�ړ��ʕۑ�
    XMFLOAT2 rotateShakeDirKeepSub_;//�J����Shake�ړ��ʌv�Z�p

    //�����ړ��Ŏg��
    int compulsionTime_;            //��������߂�̂Ɋ|���鎞��
    float compulsionComplement_;    //�����ړ��̕⊮�
    XMFLOAT3 compulsionTarget_;     //�������̃J�����̏œ_�ڕW
    XMFLOAT3 compulsionPosisiton_;  //�������̃J�����̏ꏊ�ڕW

    //Aim�̊�{���
    float mouseSensitivity_;        //�}�E�X���x
    float distanceHeight_;          //Aim�̍���
    float distanceHorizontal_;      //���E�ɂ��炷����
    float distanceBehind_;          //��납��f������
    float distanceTargetHeight_;    //�����ڕW����
    float distanceTargetHorizontal_;//���E�ڕW����
    float distanceTargetBehind_;    //�����ڕW����
    float distanceIncreaseAmount_;  //����/���E/�����R�̑�����
    XMFLOAT3 cameraOffset_;         //�J�����̈ړ���
    XMFLOAT3 cameraTarget_;         //�J�����̏œ_�ڕW
    XMFLOAT3 cameraPosition_;       //�J�����̏ꏊ�ڕW
    XMFLOAT3 aimDirection_;         //���݂̎��_�Ɋ�Â����i�s�����x�N�g��

    Player* pPlayer_;

    void FPSAim();          //FPS�p
    void DefaultAim();      //�O�l�̎��_�̌v�Z
    void Compulsion();      //�����ړ����̌v�Z
    void BackCompulsion();  //�����ړ�����ʏ�Ɉڂ�Ƃ��̌v�Z

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

    //���W���w�肵�ă|�W�V�����Əœ_��ݒ肷��
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar);
    //��������߂鎞�ԂƁA�⊮�̋����w�肷��o�[�W����
    void SetCompulsion(XMFLOAT3 pos, XMFLOAT3 tar, int returnTime, float complement);
    
    /// <summary>
    /// �J�����̈ʒu���Ɠ�����
    /// �����񐔁A�������鋗���A�X�s�[�h�̌����n�A�X�s�[�h
    /// </summary>
    /// <param name="info"></param>
    void SetCameraShake(const CameraShakeInfo& info);
    void SetCameraShakeDirection(XMVECTOR v) { shakeDirection_ = v; }

    /// <summary>
    /// �J��������]������AX�����E�AY���㉺
    /// ��]�ʁA�����鎞��
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