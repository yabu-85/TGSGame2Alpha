#pragma once
#include "../Engine/GameObject.h"

class Player;
class AimCursor;
class BulletBase;

//�e���Ǘ�����N���X
class GunBase : public GameObject
{
protected:
    bool rayHit_;       //Collision����ŕۑ��p
    bool isFirstPerson_;//��l�̎��_���ǂ���(false���O�l��

    int hModel_;        //���f���ԍ�
    int coolTime_;      //���N�[���_�E��
    int topBoneIndex_;  //�g�b�v�{�[���C���f�b�N�X
    int topPartIndex_;  //�g�b�v�p�[�c�C���f�b�N�X
    int rootBoneIndex_; //���[�g�{�[���C���f�b�N�X
    int rootPartIndex_; //���[�g�p�[�c�C���f�b�N�X
    int playerId_;      //�v���C���[ID

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void ShootBulletCalc(BulletBase* pBullet);

public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override;
    virtual void Initialize() override = 0;
    virtual void Update() override = 0;
    virtual void Draw() override = 0;
    virtual void Release() override = 0;
    virtual void OnCollision(GameObject* pTarget) override;

    // �e�e�𐶐�����
    template<class T>
    void ShootBullet();

    bool IsFirstPerson() { return isFirstPerson_; }
    AimCursor* GetAimCursor() { return pAimCursor_; }
};
