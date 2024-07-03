#include "AimCursor.h"
#include "../Engine/Image.h"
#include "../Engine/Direct3D.h"

AimCursor::AimCursor() : hPict_(-1)
{
	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);

	Transform t;
#if ONE_PLAYER
	t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
#else
	t.scale_ = XMFLOAT3(0.55f, 0.3f, 0.3f);
#endif
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
