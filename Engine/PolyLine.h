#pragma once
#include <list>
#include "Direct3D.h"
#include "Texture.h"
#include <DirectXMath.h>
#include <string>
#include "BillBoard.h"

using namespace DirectX;
using namespace std;

class PolyLine
{
protected:
	int length_;			//����
	float width_;			//����
	float alpha_;			//�����x

	bool moveAlpha_;		//���X�ɓ����ɂ��Ă���
	bool first_;			//�ŏ��̃f�[�^��
	bool allClearReset_;	//���ׂď�������Clear����������

	ID3D11Buffer* pVertexBuffer_;	//���_�o�b�t�@
	ID3D11Buffer* pConstantBuffer_;	//�R���X�^���g�o�b�t�@
	Texture* pTexture_;	            //�摜

	list<XMFLOAT3> positions_;	    //�ߋ�length_�񕪂̈ʒu

	virtual void CalcPoly();
public:
	PolyLine();
	void Draw();
	virtual void Release();

	//�����FfileName	�摜�t�@�C����
	HRESULT Load(std::string fileName);
	
	//�S�Ẵ|�W�V���������Z�b�g
	virtual void ResetPosition();

	//��Ԍ��̃f�[�^������
	virtual void ClearLastPosition();

	//��ԍŏ��̃f�[�^������
	virtual void ClearFirstPosition();

	//���݂̈ʒu���L��������
	//�����Fpos	���݂̈ʒu
	void AddPosition(XMFLOAT3 pos);

	//���X�ɓ����ɂȂ�悤�ɐݒ�
	void SetMoveAlphaFlag() { moveAlpha_ = true; }
	void SetLength(int leng) { length_ = leng; }
	void SetWidth(float width) { width_ = width; }
};

class DoublePolyLine : public PolyLine {
	list<XMFLOAT3> positionsSub_;	//���W���w�肵���o�[�W�����Ŏg�p

	void CalcPoly() override;
public:
	DoublePolyLine();
	void Release() override;

	void ResetPosition() override;

	//��Ԍ��̃f�[�^������
	void ClearLastPosition() override;
	
	//��ԍŏ��̃f�[�^������
	void ClearFirstPosition() override;

	//���݂̈ʒu���L��������
	//�����Fpos1, pos2 �L��������ʒu
	void AddPosition(XMFLOAT3 pos1, XMFLOAT3 pos2);

};
