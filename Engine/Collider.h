#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

//クラスの前方宣言
class GameObject;
class BoxCollider;
class SphereCollider;
class CapsuleCollider;

//カプセルの当たり判定ように置いておく
class Triangle;

//あたり判定のタイプ
enum ColliderType
{
	COLLIDER_BOX,		//箱型
	COLLIDER_CIRCLE,	//球体
	COLLIDER_CAPSULE,	//カプセル
};

//-----------------------------------------------------------
//あたり判定を管理するクラス
//-----------------------------------------------------------
class Collider
{
	//それぞれのクラスのprivateメンバにアクセスできるようにする
	friend class BoxCollider;
	friend class SphereCollider;
	friend class CapsuleCollider;

protected:
public:
	GameObject*		pGameObject_;	//この判定をつけたゲームオブジェクト
	ColliderType	type_;			//種類
	XMFLOAT3		center_;		//中心位置（ゲームオブジェクトの原点から見た位置）
	XMFLOAT3		size_;			//判定サイズ（幅、高さ、奥行き）
	int				hDebugModel_;	//デバッグ表示用のモデルのID

public:
	//コンストラクタ
	Collider();

	//デストラクタ
	virtual ~Collider();

	ColliderType GetColliderType();

	//接触判定（継承先のSphereColliderかBoxColliderでオーバーライド）
	//引数：target	相手の当たり判定
	//戻値：接触してればtrue
	virtual bool IsHit(Collider* target) = 0;

	//箱型同士の衝突判定
	//引数：boxA	１つ目の箱型判定
	//引数：boxB	２つ目の箱型判定
	//戻値：接触していればtrue
	bool IsHitBoxVsBox(BoxCollider* boxA, BoxCollider* boxB);

	//箱型と球体の衝突判定
	//引数：box	箱型判定
	//引数：sphere	２つ目の箱型判定
	//戻値：接触していればtrue
	bool IsHitBoxVsCircle(BoxCollider* box, SphereCollider* sphere);

	//箱型と球体の衝突判定
	//引数：box	箱型判定
	//引数：sphere	２つ目の箱型判定
	//戻値：接触していればtrue
	bool IsHitBoxVsCapsule(BoxCollider* box, CapsuleCollider* capsule);

	//球体同士の衝突判定
	//引数：circleA	１つ目の球体判定
	//引数：circleB	２つ目の球体判定
	//戻値：接触していればtrue
	bool IsHitCircleVsCircle(SphereCollider* circleA, SphereCollider* circleB);

	//テスト表示用の枠を描画
	//引数：position	オブジェクトの位置
	virtual void Draw(XMFLOAT3 position);

	//セッター
	void SetGameObject(GameObject* gameObject) { pGameObject_ = gameObject; }

	//カプセル
	bool IsHitCapsuleVsCapsule(CapsuleCollider* capsule1, CapsuleCollider* capsule2);
	bool IsHitCircleVsCapsule(SphereCollider* circle, CapsuleCollider* capsule);
	
	//ポリゴンと
	bool IsHitCircleVsTriangle(SphereCollider* circle, Triangle* triangle, XMVECTOR& outDistanceVector);
    


};

