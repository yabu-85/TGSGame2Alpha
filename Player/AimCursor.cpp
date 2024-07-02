#include "AimCursor.h"
#include "../Engine/Image.h"

AimCursor::AimCursor(GameObject* parent) :
	hPict_(-1)
{
}

AimCursor::~AimCursor()
{
}

void AimCursor::Initialize()
{
	hPict_ = Image::Load("Image/cross.png");
	assert(hPict_ >= 0);

}

void AimCursor::Update()
{
}

void AimCursor::Draw()
{
	Transform t;
	t.scale_ = XMFLOAT3(0.3f, 0.3f, 0.3f);
	Image::SetTransform(hPict_, t);
	Image::Draw(hPict_);

}

void AimCursor::Release()
{
}
