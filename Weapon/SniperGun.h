#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class SniperGun : public GunBase
{
public:
    SniperGun(GameObject* parent);
    ~SniperGun();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
