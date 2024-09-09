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

GunBase::GunBase(GameObject* parent, const std::string& name)
    : GameObject(parent, name), hModel_(-1), pAimCursor_(nullptr), playerId_(0), coolTime_(0), rayHit_(false), 
    rootBoneIndex_(-1), rootPartIndex_(-1), topBoneIndex_(-1), topPartIndex_(-1), isPeeking_(false), peekTime_(0), reloadTime_(0), 
    currentReloadTime_(0), magazineCount_(0), currentMagazineCount_(0), hPlayerModel_(-1), handBoneIndex_(-1), handPartIndex_(-1),
    hPlayerFPSModel_(-1), animTime_(0), currentPeekTime_(0)
{
    pPlayer_ = static_cast<Player*>(GetParent());
    playerId_ = pPlayer_->GetPlayerId();
    hPlayerModel_ = pPlayer_->GetModelHandle();
    hPlayerFPSModel_ = pPlayer_->GetFPSModelHandle();
    Model::GetPartBoneIndex(hPlayerModel_, "Weapon", &handPartIndex_, &handBoneIndex_);

    //�Ƃ�܂�������(�C���ӏ�
    peekTime_ = 50;
}

GunBase::~GunBase()
{
    SAFE_DELETE(pAimCursor_);

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
}

void GunBase::SetGunHandPosition()
{
    transform_.position_ = Model::GetBoneAnimPosition(hPlayerModel_, handPartIndex_, handBoneIndex_);
    transform_.rotate_.y = pPlayer_->GetRotate().y;
}

bool GunBase::Reload()
{
    currentReloadTime_--;

    //�����[�h����
    if (currentReloadTime_ <= 0) {
        currentMagazineCount_ = magazineCount_;
        return true;
    }

    return false;
}

//�v���C���[�̃A�j���[�V�����̂����鎞�Ԃƕ���Ɏ������������鎞�ԂŌv�Z����AnimationSpeed���v�Z���Ă��
//�������AAnimation�J�n�Ƃ��̃R�[�h��PlayerState�ɒu������
//IsCmdDown��IsCmdUp�͂Ȃ�ׂ��g����
//�W�����v���̃A�j���[�V�����₯�ǁA�J������x��Ă��Ă���悤�ɂ��邾���ł�������
//�Đ�����A�j���[�V�����̓W�����v�ɂ���A����Anim�͎~�߂Ă������ȁA����ƃW�����v���͔`�����݂ł��Ȃ��悤�ɂ���
void GunBase::Peeking()
{
    bool playerClimb = pPlayer_->IsClimb();

    if (InputManager::IsCmd(InputManager::AIM, playerId_)) {
        currentPeekTime_--;

        float zoom = (float)currentPeekTime_ / (float)peekTime_;
        zoom = std::clamp(zoom, 0.7f, 1.0f);
        Camera::SetPeekFOVZoom(zoom, playerId_);

        //�`�����݊���
        if (currentPeekTime_ <= 0) {
            currentPeekTime_ = 0;
            isPeeking_ = true;
        }
    }
    else {
        //Peek��߂���
        if (isPeeking_ ) {
            int a = 0;
        }

        isPeeking_ = false;
        currentPeekTime_++;

        //���S��Peek�I�����
        if (currentPeekTime_ >= peekTime_) {
            currentPeekTime_ = peekTime_;
        }

        float zoom = (float)currentPeekTime_ / (float)peekTime_;
        zoom = std::clamp(zoom, 0.7f, 1.0f);
        Camera::SetPeekFOVZoom(zoom, playerId_);
    }

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
    float bure = pAimCursor_->GetBurePower();
    XMFLOAT3 bureMove = XMFLOAT3();
    bureMove.x = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
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
    float bure = pAimCursor_->GetBurePower();
    XMFLOAT3 bureMove = XMFLOAT3();
    bureMove.x = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.y = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
    bureMove.z = bure * (BURE_POWER * (float)(rand() % 200 - 100) * 0.01f);
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