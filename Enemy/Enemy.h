#pragma once
#include <vector>
#include "../Engine/GameObject.h"

class Enemy : public GameObject
{
    int hModel_;

    float MoveSpeed = 0.0f;
    float gra = 0.0f;
    bool isGround = false;

    int lastTarget = 0;
    float moveRange_ = 0.2f;
    float moveSpeed_ = 0.05f;
    float outRange = 3.0f;
    std::vector<XMFLOAT3> targetList_;

public:
    Enemy(GameObject* parent);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void OnCollision(GameObject* pTarget) override;

    void  Move();
};