#pragma once
#include <vector>
#include "../Engine/GameObject.h"

class HealthGauge;
class DamageSystem;
enum ENEMY_TYPE;

class EnemyBase : public GameObject
{
protected:
    ENEMY_TYPE enemyType_;
    int hModel_;

    float MoveSpeed = 0.0f;
    float gravity_ = 0.0f;
    bool isGround = false;
    bool moveReady = false;
    float moveRange_ = 0.3f;
    float outRange = 3.0f;

    float damageTime = 0.0f;

    HealthGauge* pHealthGauge_;
    DamageSystem* pDamageSystem_;

public:
    EnemyBase(GameObject* parent);
    ~EnemyBase();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    void Move();
    void CalcDodge(XMVECTOR& move);
    void ReflectCharacter();

    void SetEnemyType(ENEMY_TYPE type) { enemyType_ = type; }
    void SetDamageTime(float t) { damageTime = t; }
    DamageSystem* GetDamageSystem() { return pDamageSystem_; }

    virtual float GetValueA() = 0;

};