#pragma once
#include "../Engine/GameObject.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"
#include <vector>

class Player;
class AimCursor;
enum class BulletType;

//�e���Ǘ�����N���X
class Gun : public GameObject
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
    std::shared_ptr<EFFEKSEERLIB::EFKTransform> mt;

public:
    Gun(GameObject* parent);
    ~Gun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;

    // �e�e�𐶐�����
    template<class T>
    void ShootBullet(BulletType type);

    AimCursor* GetAimCursor() { return pAimCursor_; }
};
