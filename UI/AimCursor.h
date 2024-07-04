#pragma once

//‰æ–Ê‚É•\¦‚³‚ê‚éAimCursor‚ÌƒNƒ‰ƒX
//Draw‚ÍPlayScene‚©‚çŒÄ‚Î‚ê‚é
class AimCursor
{
    int hPict_;

    float shotDurace_;  //–ß‚éŠÔ
    float shotParce_;   //0.0f ` 1.0f‚ÌŠÔŠ„‡

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();

    void Shot();

};