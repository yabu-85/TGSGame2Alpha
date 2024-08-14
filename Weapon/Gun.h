#pragma once
#include "GunBase.h"

class Player;
class AimCursor;

//銃を管理するクラス
class Gun : public GunBase
{
    int peekRootBoneIndex_;
    int peekRootPartIndex_;

    void PressedShot() override;
    bool PressedReload() override;
public:
    Gun(GameObject* parent);
    ~Gun() override;
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};
