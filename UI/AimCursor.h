#pragma once

//画面に表示されるAimCursorのクラス
//DrawはPlaySceneから呼ばれる
class AimCursor
{
    int hPict_;

    float shotDurace_;  //戻る時間
    float shotParce_;   //0.0f 〜 1.0fの時間割合

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();

    void Shot();

};