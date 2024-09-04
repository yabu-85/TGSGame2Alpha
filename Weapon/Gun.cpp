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
#include "../Engine/EffekseeLib/EffekseerVFX.h"

#include "../Engine/Input.h"
#include "../Scene/PlayScene.h"

namespace {
    static const int PEEK_TIME = 15;                            //�`�����݂ɂ����鎞��

}

Gun::Gun(GameObject* parent)
    : GunBase(parent, "Gun"), animTime_(0)
{
}

Gun::~Gun()
{
}

void Gun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    transform_.pParent_ = nullptr;
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("Gun");
}

void Gun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

    //�`�����݂�Input�֌W��Up��Down�g�킸�ɍ�����ق�����������
    //Pause����߂����ɃL�����Z������Ȃ�

    //�A�j���[�V�����Z�b�g
    if (currentReloadTime_ <= 0) {
        if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
            //�`�����݂ւ̐���
            if (animTime_ <= 0) {
                Model::SetAnimFrame(hPlayerModel_, 120, 140, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 120, 140, 1.0f);
                animTime_ = 0;
            }

            //�`�����ݒ�
            if (animTime_ == 20) {
                Model::SetAnimFrame(hPlayerModel_, 140, 260, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 140, 260, 1.0f);
            }

            animTime_++;
        }
        else {
            //�`�����ݏI���̐���
            if (InputManager::IsCmdUp(InputManager::AIM, playerId_)) {
                Model::SetAnimFrame(hPlayerModel_, 260, 280, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 260, 280, 1.0f);
                animTime_ = 0;
            }

            //�ʏ���
            if (animTime_ == -20) {
                Model::SetAnimFrame(hPlayerModel_, 0, 120, 1.0f);
                Model::SetAnimFrame(hPlayerFPSModel_, 0, 120, 1.0f);
            }

            animTime_--;
        }
    }

    //�����[�h���̏���
    if (currentReloadTime_ > 0) {
        Reload();

        animTime_--;
        //�`�����ݒ�
        if (animTime_ == 0) {
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 0, 1.0f);
        }

        //�����[�h�I��
        if (currentMagazineCount_ == magazineCount_) {
            Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 0, 1.0f);
            animTime_ = 0;
        }

        return;
    }

    //��������̎��������[�h
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        PressedReload();

        //�A�j���[�V����
        if (isPeeking_) {
            //�`�����ݏI��
            Model::SetAnimFrame(hPlayerModel_, 260, 280, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 260, 280, 1.0f);
        }
        else {
            //�ʏ���
            Model::SetAnimFrame(hPlayerModel_, 0, 0, 1.0f);
            Model::SetAnimFrame(hPlayerFPSModel_, 0, 00, 1.0f);
        }
        animTime_ = 20;

        //Peeking
        isPeeking_ = false;
        peekTime_ = PEEK_TIME;

        return;
    }

    //�̂������ݏ���
    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        peekTime_--;

        float zoom = (float)peekTime_ / (float)PEEK_TIME;
        zoom = std::clamp(zoom, 0.7f, 1.0f);
        Camera::SetPeekFOVZoom(zoom, playerId_);

        //�`�����݊���
        if (peekTime_ <= 0) {
            isPeeking_ = true;
        }
    }
    else {
        peekTime_ = PEEK_TIME;
        isPeeking_ = false;
        Camera::SetPeekFOVZoom(1.0f, playerId_);
    }

    //�ʏ�ˌ�
    if (InputManager::IsCmd(InputManager::ATTACK, playerId_))
    {
        PressedShot();
    }

    //RotateShake�߂菈����True��
    if (currentMagazineCount_ <= 0 || coolTime_ <= 0) pPlayer_->GetAim()->SetCameraRotateReturn(true);

}

void Gun::Draw()
{
    //�A�j���[�V����
    if (IsEntered()) Model::AnimStart(hModel_);
    else Model::AnimStop(hModel_);

    //Shado�̏ꍇReturn
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (type == Direct3D::SHADER_SHADOWMAP) return;

    transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
    int dI = GameManager::GetDrawIndex();
    if (pPlayer_->GetPlayerId() == dI) {

        //�s�[�N���͉摜�̕\��
        if (isPeeking_) {
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                Model::SetTransform(hModel_, transform_);
                Model::Draw(hModel_);
            }
        }
        else {
            Model::SetTransform(hModel_, transform_);
            Model::Draw(hModel_);
        }
    }
    //����̕\��
    else {
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }
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

    //�e��Init
    ShotBullet<Bullet_Normal>("Bullet_Normal");
    pAimCursor_->Shot();

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);

    //�q�b�g�G�t�F�N�g
    EFFEKSEERLIB::EFKTransform t;
    Transform transform;
    transform.position_ = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    transform.rotate_ = transform_.rotate_;
    transform.rotate_.x = -transform.rotate_.x;
    transform.rotate_.y += 180.0f;
    transform.scale_ = transform_.scale_;
    DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
    t.isLoop = false;   //�J��Ԃ�
    t.maxFrame = 20;    //80�t���[��
    t.speed = 1.0;      //�X�s�[�h
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t);
}

bool Gun::PressedReload()
{
    //���łɖ��^��
    if (currentMagazineCount_ >= magazineCount_) return false;
    //���łɃ����[�h��
    if (currentReloadTime_ > 0) return false;
    //�N�[���^�C���ł���
    if (coolTime_ > 0) return false;

    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    pPlayer_->GetAim()->SetCameraRotateReturn(true);
    Model::SetAnimFrame(hModel_, 0, 200, (float)(200.0f / reloadTime_ * 0.5f) );
    return true;
}
