#pragma once
#include "../Engine/GameObject.h"

//タイマー管理するクラス
class BulletInfoDisplay : public GameObject
{
    //フレームレート
    const int pictSize = 12;

    int hPict_[10];     //画像番号
    float drawX_;       //描画位置(左右)
    float drawY_;       //描画位置(上下)

public:
    BulletInfoDisplay(GameObject* parent);
    ~BulletInfoDisplay() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};