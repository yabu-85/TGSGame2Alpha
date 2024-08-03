#pragma once
#include <DirectXMath.h>
#include <string>
#include "Fbx.h"
#include "Transform.h"

//UEで言う、LookAtのデータ
struct OrientRotateInfo
{
	int boneIndex;			//ボーンのインデックス
	XMFLOAT3 orientRotate;	//回転量
	OrientRotateInfo() : boneIndex(-1), orientRotate(XMFLOAT3()) {}
};

//-----------------------------------------------------------
//3Dモデル（FBXファイル）を管理する
//-----------------------------------------------------------
namespace Model
{
	//モデル情報
	struct ModelData
	{
		//ファイル名
		std::string fileName;

		//ロードしたモデルデータのアドレス
		Fbx*		pFbx;

		//行列
		Transform 	transform;

		//アニメーションのフレーム
		float nowFrame, animSpeed;
		int startFrame, endFrame;

		std::vector<OrientRotateInfo> orientRotateDatas_;
		
		//初期化
		ModelData() : pFbx(nullptr), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0)
		{
		}

		//アニメーションのフレーム数をセット
		//引数：startFrame	開始フレーム
		//引数：endFrame	終了フレーム
		//引数：animSpeed	アニメーション速度
		void SetAnimFrame(int start, int end, float speed)
		{
			nowFrame = (float)start;
			startFrame = start;
			endFrame = end;
			animSpeed = speed;
		}
	};



	//初期化
	void Initialize();

	//モデルをロード
	//引数：fileName　ファイル名
	//戻値：そのモデルデータに割り当てられた番号
	int Load(std::string fileName);

	//描画
	//引数：handle	描画したいモデルの番号
	//引数：matrix	ワールド行列
	void Draw(int handle);

	//任意のモデルを開放
	//引数：handle	開放したいモデルの番号
	void Release(int handle);

	//全てのモデルを解放
	//（シーンが切り替わるときは必ず実行）
	void AllRelease();

	//アニメーションのフレーム数をセット
	//引数：handle		設定したいモデルの番号
	//引数：startFrame	開始フレーム
	//引数：endFrame	終了フレーム
	//引数：animSpeed	アニメーション速度
	void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed);

	//現在のアニメーションのフレームを取得
	int GetAnimFrame(int handle);

	//パーツとボーンのインデックス取得
	bool GetPartBoneIndex(int handle, std::string boneName, int* partIndex, int* boneIndex);

	//パーツのインデックス取得
	int GetPartIndex(int handle, std::string boneName);

	//ボーンのインデックス取得
	int GetBoneIndex(int handle, std::string boneName);

	//任意のボーンの位置を取得
	//引数：handle		調べたいモデルの番号
	//引数：boneName	調べたいボーンの名前
	//戻値：ボーンの位置（ワールド座標）
	XMFLOAT3 GetBonePosition(int handle, int partIndex, int boneIndex);

	//アニメーション時のボーンの位置を取得
	XMFLOAT3 GetBoneAnimPosition(int handle, int partIndex, int boneIndex);
	
	//アニメーション時のボーンの回転を取得
	XMFLOAT3 GetBoneAnimRotate(int handle, int partIndex, int boneIndex);

	//ワールド行列を設定
	//引数：handle	設定したいモデルの番号
	//引数：matrix	ワールド行列
	void SetTransform(int handle, Transform& transform);

	//ワールド行列の取得
	//引数：handle	知りたいモデルの番号
	//戻値：ワールド行列
	XMMATRIX GetMatrix(int handle);

	//レイキャスト（レイを飛ばして当たり判定）
	//引数：handle	判定したいモデルの番号
	//引数：data	必要なものをまとめたデータ
	void RayCast(int handle, RayCastData *data);

	//リストに追加
	int AddOrientRotateBone(int handle, std::string boneName);

	//リスト初期化
	void ResetOrientRotateBone(int handle);
	
	//回転軸セット
	void SetOrietnRotateBone(int handle, int listIndex, XMFLOAT3 rotate);

	//すべてのポリゴン取得
	void GetAllPolygon(int handle, std::vector<PolygonData>& list);

};