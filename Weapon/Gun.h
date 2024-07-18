#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//e‚ğŠÇ—‚·‚éƒNƒ‰ƒX
class Gun : public GunBase
{
    void PressedShot() override;
    void PressedReload() override;
public:
    Gun(GameObject* parent);
    ~Gun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
