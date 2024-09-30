#include "BulletInfoDisplay.h"
#include "../Engine/Image.h"

namespace {
    XMFLOAT3 MAGAZINE_DRAW_POS = XMFLOAT3(0.4f, -0.8f, 0.0f);    //左端の座標
    float MAGAZINE_ADD_POS = 0.08f;                             //文字間のサイズ

    XMFLOAT3 MAGAZINE_SIZE = XMFLOAT3(1.0f, 1.0f, 1.0f);        //マガジン画像のサイズ
    XMFLOAT3 MAGAZINE_CENTER_SIZE = XMFLOAT3(0.6f, 0.25f, 1.0f); //マガジン中心画像のサイズ

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
    //Imageの初期設定
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

//描画
void BulletInfoDisplay::Draw()
{
}

//開放
void BulletInfoDisplay::Release()
{
}

void BulletInfoDisplay::DrawBullet()
{
    //十の位
    Transform picTrans = transform_;
    picTrans.position_ = MAGAZINE_DRAW_POS;
    picTrans.scale_ = MAGAZINE_SIZE;
    int secondDigit = (currentMagazine_ / 10) % 10;
    Image::SetTransform(hPict_[secondDigit], picTrans);
    Image::Draw(hPict_[secondDigit]);

    //一の位
    picTrans.position_.x += MAGAZINE_ADD_POS;
    int firstDigit = currentMagazine_ % 10;
    Image::SetTransform(hPict_[firstDigit], picTrans);
    Image::Draw(hPict_[firstDigit]);

    //中心の画像------------------------------
    picTrans.position_.x += MAGAZINE_ADD_POS;
    picTrans.scale_ = MAGAZINE_CENTER_SIZE;
    Image::SetTransform(hPict_[9 + CENTER + 1], picTrans);
    Image::Draw(hPict_[9 + CENTER + 1]);

    //最大マガジンサイズ----------------------
    //十の位
    picTrans.position_.x += MAGAZINE_ADD_POS;
    picTrans.scale_ = MAGAZINE_SIZE;
    secondDigit = (maxMagazine_ / 10) % 10;
    Image::SetTransform(hPict_[secondDigit], picTrans);
    Image::Draw(hPict_[secondDigit]);

    //一の位
    firstDigit = maxMagazine_ % 10;
    picTrans.position_.x += MAGAZINE_ADD_POS;
    Image::SetTransform(hPict_[firstDigit], picTrans);
    Image::Draw(hPict_[firstDigit]);

}
