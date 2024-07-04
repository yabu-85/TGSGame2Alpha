#include "AimCursor.h"
#include "../Engine/Image.h"
#include "../Other/GameManager.h"

namespace {
	XMFLOAT3 ONE_PLAYER_SIZE = XMFLOAT3(0.3f, 0.3f, 0.3f);
	XMFLOAT3 TWO_PLAYER_SIZE = XMFLOAT3(0.55f, 0.3f, 0.3f);

}

AimCursor::AimCursor() : hPict_(-1)
{
	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);

	//l”‚É‚æ‚Á‚Ä‘å‚«‚³’²®
	Transform t;
	if (GameManager::IsOnePlayer()) t.scale_ = ONE_PLAYER_SIZE;
	else t.scale_ = TWO_PLAYER_SIZE;
	Image::SetTransform(hPict_, t);

}

AimCursor::~AimCursor()
{
}

void AimCursor::Update()
{
}

void AimCursor::Draw()
{
	Image::Draw(hPict_);

}
