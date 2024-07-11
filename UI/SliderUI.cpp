#include "SliderUI.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"
#include "../Engine/Input.h"
#include "../Engine/Global.h"
#include "../Engine/Easing.h"

namespace {
	static const float BOUND_SIZE = 1.1f;		//通常サイズに掛けて表示
	static const float BUTTON_SIZE = 0.05f;		//IsBoundの判定距離
	static const float BORDER_POSX = 10000.0f;	//マウスのウィンドウ範囲外対処用

}

SliderUI::SliderUI(XMFLOAT2 pos, XMFLOAT2 size, std::function<void()> onClick, XMFLOAT2 tsize)
	: UIBase(pos, size, onClick, tsize), buttonPosX_(0.0f), gaugePercent_(0.5f), isDragging_(false), frameSize_{0,0}
{
}

SliderUI::~SliderUI()
{
}

void SliderUI::Update()
{
	//値を変える
	if (isDragging_) {
		//マウスの左ボタンが離されたとき、ドラッグを停止
		if (Input::IsMouseButtonUp(0)) {
			isDragging_ = false;
			return;
		}

		Dragging();
		return;
	}

	if (IsWithinBound()) {
		//カーソルが重なってるとき一回再生
		if (!isBound_)
		{
			//AudioManager::PlaySoundMa(AUDIO_POINTCURSOR);
			isBound_ = true;
		}

		//マウスの左ボタンが押されたとき、ドラッグを開始
		if (Input::IsMouseButtonDown(0)) {
			Dragging();
			isDragging_ = true;
		}
	}
	else {
		isBound_ = false;
	}

}

void SliderUI::Draw()
{
	float scrX = (float)Direct3D::screenWidth_;

	//濃いほうのバー
	Image::SetTransform(hButtonPict_[1], frameTransform_);
	Image::Draw(hButtonPict_[1]);

	//薄いほうのバー
	float NXsize = Image::GetTextureSize(hButtonPict_[0]).x;
	float NYsize = Image::GetTextureSize(hButtonPict_[0]).y;
	Transform slider = frameTransform_;
	slider.position_.x -= (frameSize_.x / scrX) - frameSize_.x / scrX * gaugePercent_;
	Image::SetRect(hButtonPict_[0], 0, 0, (int)(NXsize * gaugePercent_) , NYsize);
	Image::SetTransform(hButtonPict_[0], slider);
	Image::Draw(hButtonPict_[0]);

	//ボタン
	Transform button = imageTransform_;
	if (isBound_) button.scale_ = Float3Multiply(button.scale_, BOUND_SIZE);
	Image::SetAlpha(hImagePict_, 100);
	Image::SetTransform(hImagePict_, button);
	Image::Draw(hImagePict_);
}

void SliderUI::Initialize(std::string name)
{
	//画像データ読み込み
	std::string fileName[] = { "SliderFrame", "Slider", "SliderButton" };
	for (int i = 0; i < 2; i++) {
		hButtonPict_[i] = Image::Load("Image/" + fileName[i] + ".png");
		assert(hButtonPict_[i] >= 0);
	}
	hImagePict_ = Image::Load("Image/" + fileName[2] + ".png");
	assert(hImagePict_ >= 0);

	//Scaleに合わせて調整
	frameSize_ = { Image::GetTextureSize(hButtonPict_[0]).x, Image::GetTextureSize(hButtonPict_[0]).y};
	frameSize_ = { frameSize_.x * frameTransform_.scale_.x, frameSize_.y * frameTransform_.scale_.y };

	//スライダーボタン
	float sizeX = frameSize_.x / (float)Direct3D::screenWidth_;
	imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
	buttonPosX_ = imageTransform_.position_.x;

}

bool SliderUI::IsWithinBound()
{
	//-1～１の範囲に計算
	float scrX = (float)Direct3D::screenWidth_;
	float scrY = (float)Direct3D::screenHeight_;
	XMFLOAT3 mouse = Input::GetMousePosition();
	mouse = { mouse.x * 2.0f - scrX, mouse.y * 2.0f - scrY, 0.0f };
	mouse = { mouse.x / scrX, -(mouse.y / scrY), 0.0f };

	//判定距離内か丸と点で判定
	XMFLOAT3 button = { buttonPosX_, frameTransform_.position_.y, 0.0f };
	float dist = CalculationDistance(mouse, button);
	return (dist <= BUTTON_SIZE);
}

void SliderUI::Dragging()
{
	float scrX = (float)Direct3D::screenWidth_;
	float scrY = (float)Direct3D::screenHeight_;

	//ウィンドウ外の対処
	XMFLOAT3 mouse = Input::GetMousePosition();
	if (mouse.x > BORDER_POSX || mouse.x > scrX) {
		isDragging_ = false;
		return;
	}

	//-1～１の範囲に計算
	mouse = { mouse.x * 2.0f - scrX, mouse.y * 2.0f - scrY, 0.0f };
	mouse = { mouse.x / scrX, -(mouse.y / scrY), 0.0f };

	//上下で終了判定
	static const float HEIGHT_RANGE = 0.3f;
	if (mouse.y > (frameTransform_.position_.y + HEIGHT_RANGE) ||
		mouse.y < (frameTransform_.position_.y - HEIGHT_RANGE))
	{
		isDragging_ = false;
		return;
	}
	
	//０～１に変換
	float mousePar = (mouse.x + 1.0f) * 0.5f;
	float sizeX = frameSize_.x / scrX;
	float posE = ((frameTransform_.position_.x + sizeX) + 1.0f) * 0.5f;
	float posS = ((frameTransform_.position_.x - sizeX) + 1.0f) * 0.5f;

	//ゲージ計算
	gaugePercent_ = (mousePar - posS) / (posE - posS);
	if (gaugePercent_ < 0.0f) gaugePercent_ = 0.0f;
	else if (gaugePercent_ > 1.0f) gaugePercent_ = 1.0f;

	//スライダーボタン
	imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
	buttonPosX_ = imageTransform_.position_.x;

	//関数呼び出し
	OnClick();
}
