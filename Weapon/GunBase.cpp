#include "GunBase.h"
#include "BulletBase.h"
#include "../Engine/Global.h"
#include "../Engine/Model.h"
#include "../Engine/Camera.h"
#include "../Engine/SegmentCollider.h"
#include "../Json/JsonReader.h"
#include "../Player/Player.h"
#include "../Player/Aim.h"
#include "../Other/GameManager.h"
#include "../Stage/CollisionMap.h"
#include "../Character/Character.h"
#include "../Character/CharacterManager.h"
#include "../UI/AimCursor.h"
#include "../Other/InputManager.h"

//�v���C���[�̃A�j���[�V�����i�����[�h
#include "../Animation/AnimationController.h"
#include "../State/StateManager.h"

GunBase::GunBase(GameObject* parent, const std::string& name)
    : GameObject(parent, name), hModel_(-1), playerId_(0), coolTime_(0), rayHit_(false), rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1),
    isPeeking_(false), peekTime_(0), reloadTime_(0), currentReloadTime_(0), magazineCount_(0), currentMagazineCount_(0), hFpsPlayerModel_(-1), handBoneIndex_(-1),
    handPartIndex_(-1), animTime_(0), currentPeekTime_(0), peekZoom_(0.0f), hipFireAccuracy_(0.0f), aimingAccuracy_(0.0f), accuracyDecrease_(0.0f), 
    accuracyRecovery_(0.0f), currentAccuracy_(0.0f), hUpPlayerModel_(-1), isPrePeekInput_(false), maxAccuracy_(0.0f)
{
    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();
    hUpPlayerModel_ = pPlayer_->GetUpModelHandle();
    hFpsPlayerModel_ = pPlayer_->GetFPSModelHandle();
    Model::GetPartBoneIndex(hFpsPlayerModel_, "Weapon", &handPartIndex_, &handBoneIndex_);
}

GunBase::~GunBase()
{
}

void GunBase::OnCollision(GameObject* pTarget)
{
    // �G�ɓ��������Ƃ�
    if (pTarget->GetObjectName().find("Enemy") != std::string::npos ||
        pTarget->GetObjectName().find("Player") != std::string::npos)
    {
        rayHit_ = true;
    }
}

float GunBase::GetAccuracy()
{
    if (isPeeking_) return aimingAccuracy_;
    return currentAccuracy_ + hipFireAccuracy_;
}

void GunBase::LoadGunJson(std::string fileName)
{
    //�e�e���Ƃ̐ݒ��ǂݍ���
    JsonReader::Load("Json/Weapon.json");
    auto& gunSection = JsonReader::GetSection(fileName);

    //�}�K�W�����̏�����
    magazineCount_ = gunSection["magazineCount"];
    currentMagazineCount_ = magazineCount_;

    //�����[�h���Ԃ̏�����
    reloadTime_ = gunSection["reloadTime"];
    currentReloadTime_ = 0;

    //�̂�������
    peekZoom_ = gunSection["peekValue"];
    peekTime_ = gunSection["peekTime"];
    currentPeekTime_ = peekTime_;

    //�W�e��
    hipFireAccuracy_ = gunSection["hipFireAccuracy"];
    aimingAccuracy_ = gunSection["aimingAccuracy"];
    accuracyDecrease_ = gunSection["accuracyDecrease"];
    accuracyRecovery_ = gunSection["accuracyRecovery"];
    maxAccuracy_ = gunSection["maxAccuracy"];

}

void GunBase::EnterReload()
{
    //reloadTime�ɍ��킹�āA�A�j���[�V�����̍Đ����x�v�Z���čĐ�
    int reloadS = pPlayer_->GetUpAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).startFrame;
    int reloadE = pPlayer_->GetUpAnimationController()->GetAnim((int)PLAYER_ANIMATION::RELOAD).endFrame;
    int reloadAnimTime = (reloadE - reloadS);
    float animSpeed = (float)reloadAnimTime / (float)reloadTime_;
    pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);
    pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::RELOAD, animSpeed);

}

bool GunBase::Reload()
{
    currentReloadTime_--;

    //�����[�h����
    if (currentReloadTime_ <= 0) {
        currentMagazineCount_ = magazineCount_;
        
        //���̑��EPeek���ԏ�����
        currentPeekTime_ = peekTime_;

        FinishedReload();
        return true;
    }

    return false;
}

void GunBase::FinishedReload()
{
    Model::SetAnimFrame(hModel_, 0, 0, 0.0f);

    //�v���C���[��State��Idle��Jump��������
    std::string stateName = pPlayer_->GetStateManager()->GetName();
    if (stateName == "Idle" || stateName == "Move" ) {
        pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
        pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
    }
}

void GunBase::Peeking()
{
    bool returnFlag = pPlayer_->IsClimb() || pPlayer_->IsFly() || currentReloadTime_ > 0;        //�s�[�N�ł��Ȃ�
    bool isPeekInput = InputManager::IsCmd(InputManager::AIM, playerId_);   //PeekInput�����Ă邩�ǂ���

    //�߂�����
    if (returnFlag || !isPeekInput) {
        //�߂������I����Ă���ꍇ�͏I��
        if (currentPeekTime_ >= peekTime_) {
            return;
        }

        //�؂�ւ�����i�`�����݉����j
        if (isPrePeekInput_) {
            int peekAnimTime = pPlayer_->GetUpAnimationController()->GetAnimTime((int)PLAYER_ANIMATION::PEEK_END);
            float animSpeed = (float)peekAnimTime / (float)(peekTime_);
            int addAnimStart = (int)((float)currentPeekTime_ / (float)peekTime_ * (float)peekAnimTime);

            //�����[�h���ł͂Ȃ��Ȃ�
            if (currentReloadTime_ <= 0) {
                pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK_END, animSpeed, addAnimStart);
                pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK_END, animSpeed, addAnimStart);
            }

            isPrePeekInput_ = false;
        }

        isPeeking_ = false;
        currentPeekTime_++;

        //���S�ɖ߂���
        if (currentPeekTime_ >= peekTime_) {
            currentPeekTime_ = peekTime_;
         
            //�����[�h���ł͂Ȃ��Ȃ�
            if (currentReloadTime_ <= 0) {
                pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
                pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::IDLE, 1.0f);
            }
        }

        float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
        Camera::SetFovAngleParcent(zoom, playerId_);
        return;
    }

    //�`�����݂��Ă���Ƃ��̏���
    //���łɔ`�����݂��Ă���ꍇ
    if (currentPeekTime_ <= 0) {
        return;
    }

    //�؂�ւ�����i�`�����݃X�^�[�g�j
    if (!isPrePeekInput_) {
        int peekAnimTime = pPlayer_->GetUpAnimationController()->GetAnimTime((int)PLAYER_ANIMATION::PEEK_START);
        float animSpeed = (float)peekAnimTime / (float)(peekTime_);
        int addAnimStart = peekAnimTime - (int)((float)currentPeekTime_ / (float)peekTime_ * (float)peekAnimTime);

        pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK_START, animSpeed, addAnimStart);
        pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK_START, animSpeed, addAnimStart);
        isPrePeekInput_ = true; 
    }

    //�Y�[���l�v�Z
    currentPeekTime_--;
    float zoom = peekZoom_ + ((1.0f - peekZoom_) * (float)currentPeekTime_ / (float)peekTime_);
    Camera::SetFovAngleParcent(zoom, playerId_);

    //�`�����݊���
    if (currentPeekTime_ <= 0) {
        currentPeekTime_ = 0;
        isPeeking_ = true;

        pPlayer_->GetUpAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK, 1.0f);
        pPlayer_->GetFpsAnimationController()->SetNextAnim((int)PLAYER_ANIMATION::PEEK, 1.0f);
    }

}

void GunBase::ShotVFX()
{
    int selfId = playerId_;
    int otheId = 0;
    if (selfId == 0) otheId = 1;

    //����̃V���b�g�G�t�F�N�g
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
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t, otheId);

    //�����̃V���b�g�G�t�F�N�g
    XMFLOAT3 subAim = pPlayer_->GetAim()->GetFPSSubY();
    transform.position_ = Float3Add(transform.position_, subAim);
    DirectX::XMStoreFloat4x4(&(t.matrix), transform.GetWorldMatrix());
    EFFEKSEERLIB::gEfk->Play("GUNSHOT", t, selfId);

}

void GunBase::SetTpsPlayerHandGun()
{
    transform_.position_ = Model::GetBoneAnimPosition(hUpPlayerModel_, handPartIndex_, handBoneIndex_);
}

void GunBase::SetFpsPlayerHandGun()
{
    //Aim�̍��������킹��
    XMFLOAT3 subAim = pPlayer_->GetAim()->GetFPSSubY();
    transform_.position_ = Model::GetBoneAnimPosition(hFpsPlayerModel_, handPartIndex_, handBoneIndex_);
    transform_.position_ = Float3Add(transform_.position_, subAim);
}

void GunBase::ShotBullet(BulletBase* pBullet)
{
    coolTime_ = pBullet->GetBulletParameter().shotCoolTime_;
    float calcDist = pBullet->GetBulletParameter().speed_ * pBullet->GetBulletParameter().killTimer_;
    pBullet->SetPlayerId(playerId_);

    //�F�X���v�Z
    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);
    XMFLOAT3 GunBaseTop = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    XMFLOAT3 GunBaseRoot = Model::GetBonePosition(hModel_, rootPartIndex_, rootBoneIndex_);
    XMFLOAT3 cameraVec = Float3Sub(cameraTar, cameraPos);
    float cameraDist = CalculationDistance(cameraVec);
    cameraVec = Float3Normalize(cameraVec);

    //�u���̕����v�Z������
    static const float BURE_POWER = 0.3f;
    XMFLOAT3 bureMove = XMFLOAT3();
    float accuracy = GetAccuracy();
    bureMove.x = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    XMMATRIX matRotX = XMMatrixRotationX(bureMove.x);
    XMMATRIX matRotY = XMMatrixRotationY(bureMove.y);
    XMMATRIX matRotZ = XMMatrixRotationZ(bureMove.z);
    XMVECTOR vecDir = XMVector3TransformCoord(XMLoadFloat3(&cameraVec), matRotX * matRotY * matRotZ);
    XMFLOAT3 shotVec = XMFLOAT3();
    XMStoreFloat3(&shotVec, vecDir);

    //�R���W�����}�b�v�Œ��e�n�_�v�Z
    float weaponDist = CalculationDistance(GunBaseTop, GunBaseRoot);
    RayCastData data;
    data.start = Float3Add(cameraTar, Float3Multiply(cameraVec, weaponDist));
    data.dir = shotVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist + cameraDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[�[
    //�Ώۂɂ����������ŒZ�������v�Z
    std::vector<Character*>charaList = CharacterManager::GetCharacterList();
    int minIndex = -1;
    float minDist = 999999;
    XMFLOAT3 minHitPos = XMFLOAT3();

    //�R���C�_�[�o�^
    XMFLOAT3 centerPos = Float3Sub(cameraPos, GetWorldPosition());
    SegmentCollider* collid = new SegmentCollider(centerPos, XMLoadFloat3(&data.dir));
    collid->size_ = XMFLOAT3(calcDist + cameraDist, calcDist + cameraDist, calcDist + cameraDist);
    collid->typeList_.push_back(OBJECT_TYPE::Enemy);
    collid->typeList_.push_back(OBJECT_TYPE::Player);
    AddCollider(collid);

    for (int i = 0; i < charaList.size(); i++) {
        //�e�̓X�L�b�v
        if (charaList[i] == pPlayer_) continue;

        //�����v�Z
        XMFLOAT3 vec = Float3Sub(charaList[i]->GetPosition(), data.start);
        float hitDist = CalculationDistance(vec);
        if (hitDist > data.dist) continue;

        //�ǂɓ����鋗������Ȃ�����A�����蔻����
        rayHit_ = false;
        this->Collision(charaList[i]);

        //�ŒZ�����œ�������
        if (rayHit_ && hitDist < minDist) {
            minDist = hitDist;
            minIndex = i;
            minHitPos = collid->targetPos_;
        }
    }

    //Aim�œ�����Ȃ���������A�{���̋O�ՂŔ�����s��
    if (minIndex < 0) {
        //�R���W�����}�b�v�Ƃ̔���
        XMFLOAT3 GunBaseVec = Float3Normalize(Float3Sub(stageHitPos, GunBaseTop));
        data = RayCastData();
        data.start = GunBaseTop;

        vecDir = XMVector3TransformCoord(XMLoadFloat3(&GunBaseVec), matRotX * matRotY * matRotZ);
        XMStoreFloat3(&shotVec, vecDir);

        data.dir = shotVec;
        calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
        GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
        stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));
    }
    ClearCollider();

    XMFLOAT3 endTarget = stageHitPos;

    //�G��ڕW�ɂ��Ă����ꍇ
    Character* chara = nullptr;
    //���x�Ƃ��֌W�Ȃ���u�œ��B����e�Ȃ�GunBase�Ŕ��肷�邾���ōςނ���A�c���Ă���
    if (minIndex >= 0) {
        chara = charaList[minIndex];
        endTarget = minHitPos;
    }

    float bulletSpeed = pBullet->GetBulletParameter().speed_;
    XMFLOAT3 move = Float3Sub(endTarget, GunBaseTop);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);

    pBullet->SetDrawPosition(GunBaseTop);
    pBullet->SetDrawMove(move);

    pBullet->SetMove(move);
    pBullet->SetPosition(GunBaseTop);
    pBullet->Shot(chara, stageHitPos, minHitPos);
}

void GunBase::ShotFPSBullet(BulletBase* pBullet)
{
    float bulletSpeed = pBullet->GetBulletParameter().speed_;
    float calcDist = bulletSpeed * pBullet->GetBulletParameter().killTimer_;
    coolTime_ = pBullet->GetBulletParameter().shotCoolTime_;
    pBullet->SetPlayerId(playerId_);

    //�J�������v�Z
    XMFLOAT3 cameraPos = Camera::GetPosition(playerId_);
    XMFLOAT3 cameraTar = Camera::GetTarget(playerId_);
    XMFLOAT3 cameraVec = Float3Sub(cameraTar, cameraPos);
    cameraVec = Float3Normalize(cameraVec);

    //�u���̕����v�Z������
    static const float BURE_POWER = 0.3f;
    float accuracy = GetAccuracy();
    XMFLOAT3 bureMove = XMFLOAT3();
    bureMove.x = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = accuracy * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    XMMATRIX matRotX = XMMatrixRotationX(bureMove.x);
    XMMATRIX matRotY = XMMatrixRotationY(bureMove.y);
    XMMATRIX matRotZ = XMMatrixRotationZ(bureMove.z);
    XMVECTOR vecDir = XMVector3TransformCoord(XMLoadFloat3(&cameraVec), matRotX * matRotY * matRotZ);
    XMFLOAT3 shotVec = XMFLOAT3();
    XMStoreFloat3(&shotVec, vecDir);

    //�R���W�����}�b�v�Œ��e�n�_�v�Z
    RayCastData data;
    data.start = cameraPos;
    data.dir = shotVec;
    XMFLOAT3 calcTar = Float3Add(data.start, Float3Multiply(data.dir, calcDist));
    GameManager::GetCollisionMap()->RaySelectCellVsSegment(calcTar, &data);
    XMFLOAT3 stageHitPos = Float3Add(data.start, Float3Multiply(data.dir, data.dist));

    //�\���p�̏��Z�b�g
    XMFLOAT3 GunBaseTop = Model::GetBonePosition(hModel_, topPartIndex_, topBoneIndex_);
    XMFLOAT3 drawMove = Float3Sub(stageHitPos, GunBaseTop);
    drawMove = Float3Multiply(Float3Normalize(drawMove), bulletSpeed);
    pBullet->SetDrawPosition(GunBaseTop);
    pBullet->SetDrawMove(drawMove);

    //�v�Z�p�̏��Z�b�g
    XMFLOAT3 move = Float3Sub(stageHitPos, cameraPos);
    move = Float3Multiply(Float3Normalize(move), bulletSpeed);
    pBullet->SetPosition(cameraPos);
    pBullet->SetMove(move);

    pBullet->Shot(nullptr, stageHitPos, XMFLOAT3());
}