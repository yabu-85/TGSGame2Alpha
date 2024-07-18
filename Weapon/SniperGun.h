#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class SniperGun : public GunBase
{
    int hPict_;
public:
    SniperGun(GameObject* parent);
    ~SniperGun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
