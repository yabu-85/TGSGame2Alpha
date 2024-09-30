#pragma once
#include "../Engine/GameObject.h"

//�^�C�}�[�Ǘ�����N���X
class BulletInfoDisplay : public GameObject
{
    enum Bullet_INFO_IMAGE {
        CENTER = 0,     //�e�e�̒��S�ɕ\�����鉽���̉��̉摜
        MAX,    
    };

    int hPict_[10 + MAX];   //�摜�ԍ�
    int currentMagazine_;   //�}�K�W���e��
    int maxMagazine_;       //�ő�}�K�W���e��

public:
    BulletInfoDisplay(GameObject* parent);
    ~BulletInfoDisplay() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    void DrawBullet();
    void SetMaxMagazine(int mag) { maxMagazine_ = mag; }
    void SetCurrentMagazine(int mag) { currentMagazine_ = mag; }

};