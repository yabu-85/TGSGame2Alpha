#include "Gun.h"
#include "Bullet_Normal.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Animation/AnimationController.h"
#include "../State/StateManager.h"
#include "../Other/InputManager.h"
#include "../Other/AudioManager.h"
#include "../Other/GameManager.h"
#include "../Scene/PlayScene.h"
#include "../UI/AimCursor.h"

Gun::Gun(GameObject* parent) : GunBase(parent, "Gun")
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

    //�����[�h���̏���
    if (currentReloadTime_ > 0) {
        Reload();

        //�����[�h�I��
        if (currentMagazineCount_ == magazineCount_) {
            Model::SetAnimFrame(hModel_, 0, 0, 1.0f);
            
            //�v���C���[��State��Idle��Jump��������
            std::string stateName = pPlayer_->GetStateManager()->GetName();
            if (stateName == "Idle") {
                pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
                pPlayer_->GetAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
            }
        }
        return;
    }

    //�����[�h�{�^��������
    if (InputManager::IsCmdDown(InputManager::RELOAD, playerId_)) {
        //����������
        if (PressedReload()) {
            //Peeking
            isPeeking_ = false;
            currentPeekTime_ = peekTime_;
        
            //PlayerState
            pPlayer_->GetStateManager()->ChangeState("Reload");

            //�I���
            return;
        }
    }

    //�e�e�󂾂��ǌ��{�^���������A�����[�h
    if (currentMagazineCount_ <= 0 && InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        //�����[�h�ł���Ȃ�I���
        if (PressedReload()) return;
    }

    //�̂������ݏ���
    Peeking();

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
    if (pPlayer_->GetAim()->IsAimFps()) ShotFpsBullet<Bullet_Normal>("Bullet_Normal");
    else ShotBullet<Bullet_Normal>("Bullet_Normal");

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 0.3f), 1);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(false);
    pAimCursor_->Shot();

    AudioManager::Play(AUDIO_TYPE::SMALL_SHOT, 0.5f);

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
    //Player�̏�Ԃ��ǂ���
    if (pPlayer_->IsClimb()) return false;

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
