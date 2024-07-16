#pragma once
#include "AnimationNotify.h"

class TestBossAttackNotify : public DoubleFrame {
public:
    TestBossAttackNotify(int first, int end) : DoubleFrame(first, end) {};
    void OnFrame(GameObject* obj) override;
    void OnEnter(GameObject* obj) override;
    void OnExit(GameObject* obj) override;
};

class TestBossRotateNotify : public DoubleFrame {
public:
    TestBossRotateNotify(int first, int end) : DoubleFrame(first, end) {};
    void OnFrame(GameObject* obj) override;
};