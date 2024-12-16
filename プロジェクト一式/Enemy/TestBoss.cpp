#include "TestBoss.h"
#include "EnemyManager.h"
#include "../Engine/Model.h"
#include "../Engine/CapsuleCollider.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Global.h"
#include "../Engine/Input.h"
#include "../Character/DamageSystem.h"
#include "../Animation/AnimationController.h"
#include "../Animation/TestBossNotify.h"
#include "../UI/FixedHealthGauge.h"
#include "../Other/GameManager.h"
#include "../Other/VFXManager.h"

#include "../Json/JsonReader.h"
#include "../Engine/ImGui/imgui.h"
#include "../Engine/ImGui/imgui_impl_dx11.h"
#include "../Engine/ImGui/imgui_impl_win32.h"
#include <fstream>
#include <cstring> 
#include <string>

namespace {
    const XMFLOAT3 START_POS = XMFLOAT3(50.0f, 5.0f, 50.0f);
    
    const int BoneAttackSize = 14;
    BoneAttachColliderData BoneAttachData[BoneAttackSize] = {
        { 0.85f, 0.85f, "Head", XMFLOAT3(0.0f, 0.708f, 0.04f), XMFLOAT3(98.7f, 0.0f, 0.0f)},      //頭
        { 0.7f, 1.3f, "Neck", XMFLOAT3(0.0f, 0.22f, 0.0f), XMFLOAT3(84.4f, 0.0f, 0.0f)},              //首
        { 1.2f, 0.8f, "BodyUp", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},    //胴体上
        { 0.8f, 1.1f, "BodyCenter", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},          //胴体下

        { 0.1f, 0.1f, "TailUp", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},    //尻尾上
        { 0.1f, 0.1f, "TailDown", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},    //尻尾下

        { 0.1f, 0.1f, "Thighs.L", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//左足上
        { 0.1f, 0.1f, "Calf.L", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//左足下
        { 0.1f, 0.1f, "Feet.L", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},    //左足 
        
        { 0.1f, 0.1f, "Thighs.R", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//右足上
        { 0.1f, 0.1f, "Calf.R", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//右足下
        { 0.1f, 0.1f, "Feet.R", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},    //右足

        { 0.1f, 0.1f, "Hand.L", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//左手
        { 0.1f, 0.1f, "Hand.R", XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, 0.0f)},//右手
    };

}

TestBoss::TestBoss(GameObject* parent)
    : EnemyBase(parent, "TestBossEnemy"), pAnimationController_(nullptr)
{
}

TestBoss::~TestBoss() 
{
}

void TestBoss::Initialize()
{
    hModel_ = Model::Load("Model/TREX.fbx");
    assert(hModel_ >= 0);

    transform_.position_ = START_POS;
    enemyType_ = ENEMY_TYPE::ENEMY_TEST;

    SetBodyRange(0.5f);
    SetBodyWeight(0.3f);
    SetBodyHeightHalf(0.8f);
    
    SetMaxHP(1000);
    SetHP(1000);

    pHealthGauge_ = new FixedHealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.2f, 0.8f));

    LoadBoneAttachData("Json/TestBossAttachCollider");

    for (int i = 0; i < BoneAttackSize; i++) {
        CapsuleCollider* pCapsuleCollider = new CapsuleCollider(XMFLOAT3(), BoneAttachData[i].radius, BoneAttachData[i].height, XMVECTOR());
        AddCollider(pCapsuleCollider);
        Model::AddAttachColliderToBone(hModel_, pCapsuleCollider, BoneAttachData[i].boneName, BoneAttachData[i].position, BoneAttachData[i].rotation);
    }

    //アニメーションデータのセットフレームはヘッダに書いてる
    pAnimationController_ = new AnimationController(hModel_, this);
    for (int i = 0; i < (int)TESTBOSS_ANIMATION::MAX; i++) pAnimationController_->AddAnim(TESTBOSS_ANIMATION_DATA[i][0], TESTBOSS_ANIMATION_DATA[i][1]);
    //1
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(60, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(120, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(180, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(240, VFX_TYPE::Explode));
    pAnimationController_->SetNextAnim(0, 0.3f);
    Model::AnimStop(hModel_);

}

void TestBoss::Update()
{
    //Dead判定
    if (IsHealthZero()) {
        KillMe();
        return;
    }

    if (!Input::IsKey(DIK_F)) {
        float speed = 0.3f;
        if (Input::IsKey(DIK_NUMPAD1)) transform_.position_.x -= speed;
        if (Input::IsKey(DIK_NUMPAD2)) transform_.position_.x += speed;
        if (Input::IsKey(DIK_NUMPAD4)) transform_.position_.y -= speed;
        if (Input::IsKey(DIK_NUMPAD5)) transform_.position_.y += speed;
        if (Input::IsKey(DIK_NUMPAD7)) transform_.position_.z -= speed;
        if (Input::IsKey(DIK_NUMPAD8)) transform_.position_.z += speed;
        if (Input::IsKey(DIK_NUMPAD0)) transform_.position_ = XMFLOAT3();
    }
    else {
        float speed = 2.0f;
        if (Input::IsKey(DIK_NUMPAD1)) transform_.rotate_.x -= speed;
        if (Input::IsKey(DIK_NUMPAD2)) transform_.rotate_.x += speed;
        if (Input::IsKey(DIK_NUMPAD4)) transform_.rotate_.y -= speed;
        if (Input::IsKey(DIK_NUMPAD5)) transform_.rotate_.y += speed;
        if (Input::IsKey(DIK_NUMPAD7)) transform_.rotate_.z -= speed;
        if (Input::IsKey(DIK_NUMPAD8)) transform_.rotate_.z += speed;
        if (Input::IsKey(DIK_NUMPAD0)) transform_.rotate_ = XMFLOAT3();
    }
    if (Input::IsKeyDown(DIK_U)) Model::AnimStop(hModel_);
    if (Input::IsKeyDown(DIK_I)) Model::AnimStart(hModel_);

    pAnimationController_->Update();
    Model::SetTransform(hModel_, transform_);
    Model::Update(hModel_);
}

void TestBoss::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)GetHP() / (float)GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw(GameManager::GetDrawIndex());
    }

#if _DEBUG
    Direct3D::SetDepthBafferWriteEnable(false);
    CollisionDraw();
    Direct3D::SetDepthBafferWriteEnable(true);
#else
    CollisionDraw();
#endif // _DEBUG

}

void TestBoss::Release()
{
    Model::Release(hModel_);

}

void TestBoss::CalcDraw()
{
    Model::CalcDraw(hModel_);
}

void TestBoss::LoadBoneAttachData(const std::string& filename)
{
    JsonReader::Load(filename);
    const auto& j = JsonReader::GetSection("boneAttachData");

    for (size_t i = 0; i < j.size(); ++i)
    {
        const auto& boneData = j[i];
        BoneAttachData[i].radius = boneData["radius"];
        BoneAttachData[i].height = boneData["height"];
        BoneAttachData[i].boneName = boneData["boneName"];
        BoneAttachData[i].position = DirectX::XMFLOAT3(
            boneData["position"][0], boneData["position"][1], boneData["position"][2]
        );
        BoneAttachData[i].rotation = DirectX::XMFLOAT3(
            boneData["rotation"][0], boneData["rotation"][1], boneData["rotation"][2]
        );
    }
}