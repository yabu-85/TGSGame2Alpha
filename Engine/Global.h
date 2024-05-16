#pragma once
#include "Direct3D.h"

//安全にメモリを開放するためのマクロ
#define SAFE_DELETE(p) {if ((p)!=nullptr) { delete (p); (p)=nullptr;}}
#define SAFE_DELETE_ARRAY(p) {if ((p)!=nullptr) { delete[] (p); (p)=nullptr;}}
#define SAFE_RELEASE(p) {if ((p)!=nullptr) { p->Release(); (p)=nullptr;}}

//XMFLOAT3同士の足し算
static XMFLOAT3 Float3Add(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

//XMFLOAT3同士の引き算
static XMFLOAT3 Float3Sub(XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
}

//XMFLOAT3とfloatの掛け算
static XMFLOAT3 Float3Multiply(XMFLOAT3 a, float scalar)
{
	return XMFLOAT3(a.x * scalar, a.y * scalar, a.z * scalar);
}

//XMFLOAT3を正規化
static XMFLOAT3 Float3Normalize(XMFLOAT3 a)
{
	XMVECTOR v = XMVector3Normalize(XMLoadFloat3(&a));
	XMStoreFloat3(&a, v);
	return a;
}

//距離を計算
static float CalculationDistance(XMFLOAT3 dir)
{
	return sqrtf((dir.x * dir.x) + (dir.y * dir.y) + (dir.z * dir.z));
}

//距離を計算
static float CalculationDistance(XMFLOAT3 a, XMFLOAT3 b)
{
	XMFLOAT3 c = Float3Sub(a, b);
	return sqrtf((c.x * c.x) + (c.y * c.y) + (c.z * c.z));
}

//ベクトルから回転軸を計算（度）
static XMFLOAT3 CalculationRotate(XMFLOAT3 dir) {
	XMFLOAT3 rot = XMFLOAT3();
	rot.x = XMConvertToDegrees(-asinf(dir.y));
	rot.y = XMConvertToDegrees(atan2f(dir.x, dir.z));
	return rot;
}

//ベクトルから回転軸を計算（度）
static float CalculationRotateZ(XMFLOAT3 dir) {
	float cosX = cosf(dir.x * XM_PI / 180.0f);
	float cosY = cosf(dir.y * XM_PI / 180.0f);
	float sinX = sinf(dir.x * XM_PI / 180.0f);
	float sinY = sinf(dir.y * XM_PI / 180.0f);
	return XMConvertToDegrees(atan2f(-sinY * dir.x + cosY * dir.z, dir.y * sinX + (dir.x * cosY + dir.z * sinY) * cosX));
}

//回転軸から向いている方向を計算ベクター
static XMVECTOR CalculationVectorDirection(XMFLOAT3 rotate) {
	XMMATRIX mRotX = XMMatrixRotationX(XMConvertToRadians(rotate.x));
	XMMATRIX mRotY = XMMatrixRotationY(XMConvertToRadians(rotate.y));
	XMMATRIX mView = mRotX * mRotY;
	const XMVECTOR forwardVector = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	return XMVector3TransformNormal(forwardVector, mView);
}

//回転軸から向いている方向を計算XMFLOAT3
static XMFLOAT3 CalculationDirection(XMFLOAT3 rotate) {
	XMFLOAT3 dir;
	XMStoreFloat3(&dir, CalculationVectorDirection(rotate));
	return dir;
}