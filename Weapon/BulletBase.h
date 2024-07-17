#pragma once
#include "../Engine/GameObject.h"

class Character;

// �e�e�̊��N���X
class BulletBase : public GameObject
{
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
    int playerId_;              //�_���[�W�`��̂��߂�
    BulletParameter parameter_; //�p�����[�^
    XMFLOAT3 move_;             //�ړ��̑��x�ƕ���

public:
    BulletBase(GameObject* parent, std::string name);
    virtual ~BulletBase() = default;

    virtual void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) {};

    // �e�e�̃p�����[�^���擾
    BulletParameter GetBulletParameter() const { return parameter_; }

    // �e�e�̈ړ���ݒ�
    void SetMove(const XMFLOAT3& move) { move_ = move; }

    void SetPlayerId(int id) { playerId_ = id; }

};