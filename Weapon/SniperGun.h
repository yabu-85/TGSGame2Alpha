#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//�e���Ǘ�����N���X
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
