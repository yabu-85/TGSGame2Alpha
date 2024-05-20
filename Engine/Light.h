#pragma once
#include <DirectXMath.h>
using namespace DirectX;

//定数
static const int LIGHT_TOTAL_NUM = 5;	//ライトの総数  

namespace Light
{
	void Initialize();
	void Draw();
	void SetPosition(int i, XMFLOAT4 pos);
	XMFLOAT4 GetPosition(int i);

	/// <summary>
	/// Playerのライトの位置を設定
	/// </summary>
	/// <param name="position">設定したい位置</param>
	void SetPlayerPosition(XMFLOAT4 position);

	/// <summary>
	/// ライトの位置を設定
	/// </summary>
	/// <param name="position">設定したい位置</param>
	/// <param name="intensity">設定したい強さ</param>
	/// <returns>ライトの番号</returns>
	int CreateLight(XMFLOAT4 position, float intensity);

	/// <summary>
	/// 距離によってどのように明るさを変化させるか(減衰パラメータ)を設定
	/// </summary>
	/// <param name="attenuation">設定したい減衰パラメータ</param>
	void SetAttenuation(XMFLOAT4 attenuation);

	/// <summary>
	/// ライトの向きを設定
	/// </summary>
	/// <param name="direction">設定したいライトの向き</param>
	void SetDirection(XMFLOAT4 direction);

	/// <summary>
	/// Playerのライトの強さを設定
	/// </summary>
	/// <param name="intensity">設定したいライトの強さ</param>
	void SetPlayerIntensity(float intensity);

	/// <summary>
	/// ライトの強さを設定
	/// </summary>
	/// <param name="num">ライトの番号</param>
	/// <param name="intensity">ライトの強さ</param>
	void SetIntensity(int num, float intensity);

	/// <summary>
	/// ライトの削除
	/// </summary>
	/// <param name="num">ライトの番号</param>
	void DeleteLight(int num);

	/// <summary>
	/// 位置を取得
	/// </summary>
	/// <param name="num">ライトの番号</param>
	/// <returns>位置</returns>
	XMFLOAT4 GetPosition(int num);

	/// <summary>
	/// 減衰パラメータを取得
	/// </summary>
	/// <returns>減衰パラメータ</returns>
	XMFLOAT4 GetAttenuation();

	/// <summary>
	/// ライトの向きを取得
	/// </summary>
	/// <returns>ライトの向き</returns>
	XMFLOAT4 GetDirection();

	/// <summary>
	/// ライトの強さを取得
	/// </summary>
	/// <param name="num">ライトの番号</param>
	/// <returns>ライトの強さ</returns>
	float  GetIntensity(int num);
};

