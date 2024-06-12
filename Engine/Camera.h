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
	void Update();

	//更新（ビュー行列作成）
	void TwoWindowUpdate();

	//視点（カメラの位置）を設定
	void SetPosition(XMFLOAT3 position);
	void SetPosition2(XMFLOAT3 position);

	//焦点（見る位置）を設定
	void SetTarget(XMFLOAT3 target);
	void SetTarget2(XMFLOAT3 target);

	//位置を取得
	XMFLOAT3 GetPosition();

	//焦点を取得
	XMFLOAT3 GetTarget();

	//ビュー行列を取得
	XMMATRIX GetViewMatrix();

	//プロジェクション行列を取得
	XMMATRIX GetProjectionMatrix();

	//ビルボード用回転行列を取得
	XMMATRIX GetBillboardMatrix();

	//引数のワールド座標が画面内かどうか
	bool IsWorldPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//引数のスクリーン座標が画面内かどうか
	bool IsScreenPositionWithinScreen(XMFLOAT3 pos, float size = 1.0f);

	//画面のどこのポジションかどうか計算
	XMFLOAT3 CalcScreenPosition(XMFLOAT3 pos);

};