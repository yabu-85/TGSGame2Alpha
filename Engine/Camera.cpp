#include "camera.h"
#include "Direct3D.h"

XMFLOAT3 _position[2];
XMFLOAT3 _target[2];
XMMATRIX _view;
XMMATRIX _proj;
XMMATRIX _billBoard;

//�������i�v���W�F�N�V�����s��쐬�j
void Camera::Initialize()
{
	for (int i = 0; i < 2; i++) {
		_position[i] = XMFLOAT3(40, 10, 60);	//�J�����̈ʒu
		_target[i] = XMFLOAT3(50, 5, 50);	//�J�����̏œ_
	}

	//////////////////////�c�ɒׂ��̒����Ƃ�
	SetOneProjectionMatrix();
}

//�X�V�i�r���[�s��쐬�j
void Camera::Update(int id)
{
	//�r���[�s��
	_view = XMMatrixLookAtLH(XMVectorSet(_position[id].x, _position[id].y, _position[id].z, 0),
		XMVectorSet(_target[id].x, _target[id].y, _target[id].z, 0), XMVectorSet(0, 1, 0, 0));


	//�r���{�[�h�s��
	//�i��ɃJ�����̕��������悤�ɉ�]������s��B�p�[�e�B�N���ł����g��Ȃ��j
	//http://marupeke296.com/DXG_No11_ComeOnBillboard.html
	_billBoard = XMMatrixLookAtLH(XMVectorSet(0, 0, 0, 0), XMLoadFloat3(&_target[id])- XMLoadFloat3(&_position[id]), XMVectorSet(0, 1, 0, 0));
	_billBoard = XMMatrixInverse(nullptr, _billBoard);
}

//�œ_��ݒ�
void Camera::SetTarget(XMFLOAT3 target, int id) { _target[id] = target; }

//�ʒu��ݒ�
void Camera::SetPosition(XMFLOAT3 position, int id) { _position[id] = position; }

//�œ_���擾
XMFLOAT3 Camera::GetTarget(int id) { return _target[id]; }

//�ʒu���擾
XMFLOAT3 Camera::GetPosition(int id) { return _position[id]; }

//�r���[�s����擾
XMMATRIX Camera::GetViewMatrix() { return _view; }

//�v���W�F�N�V�����s����擾
XMMATRIX Camera::GetProjectionMatrix() { return _proj; }

void Camera::SetOneProjectionMatrix()
{
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, ((FLOAT)Direct3D::screenWidth_) / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);
}

void Camera::SetTwoProjectionMatrix()
{
	_proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, ((FLOAT)Direct3D::screenWidth_ / 2.0f) / (FLOAT)Direct3D::screenHeight_, 0.1f, 1000.0f);
}

//�r���{�[�h�p��]�s����擾
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