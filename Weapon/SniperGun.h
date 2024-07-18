#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//�e���Ǘ�����N���X
class SniperGun : public GunBase
{
    int hPict_;

    void PressedShot() override;
    void PressedReload() override;
public:
    SniperGun(GameObject* parent);
    ~SniperGun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
