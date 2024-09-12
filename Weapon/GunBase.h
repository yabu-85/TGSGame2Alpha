#pragma once
#include "BulletBase.h"
#include "../Engine/GameObject.h"

class Player;

//�e���Ǘ�����N���X
class GunBase : public GameObject
{
protected:
    bool rayHit_;           //Collision����ŕۑ��p
    bool isPeeking_;        //���S�ɂ̂������݂��Ă����Ԃ�

    int hModel_;            //���f���ԍ�
    int hFpsPlayerModel_;   //�v���C���[�̃��f���ԍ�

    int topBoneIndex_;      //�g�b�v�{�[���C���f�b�N�X
    int topPartIndex_;      //�g�b�v�p�[�c�C���f�b�N�X
    int rootBoneIndex_;     //���[�g�{�[���C���f�b�N�X
    int rootPartIndex_;     //���[�g�p�[�c�C���f�b�N�X
    int handBoneIndex_;     //�v���C���[�̎�̃{�[��
    int handPartIndex_;     //�v���C���[�̎�̃p�[�c

    int playerId_;                  //�v���C���[ID
    int animTime_;                  //�A�j���[�V�������Ԍv�Z
    int coolTime_;                  //���N�[���_�E��
    int magazineCount_;             //�}�K�W���ő呕�e��
    int currentMagazineCount_;      //�}�K�W���ő呕�e��

    int reloadTime_;                //�����[�h���ԍő�l
    int currentReloadTime_;         //�����[�h���ԕۑ�

    int peekTime_;                  //�`�����ݎ��ԍő�l
    int currentPeekTime_;           //�`�����ݎ��Ԍv�Z�p
    float peekZoom_;                //�Y�[������x����

    float hipFireAccuracy_;         //���������̏W�e��
    float aimingAccuracy_;          //�`�����ݎ��̏W�e��
    float accuracyDecrease_;        //�W�e�������炷�l
    float accuracyRecovery_;        //�W�e�����񕜂�����l
    float currentAccuracy_;         //�v�Z�p���݂̏W�e��
    float maxAccuracy_;             //�ő�W�e��

    Player* pPlayer_;

    void LoadGunJson(std::string fileName);         //�e�̏��ǂݍ���
    void SetGunHandPosition();                      //��̈ʒu�ɏe�̃|�W�V���������킹��
    
    void EnterReload();                             //�����[�h���n�߂�
    bool Reload();                                  //�����[�h���̏����i�����[�h�ł�����True�j
    void FinishedReload();                          //�����[�h�I��������̏���
    
    void Peeking();                                 //�`�����ݏ����i�A�j���[�V�����͂��Ȃ��j
    void ShotVFX();                                 //ShotVFX�Đ�

    virtual void ShotBullet(BulletBase* pBullet);       //�e�e���˂̏���
    virtual void ShotFPSBullet(BulletBase* pBullet);    //�e�e���˂̏���

    virtual void PressedShot() {};                      //���˂̃{�^��������
    virtual bool PressedReload() { return false; };     //�����[�h�̃{�^��������
    virtual void ResetReload() {};                      //�����[�h�̃L�����Z��

public:
    GunBase(GameObject* parent, const std::string& name);
    virtual ~GunBase() override;
    virtual void Initialize() override {};
    virtual void Update() override {};
    virtual void Draw() override {};
    virtual void Release() override {};
    virtual void OnCollision(GameObject* pTarget) override;

    // ���� / �`������ + currentAcct
    float GetAccuracy();

    bool IsPeeking() { return isPeeking_; }
    int GetCurrentMagazineCount() { return currentMagazineCount_; }
    int GetMaxMagazineCount() { return magazineCount_; }
    int GetReloadTime() { return reloadTime_; }

    //�e�e�𐶐�����
    template<class T>
    inline BulletBase* ShotBullet(std::string jsonName)
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        pNewBullet->LoadBulletParameter(jsonName);
        ShotBullet(pNewBullet);
        return pNewBullet;
    }

    //�e�e�𐶐�����
    template<class T>
    inline BulletBase* ShotFpsBullet(std::string jsonName)
    {
        BulletBase* pNewBullet = Instantiate<T>(GetParent()->GetParent());
        pNewBullet->LoadBulletParameter(jsonName);
        ShotFPSBullet(pNewBullet);
        return pNewBullet;
    }

};
