#pragma once
#include "../Engine/GameObject.h"

//タイマー管理するクラス
class BulletInfoDisplay : public GameObject
{
    enum Bullet_INFO_IMAGE {
        CENTER = 0,     //銃弾の中心に表示する何分の何の画像
        MAX,    
    };

    int hPict_[10 + MAX];   //画像番号
    int currentMagazine_;   //マガジン容量
    int maxMagazine_;       //最大マガジン容量

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