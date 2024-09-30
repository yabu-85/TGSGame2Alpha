#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;

namespace Easing
{
	//イージング関数
	//https://easings.net/ja

	float EaseInQuint(float x);
	float EaseOutQuint(float x);
	float EaseInOutQuint(float x);

	//自分で指定できるイージング関数
	float Leap(float start, float end, float x);
};
