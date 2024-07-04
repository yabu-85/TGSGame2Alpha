#include "AimCursor.h"
#include "../Engine/Image.h"
#include "../Engine/Easing.h"
#include "../Other/GameManager.h"

namespace {
	XMFLOAT3 ONE_PLAYER_SIZE = XMFLOAT3(0.3f, 0.3f, 0.3f);
	XMFLOAT3 TWO_PLAYER_SIZE = XMFLOAT3(0.55f, 0.3f, 0.3f);

}

AimCursor::AimCursor() : hPict_(-1), shotParce_(0.0f), shotDurace_(0.01f)
{
	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);
}

AimCursor::~AimCursor()
{
}

void AimCursor::Update()
{
	shotParce_ -= shotDurace_;
	if (shotParce_ < 0.0f) shotParce_ = 0.0f;

}

void AimCursor::Draw()
{
	//l”‚É‚æ‚Á‚Ä‘å‚«‚³’²®
	Transform t;
	float scale = Easing::EaseInQuint(shotParce_);
	if (GameManager::IsOnePlayer()) {
		t.scale_ = ONE_PLAYER_SIZE;
		t.scale_.x += t.scale_.x * scale;
		t.scale_.y += t.scale_.y * scale;
		t.scale_.z += t.scale_.z * scale;
	}
	else {
		t.scale_ = TWO_PLAYER_SIZE;
		t.scale_.x += t.scale_.x * scale;
		t.scale_.y += t.scale_.y * scale;
		t.scale_.z += t.scale_.z * scale;
	}

	Image::SetTransform(hPict_, t);
	Image::Draw(hPict_);
}

void AimCursor::Shot()
{
	shotParce_ = 1.0f;

}
