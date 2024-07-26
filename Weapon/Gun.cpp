#include "Gun.h"
#include "Bullet_Normal.h"
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

#include "../Engine/Input.h"

namespace {
    XMFLOAT3 handOffset = { 0.15f, 0.7f, 0.05f };      // �ړ���

}

Gun::Gun(GameObject* parent)
    : GunBase(parent, "Gun")
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle2.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    Model::GetPartBoneIndex(hModel_, "PeakRoot", &peekRootPartIndex_, &peekRootBoneIndex_);
    assert(peekRootPartIndex_ >= 0);

    transform_.pParent_ = nullptr;
    pAimCursor_ = new AimCursor();
    TestScene* scene = static_cast<TestScene*>(FindObject("TestScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("Gun");
    isFirstPerson_ = false; 
}

void Gun::Update()
{
    //�N�[���^�C�������炷
    coolTime_--;
    pAimCursor_->Update();

    static float rotateY = 0.0f;
    if(Input::IsKey(DIK_V)) rotateY += 1.0f;
    if (Input::IsKey(DIK_B)) rotateY -= 1.0f;

    int p, b, handle;
    handle = pPlayer_->GetModelHandle();
    Model::GetPartBoneIndex(handle, "Weapon", &p, &b);
    transform_.position_ = Model::GetBoneAnimPosition(handle, p, b);
    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        Model::SetAnimFrame(handle, 150, 150, 1.0f);
    }
    else {
        Model::SetAnimFrame(handle, 0, 120, 1.0f);
    }


    //�����[�h��
    if (currentReloadTime_ >= 1) {
        Reload();

        //�I���
        if (currentReloadTime_ <= 0) {
            Model::SetAnimFrame(hModel_, 0, 0, 0.0f);
        }
        return;
    }

    //�����[�h
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        PressedReload();
    }

    //�ʏ�ˌ�
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_)) {
        PressedShot();
    }

    //RotateShake�߂菈����True��
    if (currentMagazineCount_ <= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

}

void Gun::Draw()
{
    Model::SetTransform(hModel_, transform_);
    Model::Draw(hModel_);
}

void Gun::Release()
{
}

void Gun::PressedShot()
{
    //�܂��N�[���^�C����
    if (coolTime_ > 0) return;

    //�}�K�W���v�Z
    if (currentMagazineCount_ <= 0) return;
    currentMagazineCount_--;

    ShotBullet<Bullet_Normal>();
    pAimCursor_->Shot();

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);

    //�q�b�g�G�t�F�N�g
    EFFEKSEERLIB::EFKTransform t;
    Transform transform;
    transform.position_ = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
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

void Gun::PressedReload()
{
    //���łɖ��^��
    if (currentMagazineCount_ >= magazineCount_) return;
    //���łɃ����[�h��
    if (currentReloadTime_ > 0) return;
    //�N�[���^�C���ł���
    if (coolTime_ > 0) return;

    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    pPlayer_->GetAim()->SetCameraRotateReturn(true);
    Model::SetAnimFrame(hModel_, 0, 200, (float)(200.0f / reloadTime_ * 0.5f) );
}
