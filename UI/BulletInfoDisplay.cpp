#include "BulletInfoDisplay.h"
#include "../Engine/Image.h"

BulletInfoDisplay::BulletInfoDisplay(GameObject* parent)
    :GameObject(parent, "BulletInfoDisplay"), drawX_(10), drawY_(20)
{
    for (int i = 0; i < pictSize; i++) { hPict_[i] = -1; };
}

BulletInfoDisplay::~BulletInfoDisplay()
{
}

void BulletInfoDisplay::Initialize()
{
    for (int i = 0; i < 10; i++) {
        std::string fileName = "Image/Time/Time";
        fileName = fileName + std::to_string(i);
        fileName = fileName + ".png";
        hPict_[i] = Image::Load(fileName);
        assert(hPict_[i] >= 0);
    }
}

void BulletInfoDisplay::Update()
{
}

//描画
void BulletInfoDisplay::Draw()
{
    int sec = 123;
    int firstDigit = sec % 10;                          //一の位
    int secondDigit = (sec / 10) % 10;                  //十の位の数字を取得
    int third = (sec / 100);

    //左の一の位
    Transform pic1 = transform_;
    pic1.scale_.x += 1.2f;
    pic1.scale_.y += 1.2f;
    pic1.position_ = { 0.0f, 0.8f, 0.0f };
    Image::SetTransform(hPict_[firstDigit], pic1);
    Image::Draw(hPict_[firstDigit]);

    //左の十の位
    Transform pic2 = pic1;
    pic2.position_.x -= 0.09f;
    Image::SetTransform(hPict_[secondDigit], pic2);
    Image::Draw(hPict_[secondDigit]);

    //左の百の位
    Transform pic3 = pic1;
    pic3 = pic1;
    pic3.position_.x -= 0.18f;
    Image::SetTransform(hPict_[third], pic3);
    Image::Draw(hPict_[third]);

}

//開放
void BulletInfoDisplay::Release()
{
}
