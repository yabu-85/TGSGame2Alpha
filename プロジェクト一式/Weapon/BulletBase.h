#pragma once
#include "../Engine/GameObject.h"

class Character;

//�e�e�̊��N���X
class BulletBase : public GameObject
{
protected:
    //�o���b�g�̋��ʃp�����[�^
    struct BulletParameter
    {
        int damage_;            //�_���[�W
        int killTimer_;         //��������
        int shotCoolTime_;      //�N�[���^�C��
        float speed_;           //�X�s�[�h
        float collisionScale_;  //�����蔻��̃T�C�Y
    } parameter_;               //�e�ۃp�����[�^

    int playerId_;              //�_���[�W�`��̂��߂�
    XMFLOAT3 move_;             //�ړ��̑��x�ƕ���
    XMFLOAT3 drawPos_;          //�\���p�̍��W
    XMFLOAT3 drawMove_;         //�\���p�̈ړ��x�N�g��

public:
    BulletBase(GameObject* parent, std::string name);
    virtual ~BulletBase() = default;

    virtual void Shot(Character* chara, XMFLOAT3 wallHitPos, XMFLOAT3 charaHitPos) {};

    //�e�e�̃p�����[�^���擾
    BulletParameter GetBulletParameter() { return parameter_; }
    void LoadBulletParameter(std::string name);

    void SetPlayerId(int id) { playerId_ = id; }
    void SetMove(XMFLOAT3 move) { move_ = move; }
    void SetDrawMove(XMFLOAT3 move) { drawMove_ = move; }
    void SetDrawPosition(XMFLOAT3 pos) { drawPos_ = pos; }

};