#pragma once
#include "../Engine/GameObject.h"

class Character;

// �e�ۂ̖��O��ID���Ǘ�����enum
enum class BulletType
{
    NORMAL,
    MAX
};

// �e�e�̊��N���X
class BulletBase : public GameObject
{
    BulletType bulletType_;

    // �o���b�g�̋��ʃp�����[�^
    struct BulletParameter
    {
        int damage_;            //�_���[�W
        int shotCoolTime_;      //�N�[���^�C��
        float speed_;           //�X�s�[�h
        int killTimer_;         //��������
        float collisionScale_;  //�����蔻��̃T�C�Y
        int isPenetration_;     //�ђ�   ����:1 �Ȃ�:0
    };
protected:
    BulletParameter parameter_; //�p�����[�^
    XMFLOAT3 move_;             //�ړ��̑��x�ƕ���

public:
    BulletBase(GameObject* parent, BulletType bulletType, std::string name);
    virtual ~BulletBase() = default;

    virtual void Shot(Character* chara, XMFLOAT3 pos) {};

    // �e�e�̎�ނ��擾
    BulletType GetBulletType() const { return bulletType_; }

    // �e�e�̃p�����[�^���擾
    BulletParameter GetBulletParameter() const { return parameter_; }

    // �e�e�̈ړ���ݒ�
    void SetMove(const XMFLOAT3& move) { move_ = move; }
};