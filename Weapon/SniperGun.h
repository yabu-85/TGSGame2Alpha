#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class SniperGun : public GunBase
{
    int hPict_;

    void PressedShot() override;
    bool PressedReload() override;
    void ResetReload() override;

public:
    SniperGun(GameObject* parent);
    ~SniperGun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
