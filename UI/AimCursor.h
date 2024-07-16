#pragma once

//‰æ–Ê‚É•\¦‚³‚ê‚éAimCursor‚ÌƒNƒ‰ƒX
//Draw‚ÍPlayScene‚©‚çŒÄ‚Î‚ê‚é
class AimCursor
{
    int hPict_;
    int hHitPict_;

    float shotDurace_;  //–ß‚éŠÔ
    float shotParce_;   //0.0f ` 1.0f‚ÌŠÔŠ„‡
    float hitDurace_;   //–ß‚éŠÔ
    float hitParce_;    //0.0f ` 1.0f

public:
    AimCursor();
    ~AimCursor();
    void Update();
    void Draw();
    void Shot();
    void Hit();

    float GetBurePower();
};