#include "SniperGun.h"
#include "Bullet_Sniper.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Image.h"
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
#include "../Scene/PlayScene.h"
#include "../Engine/EffekseeLib/EffekseerVFX.h"

//�X�i�C�p�[�摜URL
//https://www.ac-illust.com/main/detail.php?id=23464746&word=%E7%85%A7%E6%BA%96%E3%83%9E%E3%83%BC%E3%82%AF%E9%BB%92

namespace {
    static const int PEEK_TIME = 15;                            //�`�����݂ɂ����鎞��
        
}

SniperGun::SniperGun(GameObject* parent)
    : GunBase(parent, "SniperGun"), hPict_(-1), animTime_(0)
{
}

SniperGun::~SniperGun()
{
}

void SniperGun::Initialize()
{
    hModel_ = Model::Load("Model/Rifle2.fbx");
    assert(hModel_ >= 0);

    Model::GetPartBoneIndex(hModel_, "Top", &topPartIndex_, &topBoneIndex_);
    Model::GetPartBoneIndex(hModel_, "Root", &rootPartIndex_, &rootBoneIndex_);
    assert(topBoneIndex_ >= 0);
    assert(rootBoneIndex_ >= 0);

    hPict_ = Image::Load("Image/SniperPeak.png");
    assert(hPict_ >= 0);

    transform_.pParent_ = nullptr;
    pAimCursor_ = new AimCursor();
    PlayScene* scene = static_cast<PlayScene*>(FindObject("PlayScene"));
    if (scene) scene->SetAimCursor(playerId_, pAimCursor_);

    LoadGunJson("SniperGun");
    isFirstPerson_ = true;
}

void SniperGun::Update()
{
    coolTime_--;
    pAimCursor_->Update();

    transform_.position_ = Model::GetBoneAnimPosition(hPlayerFPSModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;

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
    if (coolTime_ <= 0 && currentMagazineCount_ < magazineCount_) {
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
        
        //�`�����݊���
        if (peekTime_ <= 0) {
            isPeeking_ = true;
        }
    }
    else {
        peekTime_ = PEEK_TIME;
        isPeeking_ = false;
    }

    //�ʏ�ˌ�
    if (InputManager::IsCmdDown(InputManager::ATTACK, playerId_))
    {
        PressedShot();
    }

}

void SniperGun::Draw()
{
    //Shado�̏ꍇReturn
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    if (type == Direct3D::SHADER_SHADOWMAP) return;

    int dI = GameManager::GetDrawIndex();
    if (pPlayer_->GetPlayerId() == dI) {

        //�s�[�N���͉摜�̕\��
        if (isPeeking_) {
            if (Direct3D::GetCurrentShader() == Direct3D::SHADER_3D) {
                Transform t;
                Image::SetTransform(hPict_, t);
                Image::Draw(hPict_);
            
                Model::SetTransform(hModel_, transform_);
                Model::Draw(hModel_);
            }
        }
        else {
            transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
            Model::SetTransform(hModel_, transform_);
            Model::Draw(hModel_);
        }
    }
    //����̕\��
    else {
        transform_.rotate_.x = -pPlayer_->GetAim()->GetRotate().x;
        Model::SetTransform(hModel_, transform_);
        Model::Draw(hModel_);
    }

}

void SniperGun::Release()
{
}

void SniperGun::PressedShot()
{
    //�܂��N�[���^�C����
    if (coolTime_ > 0) {
        return;
    }

    //�}�K�W���v�Z
    if (currentMagazineCount_ <= 0) return;
    currentMagazineCount_--;

    ShotBullet<Bullet_Sniper>();
    pAimCursor_->Shot();

    CameraRotateShakeInfo rotShakeInfo = CameraRotateShakeInfo(XMFLOAT2(0.0f, 3.0f), 3);
    pPlayer_->GetAim()->SetCameraRotateShake(rotShakeInfo);
    pPlayer_->GetAim()->SetCameraRotateReturn(true);

}

bool SniperGun::PressedReload()
{
    //���łɖ��^��
    if (currentMagazineCount_ >= magazineCount_) return false;
    //���łɃ����[�h��
    if (currentReloadTime_ > 0) return false;
    //�N�[���^�C���ł���
    if (coolTime_ > 0) return false;

    Model::SetAnimFrame(hModel_, 0, 170, 1.0f);
    currentReloadTime_ = reloadTime_;
    coolTime_ = reloadTime_;
    return true;
}