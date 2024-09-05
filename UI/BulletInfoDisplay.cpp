#include "BulletInfoDisplay.h"
#include "../Engine/Image.h"

namespace {
    XMFLOAT3 MAGAZINE_DRAW_POS = XMFLOAT3(0.4f, -0.8f, 0.0f);    //���[�̍��W
    float MAGAZINE_ADD_POS = 0.08f;                             //�����Ԃ̃T�C�Y

    XMFLOAT3 MAGAZINE_SIZE = XMFLOAT3(1.0f, 1.0f, 1.0f);        //�}�K�W���摜�̃T�C�Y
    XMFLOAT3 MAGAZINE_CENTER_SIZE = XMFLOAT3(0.6f, 0.25f, 1.0f); //�}�K�W�����S�摜�̃T�C�Y

}

BulletInfoDisplay::BulletInfoDisplay(GameObject* parent)
    : GameObject(parent, "BulletInfoDisplay"), currentMagazine_(0), maxMagazine_(0)
{
    for (int i = 0; i < 10 + MAX; i++) { hPict_[i] = -1; };
}

BulletInfoDisplay::~BulletInfoDisplay()
{
}

void BulletInfoDisplay::Initialize()
{
    //Image�̏����ݒ�
    for (int i = 0; i < 10; i++) {
        std::string fileName = "Image/Time/Time";
        fileName = fileName + std::to_string(i);
        fileName = fileName + ".png";
        hPict_[i] = Image::Load(fileName);
        assert(hPict_[i] >= 0);
    }

    const char* fileName[MAX] = { "Image/MagazineCenter.png" };
    for (int i = 0; i < MAX; i++) {
        hPict_[9 + i + 1] = Image::Load(fileName[i]);
        assert(hPict_[9 + i + 1] >= 0);
    }

}

void BulletInfoDisplay::Update()
{
}

//�`��
void BulletInfoDisplay::Draw()
{
}

//�J��
void BulletInfoDisplay::Release()
{
}

void BulletInfoDisplay::DrawBullet()
{
    //�\�̈�
    Transform picTrans = transform_;
    picTrans.position_ = MAGAZINE_DRAW_POS;
    picTrans.scale_ = MAGAZINE_SIZE;
    int secondDigit = (currentMagazine_ / 10) % 10;
    Image::SetTransform(hPict_[secondDigit], picTrans);
    Image::Draw(hPict_[secondDigit]);

    //��̈�
    picTrans.position_.x += MAGAZINE_ADD_POS;
    int firstDigit = currentMagazine_ % 10;
    Image::SetTransform(hPict_[firstDigit], picTrans);
    Image::Draw(hPict_[firstDigit]);

    //���S�̉摜------------------------------
    picTrans.position_.x += MAGAZINE_ADD_POS;
    picTrans.scale_ = MAGAZINE_CENTER_SIZE;
    Image::SetTransform(hPict_[9 + CENTER + 1], picTrans);
    Image::Draw(hPict_[9 + CENTER + 1]);

    //�ő�}�K�W���T�C�Y----------------------
    //�\�̈�
    picTrans.position_.x += MAGAZINE_ADD_POS;
    picTrans.scale_ = MAGAZINE_SIZE;
    secondDigit = (maxMagazine_ / 10) % 10;
    Image::SetTransform(hPict_[secondDigit], picTrans);
    Image::Draw(hPict_[secondDigit]);

    //��̈�
    firstDigit = maxMagazine_ % 10;
    picTrans.position_.x += MAGAZINE_ADD_POS;
    Image::SetTransform(hPict_[firstDigit], picTrans);
    Image::Draw(hPict_[firstDigit]);

}
