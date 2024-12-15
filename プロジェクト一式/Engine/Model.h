#pragma once
#include <DirectXMath.h>
#include <string>
#include "Fbx.h"
#include "Transform.h"

class CapsuleCollider;

//インスタンスごとに持つボーン情報
struct BoneInstanceData {
	XMMATRIX  newPose;       // アニメーションで変化したときのボーン変換行列
	XMMATRIX  diffPose;      // mBindPose に対する mNowPose の変化量
};

//UEで言う、LookAtのデータ
struct OrientRotateInfo
{
	int boneIndex;			//ボーンのインデックス
	int parentBoneIndex;	//親ボーンのインデックス（Rootの場合-1）
	XMFLOAT3 orientRotate;	//回転量
	OrientRotateInfo() : boneIndex(-1), parentBoneIndex(-1), orientRotate(XMFLOAT3()) {}

	//回転行列の取得（引数はDegreesの値）
	XMMATRIX GetRotationMatrix() const {
		return
			XMMatrixRotationX(XMConvertToRadians(orientRotate.x)) *
			XMMatrixRotationY(XMConvertToRadians(orientRotate.y)) *
			XMMatrixRotationZ(XMConvertToRadians(orientRotate.z));
	}
};

//アニメーションブレンドの情報
struct BlendData {
	bool animLoop;
	int startFrame;
	int endFrame;
	float nowFrame;
	float animSpeed;
	float currentBlend;		//今のBlendの値（1～0）
	float decreaseBlend;	//1フレームでブレンド値減らす量

	BlendData() : animLoop(false), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0), currentBlend(0.0f), decreaseBlend(0.0f) {}
};

//指定したボーンにコライダーを取り付ける（今のところカプセルのみ対応）
struct BoneColliderData {
	int partIndex;
	int boneIndex;
	XMFLOAT3 offsetPosition;    //ボーンからのオフセット位置
	XMFLOAT3 offsetRotation;    //ボーンからのオフセット回転
	CapsuleCollider* pCollider;		//コライダーのポインタ

	XMMATRIX preCalculatedRotationMatrix;   // 事前計算した回転行列
	XMMATRIX preCalculatedOffsetRotationMatrix; // 事前計算したオフセット回転行列

	BoneColliderData() : partIndex(-1), boneIndex(-1), offsetPosition(XMFLOAT3()), offsetRotation(XMFLOAT3()), pCollider(nullptr) {}
};

//Fbxに送る用のデータ
struct FbxBlendData {
	FbxTime time;
	float factor;
	FbxBlendData() : factor(0.0f) {}
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

		std::vector<OrientRotateInfo> orientRotateDatas_;	//ボーンの回転情報
		std::vector<BlendData> blendDatas_;					//ブレンド情報
		std::vector<FbxBlendData> fbxBlendDatas_;			//Fbx情報
		std::vector<BoneColliderData> boneColliders_;		//コライダー情報

		//インスタンスごとに持つボーンデータ
		BoneInstanceData* pBoneInstanceData;

		bool isAnimStop;	//アニメーション再生するかどうか
		bool isShadow;		//影適応するかどうか
		bool isAnimLoop;	//アニメーションループするかどうか
		bool isBlending;	//ブレンドするかどうか
		bool isCalcDraw;	//ClacDrawする必要があるかどうか

		//初期化
		ModelData() 
			: pFbx(nullptr), pBoneInstanceData(nullptr),
			nowFrame(0), startFrame(0), endFrame(0), animSpeed(0), isAnimStop(false), isShadow(true), isAnimLoop(true), isBlending(false), isCalcDraw(false)
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

	//モデルのUpdate
	//今はアニメーションの時間を進めるのに使う
	void Update(int handle);

	//ボーン有の描画前計算
	void CalcDraw(int handle);

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

	//アニメーションを再生
	//引数：handle	開放したいモデルの番号
	void AnimStart(int handle);

	//アニメーションを止める
	//引数：handle	開放したいモデルの番号
	void AnimStop(int handle);

	//アニメーションループ再生かどうかセット
	void SetAnimLoop(int handle, bool b);

	//ブレンドするか設定する
	void SetBlend(int handle, bool b);

	/// <summary>
	/// ブレンド情報の追加
	/// </summary>
	/// <param name="factor">ブレンド値</param>
	/// <param name="decrease">1フレームでブレンド値減らす量</param>
	/// <param name="nowFrame">入力無い場合Startが再生開始Frameになる</param>
	void AddBlend(int handle, int start, int end, float speed, bool loop, float factor, float decrease, int nowFrame = -1);

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

	//現在のボーン位置を取得
	XMFLOAT3 GetBoneAnimPositionAtNow(int handle, int partIndex, int boneIndex);

	//指定したアニメーション時間のボーンの位置を取得
	XMFLOAT3 GetBoneAnimPosition(int handle, int partIndex, int boneIndex, int frame);

	//現在のボーンの回転を取得
	XMFLOAT3 GetBoneAnimRotateAtNow(int handle, int partIndex, int boneIndex);

	//指定したアニメーション時間の回転を取得
	XMFLOAT3 GetBoneAnimRotate(int handle, int partIndex, int boneIndex, int frame);

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

	//リストに追加（Rootの場合ParentBoneは入力しない）
	int AddOrientRotateBone(int handle, std::string boneName, std::string parentBoneName = "");

	//リスト初期化
	void ResetOrientRotateBone(int handle);
	
	//回転軸セット
	void SetOrietnRotateBone(int handle, int listIndex, XMFLOAT3 rotate);

	/// <summary>
	/// コライダーを指定したボーンに取り付ける
	/// </summary>
	void AttachColliderToBone(int handle, CapsuleCollider* pCollider, std::string boneName, XMFLOAT3 position = XMFLOAT3(), XMFLOAT3 rotation = XMFLOAT3());

	//影表示するかセット
	void SetShadow(int handle, bool b);

	//すべてのポリゴン取得
	void GetAllPolygon(int handle, std::vector<PolygonData>& list);

};