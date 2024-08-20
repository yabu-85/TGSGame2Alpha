#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;
class AimCursor;

//�e���Ǘ�����N���X
class GunBase : public GameObject
{
protected:
    bool rayHit_;           //Collision����ŕۑ��p
    bool isFirstPerson_;    //��l�̎��_���ǂ���(false���O�l��
    bool isPeeking_;        //���S�ɂ̂������݂��Ă����Ԃ�

    int hModel_;            //���f���ԍ�
    int hPlayerModel_;      //�v���C���[�̃��f���ԍ�
    int hPlayerFPSModel_;   //FPS�p���f���ԍ�

    int topBoneIndex_;      //�g�b�v�{�[���C���f�b�N�X
    int topPartIndex_;      //�g�b�v�p�[�c�C���f�b�N�X
    int rootBoneIndex_;     //���[�g�{�[���C���f�b�N�X
    int rootPartIndex_;     //���[�g�p�[�c�C���f�b�N�X
    int handBoneIndex_;     //�v���C���[�̎�̃{�[��
    int handPartIndex_;     //�v���C���[�̎�̃p�[�c

    int playerId_;                  //�v���C���[ID
    int coolTime_;                  //���N�[���_�E��
    int peekTime_;                  //�`�����ݎ��Ԍv�Z�p
    
    int reloadTime_;                //�����[�h���ԍő�l
    int currentReloadTime_;          //�����[�h���ԕۑ�
    int magazineCount_;             //�}�K�W���ő呕�e��
    int currentMagazineCount_;      //�}�K�W���ő呕�e��

    Player* pPlayer_;
    AimCursor* pAimCursor_;

    void LoadGunJson(std::string fileName);         //�e�̏��ǂݍ���
    void SetGunHandPosition();                      //��̈ʒu�ɏe�̃|�W�V���������킹��
    void Reload();                                  //�����[�h���̏���
    virtual void ShotBullet(BulletBase* pBullet);   //�e�e���˂̏���
    virtual void PressedShot() {};                  //���˂̃{�^��������
    virtual bool PressedReload() { return false; }; //�����[�h�̃{�^��������

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

    //�e�e�𐶐�����
    template<class T>
    inline void ShotBullet()
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        ShotBullet(pNewBullet);
    }

};
