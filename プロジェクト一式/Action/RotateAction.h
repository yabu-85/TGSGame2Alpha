#pragma once
#include "BaseAction.h"

class RotateAction : public BaseAction
{
    float rotateRatio_;         //‰ñ“]—¦
public:
    RotateAction(GameObject* obj);
    virtual ~RotateAction() override {};
    virtual void Update() override;

    //‰ñ“]—¦ƒZƒbƒg
    void SetRatio(float ratio) { rotateRatio_ = ratio; }
};