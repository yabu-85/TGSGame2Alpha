#pragma once
#include "../Character/Character.h"

class HealthGauge;
class DamageSystem;
enum ENEMY_TYPE;

class EnemyBase : public Character
{
protected:
    ENEMY_TYPE enemyType_;
    int hModel_;

    float gravity_ = 0.0f;
    bool isGround = false;
    float damageTime = 0.0f;

    HealthGauge* pHealthGauge_;
    DamageSystem* pDamageSystem_;

public:
    EnemyBase(GameObject* parent, std::string name);
    virtual ~EnemyBase() override;
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void Release() override;

    void SetEnemyType(ENEMY_TYPE type) { enemyType_ = type; }
    void SetDamageTime(float t) { damageTime = t; }
    DamageSystem* GetDamageSystem() { return pDamageSystem_; }

    virtual XMFLOAT3 GetDamageUIPos() = 0;

};