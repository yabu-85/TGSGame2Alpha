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

    //向いてる方向計算
    float charaRotateY = pCharacter_->GetRotate().y - 180.0f;
    XMFLOAT3 charaForward = XMFLOAT3();
    charaForward.x = (float)sin(XMConvertToRadians(charaRotateY));
    charaForward.z = (float)cos(XMConvertToRadians(charaRotateY));

    //ターゲットへのベクトル
    XMFLOAT3 toTarget = XMFLOAT3(charaPos.x - targetPos.x, 0.0f, charaPos.z - targetPos.z);
    XMVECTOR toTargetNorm = XMVector3Normalize(XMLoadFloat3(&toTarget));

    //視野角を計算
    float dotProduct = XMVectorGetX(XMVector3Dot(toTargetNorm, XMLoadFloat3(&charaForward)));
    float angle = acosf(dotProduct);

    //ターゲットが範囲外なら終了
    if (angle > fovRadian_) return;

    //CollisionMapと視線の判定をしていく
    RayCastData data;
    data.start = charaPos;
    XMStoreFloat3(&data.dir, (toTargetNorm * -1.0f));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(data, targetPos);

    isFoundTarget_ = false;
    if (data.hit) {
        //rayDistがtoTargetより値が大きいならTargetを目視出来た
        float leng = XMVectorGetX(XMVector3Length(XMLoadFloat3(&toTarget)));
        if (data.dist > leng) {
            isFoundTarget_ = true;
        }
    }
    else {
        //どこにも当たらなかったから見えてる
        isFoundTarget_ = true;
    }
}