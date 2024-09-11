#pragma once
#include "GameObject.h"

//-----------------------------------------------------------
//カメラ
//-----------------------------------------------------------
namespace Camera
{
	//初期化（プロジェクション行列作成）
	void Initialize();

	//更新（ビュー行列作成）
	void Update(int id);

	//視点（カメラの位置）を設定
	void SetPosition(XMFLOAT3 position, int id);

	//焦点（見る位置）を設定
	void SetTarget(XMFLOAT3 target, int id);

	//視野角Parcent（0 ～ 1 低ければ低い程視野角が低い）
	void SetFovAngleParcent(float zoom, int id);

	//視野角Parcent取得
	float GetFovAngleParcent(int id);

	//位置を取得
	XMFLOAT3 GetPosition(int id);

	//焦点を取得
	XMFLOAT3 GetTarget(int id);

	//ビュー行列を取得
	XMMATRIX GetViewMatrix();

	//プロジェクション行列を取得
	XMMATRIX GetProjectionMatrix();

	//一人用Projに設定
	void SetOneProjectionMatrix(float fovParcent = 1.0f);

	//二人用Projに設定
	void SetTwoProjectionMatrix(float fovParcent = 1.0f);

	//ビルボード用回転行列を取得
	XMMATRIX GetBillboardMatrix();

	//引数のワールド座標が画面内かどうか
	bool IsWorldPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//引数のスクリーン座標が画面内かどうか
	bool IsScreenPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//画面のどこのポジションかどうか計算
	XMFLOAT3 CalcScreenPosition(XMFLOAT3 pos);

};