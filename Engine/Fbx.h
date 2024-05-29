#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <vector>
#include <string>
#include "Transform.h"

class FbxParts;

//レイキャスト用構造体
struct RayCastData
{
	XMFLOAT3	start;	//レイ発射位置
	XMFLOAT3	dir;	//レイの向きベクトル
	float       dist;	//衝突点までの距離
	BOOL        hit;	//レイが当たったか
	XMFLOAT3 normal;	//法線

	RayCastData() { dist = 99999.0f; start = XMFLOAT3(); dir = XMFLOAT3(); hit = FALSE; normal = XMFLOAT3(); }
};

struct PolygonData {
	XMFLOAT3 position_[3];
	XMFLOAT3 normal;
	PolygonData() : position_{ XMFLOAT3(), XMFLOAT3(), XMFLOAT3() }, normal(XMFLOAT3()) {}
	PolygonData(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 nor) : position_{ p1, p2, p3 }, normal(XMFLOAT3()) { }
};

//-----------------------------------------------------------
//　FBXファイルを扱うクラス
//　ほとんどの処理は各パーツごとにFbxPartsクラスで行う
//-----------------------------------------------------------
class Fbx
{
	//FbxPartクラスをフレンドクラスにする
	//FbxPartのprivateな関数にもアクセス可
	friend class FbxParts;


	//モデルの各パーツ（複数あるかも）
	std::vector<FbxParts*>	parts_;

	//FBXファイルを扱う機能の本体
	FbxManager* pFbxManager_;

	//FBXファイルのシーン（Mayaで作ったすべての物体）を扱う
	FbxScene*	pFbxScene_;

	// アニメーションのフレームレート
	FbxTime::EMode	_frameRate;

	//アニメーション速度
	float			_animSpeed;

	//アニメーションの最初と最後のフレーム
	int _startFrame, _endFrame;


	//ノードの中身を調べる
	//引数：pNode		調べるノード
	//引数：pPartsList	パーツのリスト
	void CheckNode(FbxNode* pNode, std::vector<FbxParts*> *pPartsList);


public:
	Fbx();
	~Fbx();

	//ロード
	//引数：fileName	ファイル名
	//戻値：成功したかどうか
	virtual HRESULT Load(std::string fileName);

	//描画
	//引数：World	ワールド行列
	void    Draw(Transform& transform, int frame);

	//解放
	void    Release();

	//任意のボーンの位置を取得
	//引数：boneName	取得したいボーンの位置
	//戻値：ボーンの位置
	XMFLOAT3 GetBonePosition(std::string boneName);

	//レイキャスト（レイを飛ばして当たり判定）
	//引数：data	必要なものをまとめたデータ
	void RayCast(RayCastData *data);

	void GetAllPolygon(std::vector<PolygonData>& list);
	void GetAllPolygonRecursive(FbxNode* pNode, std::vector<PolygonData>& list);

};

