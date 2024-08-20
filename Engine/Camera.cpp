#include "camera.h"
#include "Direct3D.h"

XMFLOAT3 _position[2];
XMFLOAT3 _target[2];
XMMATRIX _view;
XMMATRIX _proj;
XMMATRIX _billBoard;

//初期化（プロジェクション行列作成）
void Camera::Initialize()
{
	for (int i = 0; i < 2; i++) {
		_position[i] = XMFLOAT3(40, 10, 60);	//カメラの位置
		_target[i] = XMFLOAT3(50, 5, 50);	//カメラの焦点
	}

	//////////////////////縦に潰れるの直すとこ
	SetOneProjectionMatrix();
}

//更新（ビュー行列作成）
void Camera::Update(int id)
{
	//ビュー行列
	_view = XMMatrixLookAtLH(XMVectorSet(_position[id].x, _position[id].y, _position[id].z, 0),
		XMVectorSet(_target[id].x, _target[id].y, _target[id].z, 0), XMVectorSet(0, 1, 0, 0));


	//ビルボード行列
	//（常にカメラの方を向くように回転させる行列。パーティクルでしか使わない）
	//http://marupeke296.com/DXG_No11_ComeOnBillboard.html
	_billBoard = XMMatrixLookAtLH(XMVectorSet(0, 0, 0, 0), XMLoadFloat3(&_target[id])- XMLoadFloat3(&_position[id]), XMVectorSet(0, 1, 0, 0));
	_billBoard = XMMatrixInverse(nullptr, _billBoard);
}

//焦点を設定
void Camera::SetTarget(XMFLOAT3 target, int id) { _target[id] = target; }

//位置を設定
void Camera::SetPosition(XMFLOAT3 position, int id) { _position[id] = position; }

//焦点を取得
XMFLOAT3 Camera::GetTarget(int id) { return _target[id]; }

//位置を取得
XMFLOAT3 Camera::GetPosition(int id) { return _position[id]; }

//ビュー行列を取得
XMMATRIX Camera::GetViewMatrix() { return _view; }

//プロジェクション行列を取得
XMMATRIX Camera::GetProjectionMatrix() { return _proj; }

void Camera::SetOneProjectionMatrix()
{
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, ((FLOAT)Direct3D::screenWidth_) / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);
}

void Camera::SetTwoProjectionMatrix()
{
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, ((FLOAT)Direct3D::screenWidth_ / 2.0f) / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);
}

//ビルボード用回転行列を取得
XMMATRIX Camera::GetBillboardMatrix(){	return _billBoard; }

bool Camera::IsWorldPositionWithinScreen(XMFLOAT3 pos, float size)
{
	pos = CalcScreenPosition(pos);
	if (pos.x >= size || pos.y >= size || pos.x <= -size || pos.y <= -size || pos.z >= 1.0f) return false;
	return true;
}

bool Camera::IsScreenPositionWithinScreen(XMFLOAT3 pos, float size)
{
	if (pos.x >= size || pos.y >= size || pos.x <= -size || pos.y <= -size || pos.z >= 1.0f) return false;
	return true;
}

XMFLOAT3 Camera::CalcScreenPosition(XMFLOAT3 pos)
{
	XMVECTOR v2 = XMVector3TransformCoord(XMLoadFloat3(&pos), Camera::GetViewMatrix());
	v2 = XMVector3TransformCoord(v2, Camera::GetProjectionMatrix());
	pos.x = XMVectorGetX(v2);
	pos.y = XMVectorGetY(v2);
	pos.z = XMVectorGetZ(v2);
	return pos;
}