#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//e‚ğŠÇ—‚·‚éƒNƒ‰ƒX
class SniperGun : public GunBase
{
    int hPict_;
    int animTime_;

    void PressedShot() override;
    bool PressedReload() override;
public:
    SniperGun(GameObject* parent);
    ~SniperGun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
