#pragma once
#include "../Character/Character.h"

/*
�e�X�g�p�̓G
f1/f2�ŏo�������邱�Ƃ��ł���
*/

enum ENEMY_TYPE;

class EnemyBase : public Character
{
protected:
    ENEMY_TYPE enemyType_;
    int hModel_;

public:
    EnemyBase(GameObject* parent, std::string name);
    virtual ~EnemyBase() override;
    virtual void Initialize() override;
    virtual void Update() override;
    virtual void Draw() override;
    virtual void Release() override;

    void SetEnemyType(ENEMY_TYPE type) { enemyType_ = type; }

};