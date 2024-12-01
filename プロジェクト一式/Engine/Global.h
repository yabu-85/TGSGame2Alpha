#pragma once
#include "Direct3D.h"
#include <string>

//安全にメモリを開放するためのマクロ
#define SAFE_DELETE(p) {if ((p)!=nullptr) { delete (p); (p)=nullptr;}}
#define SAFE_DELETE_ARRAY(p) {if ((p)!=nullptr) { delete[] (p); (p)=nullptr;}}
#define SAFE_RELEASE(p) {if ((p)!=nullptr) { p->Release(); (p)=nullptr;}}

//XMFLOAT3同士の足し算
XMFLOAT3 Float3Add(XMFLOAT3 a, XMFLOAT3 b);

//XMFLOAT3同士の引き算
XMFLOAT3 Float3Sub(XMFLOAT3 a, XMFLOAT3 b);

//XMFLOAT3同士の掛け算
XMFLOAT3 Float3Multiply(XMFLOAT3 a, XMFLOAT3 b);

//XMFLOAT3とfloatの掛け算
XMFLOAT3 Float3Multiply(XMFLOAT3 a, float scalar);

//XMFLOAT3を正規化
XMFLOAT3 Float3Normalize(XMFLOAT3 a);

//距離を計算
float CalculationDistance(XMFLOAT3 dir);

//距離を計算
float CalculationDistance(XMFLOAT3 a, XMFLOAT3 b);

//ベクトルから回転軸を計算（度）
XMFLOAT3 CalculationRotateXY(XMFLOAT3 dir);

//ベクトルから回転軸を計算（度）
float CalculationRotateZ(XMFLOAT3 dir);

XMFLOAT3 CalculationRotateXYZ(XMFLOAT3 dir);

//回転軸から向いている方向を計算ベクター
XMVECTOR CalculationVectorDirection(XMFLOAT3 rotate);

//回転軸から向いている方向を計算ベクター
XMVECTOR CalculationVectorDirection(XMVECTOR rotate);

//回転軸から向いている方向を計算XMFLOAT3
XMFLOAT3 CalculationDirection(XMFLOAT3 rotate);

//OutPutDebugStringで配列の値表示
template <typename T>
void OutPutString(std::string firstText, T* array, int size, std::string endText = "");

//OutPutDebugStringで値の表示
void OutPutString(std::string firstText, XMFLOAT3 text, std::string endText = "");