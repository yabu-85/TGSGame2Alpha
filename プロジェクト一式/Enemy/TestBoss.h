#pragma once
#include "EnemyBase.h"

class CapsuleCollider;
class AnimationController;

enum class TESTBOSS_ANIMATION {
    ANIM1 = 0,
    ANIM2,
    MAX,
};

const int TESTBOSS_ANIMATION_DATA[(int)TESTBOSS_ANIMATION::MAX][2]{
    { 0, 240 },
    { 240, 360 },
};

struct BoneAttachColliderData
{
    float radius, height;
    std::string boneName;
    XMFLOAT3 position, rotation;
    BoneAttachColliderData() : radius(0.0f), height(0.0f), position(XMFLOAT3()), rotation(XMFLOAT3()) {}
    BoneAttachColliderData(float r, float h, std::string name, XMFLOAT3 pos, XMFLOAT3 rot)
        : radius(r), height(h), boneName(name), position(pos), rotation(rot) {
    }
};

class TestBoss : public EnemyBase
{
    AnimationController* pAnimationController_;

    void LoadBoneAttachData(const std::string& filename);

public:
    TestBoss(GameObject* parent);
    ~TestBoss();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;
    void CalcDraw() override;

};