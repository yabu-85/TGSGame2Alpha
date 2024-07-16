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
#include "../UI/BossHealthGauge.h"
#include "../Other/GameManager.h"
#include "../Other/VFXManager.h"

XMFLOAT3 TestBoss::damageUIPos_ = XMFLOAT3(0.5f, 1.5f, 0.0f);

namespace {
    const XMFLOAT3 start = { 50.0f, 5.0f, 50.0f }; //スタートポジション

}

TestBoss::TestBoss(GameObject* parent)
    : EnemyBase(parent, "TestBossEnemy")
{
}

TestBoss::~TestBoss() 
{
}

void TestBoss::Initialize()
{
    hModel_ = Model::Load("Model/TREX.fbx");
    assert(hModel_ >= 0);

    transform_.position_ = start;
    enemyType_ = ENEMY_TYPE::ENEMY_TEST;

    SetBodyRange(0.5f);
    SetBodyWeight(0.3f);
    SetBodyHeightHalf(0.8f);
    pHealthGauge_ = new BossHealthGauge(this);
    pHealthGauge_->SetOffSetPosition(XMFLOAT2(0.2f, 0.8f));
    pDamageSystem_->SetMaxHP(1000);
    pDamageSystem_->SetHP(1000);

    XMVECTOR vec = { 0.0f, 0.0f, 0.0f, 0.0f };
    XMFLOAT3 center = XMFLOAT3();
    pCapsuleCollider_[0] = new CapsuleCollider(center, 0.9f, 0.5f, vec);    //頭
    pCapsuleCollider_[1] = new CapsuleCollider(center, 0.7f, 1.3f, vec);    //首
    pCapsuleCollider_[2] = new CapsuleCollider(center, 1.2f, 0.8f, vec);    //胴体上
    pCapsuleCollider_[3] = new CapsuleCollider(center, 0.8f, 1.1f, vec);    //胴体下
    pCapsuleCollider_[4] = new CapsuleCollider(center, 0.5f, 1.1f, vec);    //尻尾上
    pCapsuleCollider_[5] = new CapsuleCollider(center, 0.3f, 0.8f, vec);    //尻尾下
    pCapsuleCollider_[6] = new CapsuleCollider(center, 0.6f, 0.9f, vec);    //左足上
    pCapsuleCollider_[7] = new CapsuleCollider(center, 0.4f, 0.9f, vec);    //左足下
    pCapsuleCollider_[8] = new CapsuleCollider(center, 0.4f, 1.0f, vec);    //左足
    pCapsuleCollider_[9] = new CapsuleCollider(center, 0.6f, 0.9f, vec);    //右足上
    pCapsuleCollider_[10] = new CapsuleCollider(center, 0.4f, 0.9f, vec);   //右足下
    pCapsuleCollider_[11] = new CapsuleCollider(center, 0.4f, 1.0f, vec);   //左足
    pCapsuleCollider_[12] = new CapsuleCollider(center, 0.5f, 1.0f, vec);   //左手
    pCapsuleCollider_[13] = new CapsuleCollider(center, 0.5f, 1.0f, vec);   //右手
    for (int i = 0; i < MAX_INDEX; i++) if (pCapsuleCollider_[i]) AddCollider(pCapsuleCollider_[i]);

    std::string boneName[MAX_INDEX * 2] = {
        "Head.001", "Head.002", 
        "Bone.003.R", "Bone.004.R",
        "Bone", "Bone.001.R",
        "Bone.009.L", "Bone.008.L", 
        "Tail.011.L", "Bone.010.L", 
        "Tail.011.L", "Tail.011.L.001", 
        "Bone.007.L.002", "Bone.007.L.001",
        "Bone.007.L.002", "Bone.007.L.004",
        "Feet.L.004", "Feet.L.005",
        "Bone.007.R.002", "Bone.007.R.001",
        "Bone.007.R.002", "Bone.007.R.004",
        "Feet.R.004", "Feet.R.005",
        "Bone.001.L.002", "Bone.001.L.001", 
        "Bone.001.R.002", "Bone.001.R.001", };
    for (int i = 0; i < MAX_INDEX * 2; i++) Model::GetPartBoneIndex(hModel_, boneName[i], &partIndex_[i], &boneIndex_[i]);

    //アニメーションデータのセットフレームはヘッダに書いてる
    pAnimationController_ = new AnimationController(hModel_, this);
    for (int i = 0; i < (int)TESTBOSS_ANIMATION::MAX; i++) pAnimationController_->AddAnim(TESTBOSS_ANIMATION_DATA[i][0], TESTBOSS_ANIMATION_DATA[i][1]);
    //1
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(60, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(120, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(180, VFX_TYPE::Explode));
    pAnimationController_->AddAnimNotify((int)TESTBOSS_ANIMATION::ANIM1, new CreatFrame(240, VFX_TYPE::Explode));
    
    pAnimationController_->SetNextAnim(0, 0.3f);

}

void TestBoss::Update()
{
    pAnimationController_->Update();
    //pStateManager_->Update();

    if (Input::IsKey(DIK_G)) transform_.rotate_.y += 10;
    if (Input::IsKey(DIK_H)) transform_.rotate_.y -= 10;

    //Center計算
    for (int i = 0; i < MAX_INDEX; i++) {
        int ni = (i * 2);

        //Center
        pCapsuleCollider_[i]->center_ = Float3Sub(Model::GetBoneAnimPosition(hModel_, partIndex_[ni + 1], boneIndex_[ni + 1]), transform_.position_);
   
        //Direction
        XMFLOAT3 fff = XMFLOAT3();
        fff = Float3Sub(Model::GetBoneAnimPosition(hModel_, partIndex_[ni], boneIndex_[ni]), Model::GetBoneAnimPosition(hModel_, partIndex_[ni + 1], boneIndex_[ni + 1]));
        pCapsuleCollider_[i]->direction_ = XMVector3Normalize(XMLoadFloat3(&fff));
    }

}

void TestBoss::Draw()
{
    Direct3D::EnemyPosition = transform_.position_;

    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);

    if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
        float r = (float)pDamageSystem_->GetHP() / (float)pDamageSystem_->GetMaxHP();
        pHealthGauge_->SetParcent(r);
        pHealthGauge_->Draw(GameManager::GetDrawIndex());
    }

    //Direct3D::SetDepthBafferWriteEnable(false);
    //CollisionDraw();
    //Direct3D::SetDepthBafferWriteEnable(true);

}

void TestBoss::Release()
{
    Model::Release(hModel_);

}
