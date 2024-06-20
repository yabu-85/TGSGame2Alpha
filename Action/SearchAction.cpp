#include "SearchAction.h"
#include "../Character/Character.h"
#include "../GameManager/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../Engine/Global.h"

VisionSearchAction::VisionSearchAction(Character* obj, float range, float fov) : BaseAction(obj), range_(range), isFoundTarget_(false)
{
    pTarget_ = GameManager::GetPlayer();
    fovRadian_ = XMConvertToRadians(fov) / 2;
}

void VisionSearchAction::Update()
{
    XMFLOAT3 charaPos = pCharacter_->GetPosition();
    XMFLOAT3 targetPos = pTarget_->GetPosition();

    //�����Ă�����v�Z
    float charaRotateY = pCharacter_->GetRotate().y - 180.0f;
    XMFLOAT3 charaForward = XMFLOAT3();
    charaForward.x = (float)sin(XMConvertToRadians(charaRotateY));
    charaForward.z = (float)cos(XMConvertToRadians(charaRotateY));

    //�^�[�Q�b�g�ւ̃x�N�g��
    XMFLOAT3 toTarget = XMFLOAT3(charaPos.x - targetPos.x, 0.0f, charaPos.z - targetPos.z);
    XMVECTOR toTargetNorm = XMVector3Normalize(XMLoadFloat3(&toTarget));

    //����p���v�Z
    float dotProduct = XMVectorGetX(XMVector3Dot(toTargetNorm, XMLoadFloat3(&charaForward)));
    float angle = acosf(dotProduct);

    //�^�[�Q�b�g���͈͊O�Ȃ�I��
    if (angle > fovRadian_) return;

    //CollisionMap�Ǝ����̔�������Ă���
    RayCastData data;
    data.start = charaPos;
    XMStoreFloat3(&data.dir, (toTargetNorm * -1.0f));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(data, targetPos);

    isFoundTarget_ = false;
    if (data.hit) {
        //rayDist��toTarget���l���傫���Ȃ�Target��ڎ��o����
        float leng = XMVectorGetX(XMVector3Length(XMLoadFloat3(&toTarget)));
        if (data.dist > leng) {
            isFoundTarget_ = true;
        }
    }
    else {
        //�ǂ��ɂ�������Ȃ��������猩���Ă�
        isFoundTarget_ = true;
    }
}