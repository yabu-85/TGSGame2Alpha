#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//�e���Ǘ�����N���X
class SniperGun : public GunBase
{
    bool rayHit_;   //Collision����ŕۑ��p

    int hModel_;        //���f���ԍ�
    int coolTime_;      //���N�[���_�E��
    int topBoneIndex_;  //�g�b�v�{�[���C���f�b�N�X
    int topPartIndex_;  //�g�b�v�p�[�c�C���f�b�N�X
    int rootBoneIndex_; //���[�g�{�[���C���f�b�N�X
    int rootPartIndex_; //���[�g�p�[�c�C���f�b�N�X
    int playerId_;      //�v���C���[ID

    Player* pPlayer_;
    AimCursor* pAimCursor_;

public:
    SniperGun(GameObject* parent);
    ~SniperGun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
