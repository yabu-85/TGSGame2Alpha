#pragma once
#include "BaseAction.h"

class RotateAction : public BaseAction
{
    float rotateRatio_;         //��]��
public:
    RotateAction(GameObject* obj);
    virtual ~RotateAction() override {};
    virtual void Update() override;

    //��]���Z�b�g
    void SetRatio(float ratio) { rotateRatio_ = ratio; }
};