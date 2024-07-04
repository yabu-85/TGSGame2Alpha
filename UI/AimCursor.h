#pragma once

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

};