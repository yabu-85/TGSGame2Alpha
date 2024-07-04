#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

namespace Easing
{
	//�C�[�W���O�֐�
	//https://easings.net/ja

	float EaseInQuint(float x);
	float EaseOutQuint(float x);
	float EaseInOutQuint(float x);

	//�����Ŏw��ł���C�[�W���O�֐�
	float Leap(float start, float end, float x);
};
