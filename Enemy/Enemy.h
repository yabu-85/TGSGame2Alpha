#pragma once
#include <vector>
#include "../Engine/GameObject.h"
#include "../AI/RouteSearch.h"

class Enemy : public GameObject
{
    int hModel_;

    float MoveSpeed = 0.0f;
    float gravity_ = 0.0f;
    bool isGround = false;
    bool moveReady = false;
    float moveRange_ = 0.3f;
    float outRange = 3.0f;
    std::vector<RouteData> targetList_;

public:
    Enemy(GameObject* parent);
    ~Enemy();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    void Move();
    void CalcDodge(XMVECTOR& move);
    void ReflectCharacter();

};