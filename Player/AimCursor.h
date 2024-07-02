#pragma once
#include "../Engine/GameObject.h"

class AimCursor : public GameObject
{
    int hPict_;

public:
    AimCursor(GameObject* parent);
    ~AimCursor();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

};