#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;
class AimCursor;

//�e���Ǘ�����N���X
class GunBase : public GameObject
{
protected:
    bool rayHit_;       //Collision����ŕۑ��p
    bool isFirstPerson_;//��l�̎��_���ǂ���(false���O�l��
    bool isPeeking_;

    int hModel_;        //���f���ԍ�
    int coolTime_;      //���N�[���_�E��
    int topBoneIndex_;  //�g�b�v�{�[���C���f�b�N�X
    int topPartIndex_;  //�g�b�v�p�[�c�C���f�b�N�X
    int rootBoneIndex_; //���[�g�{�[���C���f�b�N�X
    int rootPartIndex_; //���[�g�p�[�c�C���f�b�N�X
    int playerId_;      //�v���C���[ID

    int peekTime_;      //�`�����ݎ��Ԍv�Z�p

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void ShootBullet(BulletBase* pBullet);
public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override {};
    virtual void Initialize() override {};
    virtual void Update() override {};
    virtual void Draw() override {};
    virtual void Release() override {};
    virtual void OnCollision(GameObject* pTarget) override;

    bool IsFirstPerson() { return isFirstPerson_; }
    bool IsPeeking() { return isPeeking_; }
    AimCursor* GetAimCursor() { return pAimCursor_; }

    // �e�e�𐶐�����
    template<class T>
    void ShootBullet()
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        ShootBullet(pNewBullet);
    }
};
