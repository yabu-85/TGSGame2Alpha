#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class Gun : public GunBase
{
public:
    Gun(GameObject* parent);
    ~Gun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
