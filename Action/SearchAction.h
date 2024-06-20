#pragma once
#include "BaseAction.h"
#include <DirectXMath.h>

using namespace DirectX;

//���o�̒T��
class VisionSearchAction : public BaseAction
{
    bool isFoundTarget_;    //�^�[�Q�b�g����������
    Character* pTarget_;    //�^�[�Q�b�g�̃|�C���^
    float range_;           //����͈̔�
    float fovRadian_;       //����̔��a

public:
    VisionSearchAction(Character* obj, float range, float fov);
    virtual ~VisionSearchAction() override {};
    virtual void Update() override;
    
    void SetTarget(Character* tar) { pTarget_ = tar; }
    bool IsFoundTarget() { return isFoundTarget_; }

};