#pragma once

//画面に表示されるAimCursorのクラス
//DrawはPlaySceneから呼ばれる
class AimCursor
{
    int hPict_;
    int hHitPict_;

    float shotDurace_;  //戻る時間
    float shotParce_;   //0.0f 〜 1.0fの時間割合
    float hitDurace_;   //戻る時間
    float hitParce_;    //0.0f 〜 1.0f

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();
    void Shot();
    void Hit();

    float GetBurePower();
};