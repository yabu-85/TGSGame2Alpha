#pragma once
#include "../Engine/GameObject.h"

//�^�C�}�[�Ǘ�����N���X
class BulletInfoDisplay : public GameObject
{
    //�t���[�����[�g
    const int pictSize = 12;

    int hPict_[10];     //�摜�ԍ�
    float drawX_;       //�`��ʒu(���E)
    float drawY_;       //�`��ʒu(�㉺)

public:
    BulletInfoDisplay(GameObject* parent);
    ~BulletInfoDisplay() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};