#include "camera.h"
#include "Direct3D.h"

XMFLOAT3 _position;
XMFLOAT3 _target;
XMMATRIX _view;
XMMATRIX _proj;
XMMATRIX _billBoard;

//////////二つ目のウィンドウのカメラに必要な変数
XMFLOAT3 _twoWindowPosition2;
XMFLOAT3 _twoWindowTarget2;

//初期化（プロジェクション行列作成）
void Camera::Initialize()
{
	_position = XMFLOAT3(0, 3, -10);	//カメラの位置
	_target = XMFLOAT3( 0, 0, 0);	//カメラの焦点

	_twoWindowPosition2 = XMFLOAT3(0, 10, -10);
	_twoWindowTarget2 = XMFLOAT3(0, 0, 0);

	//プロジェクション行列
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)Direct3D::screenWidth_ / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);

	/*
	lightViewMatrix = XMMatrixLookAtLH(
		XMVectorSet(50, 760, -50, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	lightPrjMatrix_ = XMMatrixPerspectiveFovLH(XM_PIDIV4, (FLOAT)screenWidth / (FLOAT)screenHeight, 330, 800);
	*/
	
}

//更新（ビュー行列作成）
void Camera::Update()
{
	//ビュー行列
	_view = XMMatrixLookAtLH(XMVectorSet(_position.x, _position.y, _position.z, 0),
		XMVectorSet(_target.x, _target.y, _target.z, 0), XMVectorSet(0, 1, 0, 0));


	//ビルボード行列
	//（常にカメラの方を向くように回転させる行列。パーティクルでしか使わない）
	//http://marupeke296.com/DXG_No11_ComeOnBillboard.html
	_billBoard = XMMatrixLookAtLH(XMVectorSet(0, 0, 0, 0), XMLoadFloat3(&_target)- XMLoadFloat3(&_position), XMVectorSet(0, 1, 0, 0));
	_billBoard = XMMatrixInverse(nullptr, _billBoard);
}

void Camera::TwoWindowUpdate()
{
	//ビュー行列
	_view = XMMatrixLookAtLH(XMVectorSet(_twoWindowPosition2.x, _twoWindowPosition2.y, _twoWindowPosition2.z, 0),
		XMVectorSet(_twoWindowTarget2.x, _twoWindowTarget2.y, _twoWindowTarget2.z, 0), XMVectorSet(0, 1, 0, 0));

}

//焦点を設定
void Camera::SetTarget(XMFLOAT3 target) { _target = target; }
void Camera::SetTarget2(XMFLOAT3 target) { _twoWindowTarget2 = target; }

//位置を設定
void Camera::SetPosition(XMFLOAT3 position) { _position = position; }
void Camera::SetPosition2(XMFLOAT3 position) { _twoWindowPosition2 = position; }

//焦点を取得
XMFLOAT3 Camera::GetTarget() { return _target; }

//位置を取得
XMFLOAT3 Camera::GetPosition() { return _position; }

//ビュー行列を取得
XMMATRIX Camera::GetViewMatrix() { return _view; }

//プロジェクション行列を取得
XMMATRIX Camera::GetProjectionMatrix() { return _proj; }

//ビルボード用回転行列を取得
XMMATRIX Camera::GetBillboardMatrix(){	return _billBoard; }
