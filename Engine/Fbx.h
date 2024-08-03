#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <vector>
#include <string>
#include "Transform.h"

class FbxParts;
struct OrientRotateInfo;

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
	void    Draw(Transform& transform, int frame, std::vector<OrientRotateInfo> &orientDatas);

	//解放
	void    Release();

	//ボーンのインデックス取得
	bool GetPartBoneIndex(std::string boneName, int* partIndex, int* boneIndex);

	//任意のボーンの位置を取得
	//引数：boneName	取得したいボーンの位置
	//戻値：ボーンの位置
	XMFLOAT3 GetBonePosition(int partIndex, int boneIndex);

	//任意のアニメーション時のボーンの位置を取得
	XMFLOAT3 GetBoneAnimPosition(int partIndex, int boneIndex, int frame);

	//任意のアニメーション時のボーンの回転を取得
	XMFLOAT3 GetBoneAnimRotate(int partIndex, int boneIndex, int frame);

	//レイキャスト（レイを飛ばして当たり判定）
	//引数：data	必要なものをまとめたデータ
	void RayCast(RayCastData *data);

	//すべてのポリゴン取得
	void GetAllPolygon(std::vector<PolygonData>& list);

private:
	//すべてのポリゴン取得計算用関数
	void GetAllPolygonRecursive(FbxNode* pNode, std::vector<PolygonData>& list);

};

