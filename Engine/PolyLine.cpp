#include "Global.h"
#include "Camera.h"
#include "PolyLine.h"
#include "Direct3D.h"
#include <vector>

PolyLine::PolyLine() : width_(0.3f), length_(50), alpha_(1.0f), moveAlpha_(false), clear_(false), allClearReset_(false), first_(true),
	pVertexBuffer_(nullptr), pConstantBuffer_(nullptr), pTexture_(nullptr)
{
	ResetPosition();
}

void PolyLine::ResetPosition()
{
	//���X�g�̐擪�Ɍ��݈ʒu��ǉ�
	positions_.clear();
	positions_.push_back(XMFLOAT3(0, 0, 0));
	first_ = true;

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX;

	//���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX);
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete vertices;
}

void PolyLine::ClearLastPositions()
{
	if (first_) return;
	
	//��납�璸�_�����Ă�
	positions_.pop_front();

	//�����Ȃ��ƃo�O��̂�1�ǉ�
	if (positions_.empty()) {
		positions_.push_back(XMFLOAT3(0, 0, 0));
		first_ = true;
	}

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX[length_ * 2];

	//�����x�N�g��
	XMVECTOR vPVec = XMVectorSet(0, 1, 0, 0);

	//���_�f�[�^�����
	int index = 0;
	auto itr = positions_.begin();
	for (int i = 0; i < length_; i++)
	{
		//�L�����Ă��ʒu
		XMVECTOR vPos = XMLoadFloat3(&(*itr));

		itr++;
		if (itr == positions_.end()) break;

		//�L�����Ă��ʒu����A���̎��ɋL�����Ă��ʒu�Ɍ������x�N�g��
		XMVECTOR vLine = XMLoadFloat3(&(*itr)) - vPos;

		//�����ƃ��C���ɐ����ȃx�N�g��
		XMVECTOR vArm = XMVector3Cross(vLine, vPVec);
		vArm = XMVector3Normalize(vArm) * width_;

		//���_�������Ă���
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, vPos + vArm);
		VERTEX vertex1 = { pos, XMFLOAT3((float)i / length_, 0, 0) };

		XMStoreFloat3(&pos, vPos - vArm);
		VERTEX vertex2 = { pos, XMFLOAT3((float)i / length_, 1, 0) };

		vertices[index] = vertex1;
		index++;
		vertices[index] = vertex2;
		index++;
	}

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * length_ * 2;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete[] vertices;
}

void PolyLine::SetClear(bool allClear)
{
	clear_ = true;
	allClearReset_ = allClear;
}

void PolyLine::ClearCancel()
{
	clear_ = false;
}

void PolyLine::AddPosition(XMFLOAT3 pos)
{
	if (first_) {
		positions_.clear();
		first_ = false;
	}

	positions_.push_back(pos);

	//�w��̒����𒴂��Ă���I�[�̃f�[�^���폜
	if (positions_.size() > length_)
	{
		positions_.pop_front();
	}

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//�x�N�g�������߂�
	XMVECTOR vPVec = XMVectorSet(0, 1, 0, 0);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX[length_ * 2];

	//���_�f�[�^�����
	int index = 0;
	auto itr = positions_.begin();
	for (int i = 0; i < length_; i++)
	{
		//�L�����Ă��ʒu
		XMVECTOR vPos = XMLoadFloat3(&(*itr));

		itr++;
		if (itr == positions_.end()) break;

		//�L�����Ă��ʒu����A���̎��ɋL�����Ă��ʒu�Ɍ������x�N�g��
		XMVECTOR vLine = XMLoadFloat3(&(*itr)) - vPos;

		//�����ƃ��C���ɐ����ȃx�N�g��
		XMVECTOR vArm = XMVector3Cross(vLine, vPVec);
		vArm = XMVector3Normalize(vArm) * width_;

		//���_�������Ă���
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, vPos + vArm);
		VERTEX vertex1 = { pos, XMFLOAT3((float)i / length_, 0, 0) };

		XMStoreFloat3(&pos, vPos - vArm);
		VERTEX vertex2 = { pos, XMFLOAT3((float)i / length_, 1, 0) };

		vertices[index] = vertex1;
		index++;
		vertices[index] = vertex2;
		index++;
	}

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * length_ * 2;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete[] vertices;
}

HRESULT PolyLine::Load(std::string fileName)
{
	//�R���X�^���g�o�b�t�@�쐬
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;

	// �R���X�^���g�o�b�t�@�̍쐬
	Direct3D::pDevice_->CreateBuffer(&cb, nullptr, &pConstantBuffer_);


	//�e�N�X�`��
	pTexture_ = new Texture;
	pTexture_->Load(fileName.c_str());

	return S_OK;
}

void PolyLine::Draw()
{
	Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
	Direct3D::SetShader(Direct3D::SHADER_BILLBOARD);
	Direct3D::SetBlendMode(Direct3D::BLEND_ADD);

	if (moveAlpha_) alpha_ -= 0.005f;
	
	if (clear_) {
		ClearLastPositions();
		if (first_ && allClearReset_) {
			clear_ = false;
			allClearReset_ = false;
		}
	}

	//�R���X�^���g�o�b�t�@�ɓn�����
	CONSTANT_BUFFER cb;
	cb.matWVP = XMMatrixTranspose(Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
	cb.color = XMFLOAT4(1, 1, 1, alpha_);

	D3D11_MAPPED_SUBRESOURCE pdata;
	Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
	memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

	ID3D11SamplerState* pSampler = pTexture_->GetSampler();
	Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

	ID3D11ShaderResourceView* pSRV = pTexture_->GetSRV();
	Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);

	Direct3D::pContext_->Unmap(pConstantBuffer_, 0);	//�ĊJ

	//���_�o�b�t�@
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//�R���X�^���g�o�b�t�@
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);	//���_�V�F�[�_�[�p	
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);	//�s�N�Z���V�F�[�_�[�p

	//���_�f�[�^�̕��ѕ����w��
	Direct3D::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//�`��i�C���f�b�N�X�o�b�t�@����Ȃ��^�C�v�j
	Direct3D::pContext_->Draw(((int)positions_.size() - 1) * 2, 0);

	//���_�f�[�^�̕��ѕ����w���߂�
	Direct3D::pContext_->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);
	Direct3D::SetShader(type);

}

void PolyLine::Release()
{
	SAFE_DELETE(pTexture_);
	SAFE_RELEASE(pConstantBuffer_);
	SAFE_RELEASE(pVertexBuffer_);
	positions_.clear();
}

//--------------------------------------------------------------

DoublePolyLine::DoublePolyLine()
{
	ResetPosition();
}

void DoublePolyLine::Release()
{
	PolyLine::Release();
	positionsSub_.clear();
}

void DoublePolyLine::ResetPosition()
{
	//���X�g�̐擪�Ɍ��݈ʒu��ǉ�
	positions_.clear();
	positionsSub_.clear();
	positions_.push_front(XMFLOAT3(0, 0, 0));
	positionsSub_.push_front(XMFLOAT3(0, 0, 0));
	first_ = true;

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX;

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX);
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete vertices;
}

void DoublePolyLine::ClearLastPositions()
{
	//first�Ȃ�I���
	if (first_) return;

	//�f�[�^�̌�납������Ă�
	positions_.pop_back();
	positionsSub_.pop_back();

	
	if (positions_.empty())
	{
		positions_.push_front(XMFLOAT3(0, 0, 0));
		positionsSub_.push_front(XMFLOAT3(0, 0, 0));
		first_ = true;
	}

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX[length_ * 2];

	//���_�f�[�^�����
	int index = 0;
	auto itr = positions_.begin();
	auto itrSub = positionsSub_.begin();
	for (int i = 0; i < length_; i++)
	{
		//�L�����Ă��ʒu�擾
		XMVECTOR vPos1 = XMLoadFloat3(&(*itr));
		XMVECTOR vPos2 = XMLoadFloat3(&(*itrSub));

		itr++;
		itrSub++;
		if (itr == positions_.end()) break;

		//���_�������Ă���
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, vPos1);
		VERTEX vertex1 = { pos, XMFLOAT3((float)i / (float)length_, 1, 0) };

		XMStoreFloat3(&pos, vPos2);
		VERTEX vertex2 = { pos, XMFLOAT3((float)i / (float)length_, 0, 0) };

		vertices[index] = vertex1;
		index++;
		vertices[index] = vertex2;
		index++;
	}

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * length_ * 2;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete[] vertices;
}

void DoublePolyLine::AddPosition(XMFLOAT3 pos1, XMFLOAT3 pos2)
{
	if (first_) {
		positions_.clear();
		positionsSub_.clear();
		first_ = false;
	}

	positions_.push_front(pos1);
	positionsSub_.push_front(pos2);

	//�w��̒����𒴂��Ă���I�[�̃f�[�^���폜
	if (positions_.size() > length_)
	{
		positions_.pop_back();
		positionsSub_.pop_back();
	}

	//���_�o�b�t�@���N���A�i�������邩��j
	SAFE_RELEASE(pVertexBuffer_);

	//���_�f�[�^����邽�߂̔z�������
	VERTEX* vertices = new VERTEX[length_ * 2];

	//���_�f�[�^�����
	int index = 0;
	auto itr = positions_.begin();
	auto itrSub = positionsSub_.begin();
	for (int i = 0; i < length_; i++)
	{
		//�L�����Ă��ʒu�擾
		XMVECTOR vPos1 = XMLoadFloat3(&(*itr));
		XMVECTOR vPos2 = XMLoadFloat3(&(*itrSub));

		//���_�������Ă���
		XMFLOAT3 pos;
		XMStoreFloat3(&pos, vPos1);
		VERTEX vertex1 = { pos, XMFLOAT3((float)i / (float)length_, 1, 0) };

		XMStoreFloat3(&pos, vPos2);
		VERTEX vertex2 = { pos, XMFLOAT3((float)i / (float)length_, 0, 0) };

		vertices[index] = vertex1;
		index++;
		vertices[index] = vertex2;
		index++;

		itr++;
		itrSub++;
		if (itr == positions_.end()) break;
	}

	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * length_ * 2;
	bd_vertex.Usage = D3D11_USAGE_DEFAULT;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = 0;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = vertices;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);

	delete[] vertices;
}
