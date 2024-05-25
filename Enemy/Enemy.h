#pragma once
#include "../Engine/GameObject.h"

class Enemy : public GameObject
{
    int hModel_;

    float MoveSpeed = 0.0f;
    float gra = 0.0f;
    bool isGround = false;

public:
    Enemy(GameObject* parent);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;

};