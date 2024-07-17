#include "SniperGun.h"
#include "BulletBase.h"
#include "Bullet_Normal.h"
#include "GunBase.h"

#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/InputManager.h"
#include "../Other/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"
#include "../Scene/TestScene.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

namespace {
    static const XMFLOAT3 handOffset = { 0.2f, 0.7f, 0.1f };    //�ړ���
    static const int PEEK_TIME = 30;                            //�`�����݂ɂ����鎞��
        
}

SniperGun::SniperGun(GameObject* parent)
    : GunBase(parent, "SniperGun")
{
}

SniperGun::~SniperGun()
{
}

void SniperGun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topBoneIndex_, &topPartIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootBoneIndex_, &rootPartIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    isFirstPerson_ = true;

    //�v���C���[�̎�̈ʒu�܂Œ���
    transform_.position_ = handOffset;

}

void SniperGun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    //FPS��TPS���Ŕ���
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        peekTime_--;
        //�`�����݊���
        if (peekTime_ <= 0) {
            Invisible();
        }
    }
    else {
        peekTime_ = PEEK_TIME;
        Visible();
    }

    // �ʏ�ˌ�
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_))
    {
        //�܂��N�[���^�C����
        if (coolTime_ > 0) return;

        ShootBullet<Bullet_Normal>();
        pAimCursor_->Shot();

        CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
        pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
        pPlayer_->GetAim()->SetCameraRotateReturn(false);

        //�q�b�g�G�t�F�N�g
        EFFEKSEERLIB::EFKTransform t;
        Transform transform;
        transform.position_ = Model::GetBonePosition(hModel_, topBoneIndex_, topPartIndex_);
        transform.rotate_ = Float3Add(pPlayer_->GetRotate(), transform_.rotate_);
        transform.rotate_.y += +180.0f;
        transform.rotate_.x = -transform.rotate_.x;
        transform.scale_ = transform_.scale_;
        DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
        t.isLoop = false;   //�J��Ԃ�
        t.maxFrame = 20;    //80�t���[��
        t.speed = 1.0;      //�X�s�[�h
        EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
    }
    else {
        //�A���őł̂�߂��������ARotateShake�߂菈����True��
        if(coolTime_ >= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

    }

}

void SniperGun::Draw()
{
    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void SniperGun::Release()
{
}