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
	static const float HEIGHT_RANGE = 0.3f;		//ウィンドウ内での範囲外距離
	static const float PAD_DEAD_ZONE = 0.1f;	//パッドのデッドゾーン
	static const float PAD_FLUCTUATION = 0.05f;	//パッドのドラッキング変動値

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
	SelectAnimUpdate();

	//値を変える
	if (isDragging_) {
		//ドラッグを停止
		if (Input::IsMouseButtonUp(0) || Input::IsPadButtonUp(XINPUT_GAMEPAD_B, 0)) {
			isDragging_ = false;
			screenSelectPossible_ = true;
			return;
		}

		screenSelectPossible_ = false;
		return;
	}

	if (IsWithinBound()) {
		//カーソルが重なってるとき一回再生
		if (!isBound_)
		{
			//AudioManager::PlaySoundMa(AUDIO_POINTCURSOR);
			isBound_ = true;
		}

		//ドラッグを開始
		if (Input::IsMouseButtonDown(0) || Input::IsPadButtonDown(XINPUT_GAMEPAD_B, 0)) {
			isDragging_ = true;
			screenSelectPossible_ = false;
		}
	}
	else {
		isBound_ = false;
	}

}

void SliderUI::Draw()
{
	//濃いほうのバー
	Image::Draw(hButtonPict_[1]);

	//薄いほうのバー
	float scrX = (float)Direct3D::screenWidth_;
	float NXsize = Image::GetTextureSize(hButtonPict_[0]).x;
	float NYsize = Image::GetTextureSize(hButtonPict_[0]).y;
	Transform slider = frameTransform_;
	slider.position_.x -= (frameSize_.x / scrX) - frameSize_.x / scrX * gaugePercent_;
	Image::SetAlpha(hButtonPict_[0], (int)(selectAnim_ * 255.0f));
	Image::SetRect(hButtonPict_[0], 0, 0, (int)(NXsize * gaugePercent_), (int)NYsize);
	Image::SetTransform(hButtonPict_[0], slider);
	Image::Draw(hButtonPict_[0]);

	//ボタン
	Transform button = imageTransform_;
	if (isBound_) button.scale_ = Float3Multiply(button.scale_, BOUND_SIZE);
	Image::SetAlpha(hImagePict_, 200);
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

	Image::SetTransform(hButtonPict_[1], frameTransform_);

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

void SliderUI::SelectUpdate()
{
	if (isSelect_) {
		isBound_ = true;

		//値を変える
		if (isDragging_) {
			//ドラッグを停止
			if (Input::IsMouseButtonUp(0) || Input::IsPadButtonUp(XINPUT_GAMEPAD_B, 0)) {
				isDragging_ = false;
				return;
			}

			Dragging();
			return;
		}

		//ドラッグを開始
		if (Input::IsMouseButtonDown(0) || Input::IsPadButtonDown(XINPUT_GAMEPAD_B, 0)) {
			Dragging();
			isDragging_ = true;
		}
	}
}

void SliderUI::SetGaugeParcent(float f)
{
	gaugePercent_ = f;

	//場所計算
	float sizeX = frameSize_.x / (float)Direct3D::screenWidth_;
	imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
	buttonPosX_ = imageTransform_.position_.x;
}

void SliderUI::Dragging()
{
	float scrX = (float)Direct3D::screenWidth_;
	float scrY = (float)Direct3D::screenHeight_;
	float sizeX = frameSize_.x / scrX;

	//コントローラー用
	XMFLOAT3 mouseMove = Input::GetMouseMove();
	if (mouseMove.x == 0.0f) {
		//スティック移動
		XMFLOAT3 lStick = Input::GetPadStickL(0);
		if (lStick.x > PAD_DEAD_ZONE) gaugePercent_ += PAD_FLUCTUATION * abs(lStick.x);
		else if (lStick.x < -PAD_DEAD_ZONE) gaugePercent_ -= PAD_FLUCTUATION * abs(lStick.x);

		//矢印ボタン移動
		else if (Input::IsPadButton(XINPUT_GAMEPAD_DPAD_RIGHT, 0)) gaugePercent_ += PAD_FLUCTUATION;
		else if (Input::IsPadButton(XINPUT_GAMEPAD_DPAD_LEFT, 0)) gaugePercent_ -= PAD_FLUCTUATION;

		//上限
		if (gaugePercent_ > 1.0f) gaugePercent_ = 1.0f;
		else if (gaugePercent_ < 0.0f) gaugePercent_ = 0.0f;

		//スライダーボタン
		imageTransform_.position_.x = frameTransform_.position_.x + (sizeX * 2.0f * gaugePercent_) - sizeX;
		buttonPosX_ = imageTransform_.position_.x;

		//関数呼び出し
		OnClick();
		return;
	}

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
	if (mouse.y > (frameTransform_.position_.y + HEIGHT_RANGE) ||
		mouse.y < (frameTransform_.position_.y - HEIGHT_RANGE))
	{
		isDragging_ = false;
		return;
	}
	
	//０～１に変換
	float mousePar = (mouse.x + 1.0f) * 0.5f;
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
