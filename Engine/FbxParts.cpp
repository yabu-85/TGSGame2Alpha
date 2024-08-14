#include "FbxParts.h"
#include "Fbx.h"
#include "Global.h"
#include "Direct3D.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"

//�R���X�g���N�^
FbxParts::FbxParts():
	ppIndexBuffer_(nullptr), pMaterial_(nullptr), 
	pVertexBuffer_(nullptr), pConstantBuffer_(nullptr),
	pVertexData_(nullptr), ppIndexData_(nullptr)
{
}

//�f�X�g���N�^
FbxParts::~FbxParts()
{
	SAFE_DELETE_ARRAY(pBoneArray_);
	SAFE_DELETE_ARRAY(ppCluster_);

	if (pWeightArray_ != nullptr)
	{
		for (DWORD i = 0; i < vertexCount_; i++)
		{
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneIndex);
			SAFE_DELETE_ARRAY(pWeightArray_[i].pBoneWeight);
		}
		SAFE_DELETE_ARRAY(pWeightArray_);
	}


	SAFE_DELETE_ARRAY(pVertexData_);
	for (DWORD i = 0; i < materialCount_; i++)
	{
		SAFE_RELEASE(ppIndexBuffer_[i]);
		SAFE_DELETE(ppIndexData_[i]);
		SAFE_DELETE(pMaterial_[i].pTexture);

	}
	SAFE_DELETE_ARRAY(ppIndexBuffer_);
	SAFE_DELETE_ARRAY(ppIndexData_);
	SAFE_DELETE_ARRAY(pMaterial_);

	SAFE_RELEASE(pVertexBuffer_);
	SAFE_RELEASE(pConstantBuffer_);
}

//FBX�t�@�C������������[�h���ď��X��������
HRESULT FbxParts::Init(FbxNode *pNode)
{
	//�m�[�h���烁�b�V���̏����擾
	FbxMesh* mesh = pNode->GetMesh();

	//�e���̌����擾
	vertexCount_ = mesh->GetControlPointsCount();			//���_�̐�
	polygonCount_ = mesh->GetPolygonCount();				//�|���S���̐�
	polygonVertexCount_ = mesh->GetPolygonVertexCount();	//�|���S�����_�C���f�b�N�X�� 


	InitVertex(mesh);		//���_�o�b�t�@����
	InitMaterial(pNode);	//�}�e���A������
	InitIndex(mesh);		//�C���f�b�N�X�o�b�t�@����
	InitSkelton(mesh);		//���̏�������
	IntConstantBuffer();	//�R���X�^���g�o�b�t�@�i�V�F�[�_�[�ɏ��𑗂��j����

	return E_NOTIMPL;
}


//���_�o�b�t�@����
void FbxParts::InitVertex(fbxsdk::FbxMesh * mesh)
{
	pVertexData_ = new VERTEX[vertexCount_];

	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3���_��
		for (int vertex = 0; vertex < 3; vertex++)
		{
			int index = mesh->GetPolygonVertex(poly, vertex);

			/////////////////////////���_�̈ʒu/////////////////////////////////////
			FbxVector4 pos = mesh->GetControlPointAt(index);
			pVertexData_[index].position = XMFLOAT3((float)-pos[0], (float)pos[1], (float)pos[2]);

			/////////////////////////���_�̖@��/////////////////////////////////////
			FbxVector4 Normal;
			mesh->GetPolygonVertexNormal(poly, vertex, Normal);	//���Ԗڂ̃|���S���́A���Ԗڂ̒��_�̖@�����Q�b�g
			pVertexData_[index].normal = XMFLOAT3((float)-Normal[0], (float)Normal[1], (float)Normal[2]);

			///////////////////////////���_�̂t�u/////////////////////////////////////
			FbxLayerElementUV * pUV = mesh->GetLayer(0)->GetUVs();
			int uvIndex = mesh->GetTextureUVIndex(poly, vertex, FbxLayerElement::eTextureDiffuse);
			FbxVector2  uv = pUV->GetDirectArray().GetAt(uvIndex);
			pVertexData_[index].uv = XMFLOAT3((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f);
		}
	}


	///////////////////////////���_�̂t�u/////////////////////////////////////
	int m_dwNumUV = mesh->GetTextureUVCount();
	FbxLayerElementUV * pUV = mesh->GetLayer(0)->GetUVs();
	if (m_dwNumUV > 0 && pUV->GetMappingMode() == FbxLayerElement::eByControlPoint)
	{
		for (int k = 0; k < m_dwNumUV; k++)
		{
			FbxVector2 uv = pUV->GetDirectArray().GetAt(k);
			pVertexData_[k].uv = XMFLOAT3((float)uv.mData[0], (float)(1.0f - uv.mData[1]), 0.0f);
		}
	}


	// ���_�f�[�^�p�o�b�t�@�̐ݒ�
	D3D11_BUFFER_DESC bd_vertex;
	bd_vertex.ByteWidth = sizeof(VERTEX) * mesh->GetControlPointsCount();
	bd_vertex.Usage = D3D11_USAGE_DYNAMIC;
	bd_vertex.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd_vertex.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd_vertex.MiscFlags = 0;
	bd_vertex.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA data_vertex;
	data_vertex.pSysMem = pVertexData_;
	Direct3D::pDevice_->CreateBuffer(&bd_vertex, &data_vertex, &pVertexBuffer_);
}

//�}�e���A������
void FbxParts::InitMaterial(fbxsdk::FbxNode * pNode)
{
	// �}�e���A���o�b�t�@�̐���
	materialCount_ = pNode->GetMaterialCount();
	pMaterial_ = new MATERIAL[materialCount_];



	for (DWORD i = 0; i < materialCount_; i++)
	{
		ZeroMemory(&pMaterial_[i], sizeof(pMaterial_[i]));

		// �t�H���V�F�[�f�B���O��z�肵���}�e���A���o�b�t�@�̒��o
		FbxSurfaceMaterial* pMaterial = pNode->GetMaterial(i);
		FbxSurfacePhong* pPhong = (FbxSurfacePhong*)pMaterial;

		// �������g�U���ˌ������ʔ��ˌ��̔��ː����l���擾
		FbxDouble3  ambient = FbxDouble3(0, 0, 0);
		FbxDouble3  diffuse = FbxDouble3(0, 0, 0);
		FbxDouble3  specular = FbxDouble3(0, 0, 0);
		ambient = pPhong->Ambient;
		diffuse = pPhong->Diffuse;


		// �������g�U���ˌ������ʔ��ˌ��̔��ː����l���}�e���A���o�b�t�@�ɃR�s�[
		pMaterial_[i].ambient = XMFLOAT4((float)ambient[0], (float)ambient[1], (float)ambient[2], 1.0f);
		pMaterial_[i].diffuse = XMFLOAT4((float)diffuse[0], (float)diffuse[1], (float)diffuse[2], 1.0f);
		pMaterial_[i].specular = XMFLOAT4(0, 0, 0, 0);
		pMaterial_[i].shininess = 0;


		if (pMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
		{
			specular = pPhong->Specular;
			pMaterial_[i].specular = XMFLOAT4((float)specular[0], (float)specular[1], (float)specular[2], 1.0f);
			pMaterial_[i].shininess = (float)pPhong->Shininess;
		}

		InitTexture(pMaterial, i);
	}
}

//�e�N�X�`������
void FbxParts::InitTexture(fbxsdk::FbxSurfaceMaterial * pMaterial, const DWORD &i)
{
	pMaterial_[i].pTexture = nullptr;

	// �e�N�X�`���[���̎擾
	FbxProperty  lProperty = pMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);

	//�e�N�X�`���̐�
	int fileTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

	if (fileTextureCount > 0)
	{
		FbxFileTexture* texture = lProperty.GetSrcObject<FbxFileTexture>(0);

		//�t�@�C����+�g�������ɂ���
		char name[_MAX_FNAME];	//�t�@�C����
		char ext[_MAX_EXT];		//�g���q
		_splitpath_s(texture->GetRelativeFileName(), nullptr, 0, nullptr, 0, name, _MAX_FNAME, ext, _MAX_EXT);
		wsprintf(name, "%s%s", name, ext);

		pMaterial_[i].pTexture = new Texture;
		pMaterial_[i].pTexture->Load(name);
	}
}

//�C���f�b�N�X�o�b�t�@����
void FbxParts::InitIndex(fbxsdk::FbxMesh * mesh)
{
	// �}�e���A���̐������C���f�b�N�X�o�b�t�@�[���쐬
	ppIndexBuffer_ = new ID3D11Buffer*[materialCount_];
	ppIndexData_ = new DWORD*[materialCount_];
	int count = 0;

	// �}�e���A������u�|���S�����ʁv�̏��𒊏o����
	for (DWORD i = 0; i < materialCount_; i++)
	{
		count = 0;
		DWORD *pIndex = new DWORD[polygonCount_ * 3];
		ZeroMemory(&pIndex[i], sizeof(pIndex[i]));

		// �|���S�����\������O�p�`���ʂ��A
		// �u���_�o�b�t�@�v���̂ǂ̒��_�𗘗p���Ă��邩�𒲂ׂ�
		for (DWORD j = 0; j < polygonCount_; j++)
		{
			FbxLayerElementMaterial *   mtl = mesh->GetLayer(0)->GetMaterials();
			int mtlId = mtl->GetIndexArray().GetAt(j);
			if (mtlId == i)
			{
				for (DWORD k = 0; k < 3; k++)
				{
					pIndex[count + k] = mesh->GetPolygonVertex(j, k);
				}
				count += 3;
			}
		}

		// �C���f�b�N�X�o�b�t�@�𐶐�����
		D3D11_BUFFER_DESC   bd;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(int) * count;
		bd.BindFlags = D3D10_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pIndex;
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if (FAILED(Direct3D::pDevice_->CreateBuffer(&bd, &InitData, &ppIndexBuffer_[i])))
		{
			//MessageBox(0, "�C���f�b�N�X�o�b�t�@�̐����Ɏ��s", fbxFileName, MB_OK);
			//return FALSE;
		}
		pMaterial_[i].polygonCount = count / 3;
		ppIndexData_[i] = new DWORD[count];
		memcpy(ppIndexData_[i], pIndex, sizeof(DWORD) * count);
		SAFE_DELETE_ARRAY(pIndex);
	}

}

//���̏�������
void FbxParts::InitSkelton(FbxMesh * pMesh)
{
	// �f�t�H�[�}���i�{�[���ƃ��f���̊֘A�t���j�̎擾
	FbxDeformer *   pDeformer = pMesh->GetDeformer(0);
	if (pDeformer == nullptr)
	{
		//�{�[�����Ȃ�
		return;
	}

	// �f�t�H�[�}��񂩂�X�L�����b�V�������擾
	pSkinInfo_ = (FbxSkin *)pDeformer;

	// �{�[�������擾����
	numBone_ = pSkinInfo_->GetClusterCount();
	ppCluster_ = new FbxCluster*[numBone_];
	for (int i = 0; i < numBone_; i++)
	{
		ppCluster_[i] = pSkinInfo_->GetCluster(i);
	}

	// �{�[���̐��ɍ��킹�ăE�F�C�g������������
	pWeightArray_ = new FbxParts::Weight[vertexCount_];
	for (DWORD i = 0; i < vertexCount_; i++)
	{
		pWeightArray_[i].posOrigin = pVertexData_[i].position;
		pWeightArray_[i].normalOrigin = pVertexData_[i].normal;
		pWeightArray_[i].pBoneIndex = new int[numBone_];
		pWeightArray_[i].pBoneWeight = new float[numBone_];
		for (int j = 0; j < numBone_; j++)
		{
			pWeightArray_[i].pBoneIndex[j] = -1;
			pWeightArray_[i].pBoneWeight[j] = 0.0f;
		}
	}


	// ���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�
	// ��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
	for (int i = 0; i < numBone_; i++)
	{
		int numIndex = ppCluster_[i]->GetControlPointIndicesCount();   //���̃{�[���ɉe�����󂯂钸�_��
		int * piIndex = ppCluster_[i]->GetControlPointIndices();       //�{�[��/�E�F�C�g���̔ԍ�
		double * pdWeight = ppCluster_[i]->GetControlPointWeights();     //���_���Ƃ̃E�F�C�g���

		//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
		for (int k = 0; k < numIndex; k++)
		{
			// ���_�Ɋ֘A�t����ꂽ�E�F�C�g��񂪃{�[���T�{�ȏ�̏ꍇ�́A�d�݂̑傫�����ɂS�{�ɍi��
			for (int m = 0; m < 4; m++)
			{
				if (m >= numBone_)
					break;

				if (pdWeight[k] > pWeightArray_[piIndex[k]].pBoneWeight[m])
				{
					for (int n = numBone_ - 1; n > m; n--)
					{
						pWeightArray_[piIndex[k]].pBoneIndex[n] = pWeightArray_[piIndex[k]].pBoneIndex[n - 1];
						pWeightArray_[piIndex[k]].pBoneWeight[n] = pWeightArray_[piIndex[k]].pBoneWeight[n - 1];
					}
					pWeightArray_[piIndex[k]].pBoneIndex[m] = i;
					pWeightArray_[piIndex[k]].pBoneWeight[m] = (float)pdWeight[k];
					break;
				}
			}

		}
	}

	//�{�[���𐶐�
	pBoneArray_ = new FbxParts::Bone[numBone_];
	for (int i = 0; i < numBone_; i++)
	{
		// �{�[���̃f�t�H���g�ʒu���擾����
		FbxAMatrix  matrix;
		ppCluster_[i]->GetTransformLinkMatrix(matrix);

		// �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
		XMFLOAT4X4 pose;
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x,y) = (float)matrix.Get(x, y);
			}
		}
		pBoneArray_[i].bindPose = XMLoadFloat4x4(&pose);
	}
}

//�R���X�^���g�o�b�t�@�i�V�F�[�_�[�ɏ��𑗂��j����
void FbxParts::IntConstantBuffer()
{
	// �萔�o�b�t�@�̍쐬(�p�����[�^�󂯓n���p)
	D3D11_BUFFER_DESC cb;
	cb.ByteWidth = sizeof(CONSTANT_BUFFER);
	cb.Usage = D3D11_USAGE_DYNAMIC;
	cb.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cb.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cb.MiscFlags = 0;
	cb.StructureByteStride = 0;
	Direct3D::pDevice_->CreateBuffer(&cb, NULL, &pConstantBuffer_);
}

void FbxParts::RotateOrient()
{
	/*
	for (const auto& pair : orientRotateMap_) {
		XMMATRIX rotationMatrix = 
			XMMatrixRotationX(XMConvertToRadians(pair.second.orientRotate.x)) * 
			XMMatrixRotationY(XMConvertToRadians(pair.second.orientRotate.y)) *
			XMMatrixRotationZ(XMConvertToRadians(pair.second.orientRotate.z));
		pBoneArray_[pair.second.boneIndex].diffPose *= rotationMatrix;
	}
	*/
}

XMFLOAT3 FbxParts::CalcMatRotateRatio(const fbxsdk::FbxMatrix& mat)
{
	//https://qiita.com/q_tarou/items/46e5045068742dfb2fa6
	float PI = 3.14159265358979f;
	float threshold = 0.0f;
	XMFLOAT3 rot = XMFLOAT3();
	float absMat = (float)mat.Get(2, 1);
	if (abs(absMat - 1.0f) < threshold) { // R(2,1) = sin(x) = 1�̎�
		rot.x = PI / 2.0f;
		rot.y = 0.0f;
		rot.z = atan2f((float)mat.Get(1, 0), (float)mat.Get(0, 0));
	}
	else if (abs(absMat + 1.0f) < threshold) { // R(2,1) = sin(x) = -1�̎�
		rot.x = -PI / 2.0f;
		rot.y = 0.0f;
		rot.z = atan2f((float)mat.Get(1, 0), (float)mat.Get(0, 0));
	}
	else {
		rot.x = asinf(absMat);
		rot.y = atan2f(-(float)mat.Get(2, 0), (float)mat.Get(2, 2));
		rot.z = atan2f(-(float)mat.Get(0, 1), (float)mat.Get(1, 1));
	}
	return rot;
}

//�`��
void FbxParts::Draw(Transform& transform, bool isShadow)
{
	//������`�悷�钸�_�����V�F�[�_�ɓ`����
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Direct3D::pContext_->IASetVertexBuffers(0, 1, &pVertexBuffer_, &stride, &offset);

	//�g�p����R���X�^���g�o�b�t�@���V�F�[�_�ɓ`����
	Direct3D::pContext_->VSSetConstantBuffers(0, 1, &pConstantBuffer_);
	Direct3D::pContext_->PSSetConstantBuffers(0, 1, &pConstantBuffer_);


	//�V�F�[�_�[�̃R���X�^���g�o�b�t�@�[�Ɋe��f�[�^��n��
	for (DWORD i = 0; i < materialCount_; i++)
	{
		// �C���f�b�N�X�o�b�t�@�[���Z�b�g
		UINT    stride = sizeof(int);
		UINT    offset = 0;
		Direct3D::pContext_->IASetIndexBuffer(ppIndexBuffer_[i], DXGI_FORMAT_R32_UINT, 0);

		//�R���X�^���g�o�b�t�@�ɓn�����
		CONSTANT_BUFFER cb;
		cb.matWVP = XMMatrixTranspose(transform.GetWorldMatrix() * Camera::GetViewMatrix() * Camera::GetProjectionMatrix());
		cb.matNormal = XMMatrixTranspose(transform.GetNormalMatrix());
		cb.matWorld = XMMatrixTranspose(transform.GetWorldMatrix());
		cb.isTexture = pMaterial_[i].pTexture != nullptr;
		cb.diffuseColor = pMaterial_[i].diffuse;
		cb.specular = pMaterial_[i].specular;
		cb.shininess = pMaterial_[i].shininess;
		cb.camPos = XMFLOAT4(Camera::GetPosition(0).x, Camera::GetPosition(0).y, Camera::GetPosition(0).z, 0);
		cb.lightPos = Light::GetPosition(0);
		cb.mWLP = XMMatrixTranspose(transform.GetWorldMatrix() * Direct3D::lightViewMatrix * Camera::GetProjectionMatrix());
		cb.mWLPT = XMMatrixTranspose(transform.GetWorldMatrix() * Direct3D::lightViewMatrix * Camera::GetProjectionMatrix() * Direct3D::clipToUVMatrix);

		D3D11_MAPPED_SUBRESOURCE pdata;
		Direct3D::pContext_->Map(pConstantBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &pdata);	// GPU����̃f�[�^�A�N�Z�X���~�߂�
		memcpy_s(pdata.pData, pdata.RowPitch, (void*)(&cb), sizeof(cb));	// �f�[�^��l�𑗂�

		// �e�N�X�`�����V�F�[�_�[�ɐݒ�
		if (cb.isTexture)
		{
			ID3D11SamplerState*			pSampler = pMaterial_[i].pTexture->GetSampler();
			Direct3D::pContext_->PSSetSamplers(0, 1, &pSampler);

			ID3D11ShaderResourceView*	pSRV = pMaterial_[i].pTexture->GetSRV();
			Direct3D::pContext_->PSSetShaderResources(0, 1, &pSRV);
		}

		Direct3D::pContext_->PSSetSamplers(1, 1, &Direct3D::pDepthSampler_);
		Direct3D::pContext_->PSSetShaderResources(2, 1, &Direct3D::pDepthSRV_);
		
		//GPU����̃��\�[�X�A�N�Z�X���ĊJ
		Direct3D::pContext_->Unmap(pConstantBuffer_, 0);

		//�|���S�����b�V����`�悷��
		Direct3D::pContext_->DrawIndexed(pMaterial_[i].polygonCount * 3, 0, 0);
	}

}

// ��]�s�񂩂�I�C���[�p�𒊏o����֐�
XMFLOAT3 GetEulerAnglesFromMatrix(const XMMATRIX& matrix) {
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, matrix);

	XMFLOAT3 euler;

	// �I�C���[�p���v�Z
	if (mat.m[0][2] < 1.0f) {
		if (mat.m[0][2] > -1.0f) {
			euler.y = asinf(mat.m[0][2]);
			euler.x = atan2f(-mat.m[1][2], mat.m[2][2]);
			euler.z = atan2f(-mat.m[0][1], mat.m[0][0]);
		}
		else { // mat.m[0][2] == -1
			euler.y = -XM_PIDIV2;
			euler.x = -atan2f(mat.m[1][0], mat.m[1][1]);
			euler.z = 0.0f;
		}
	}
	else { // mat.m[0][2] == 1
		euler.y = XM_PIDIV2;
		euler.x = atan2f(mat.m[1][0], mat.m[1][1]);
		euler.z = 0.0f;
	}

	return euler;
}

//�{�[���L��̃��f����`��
void FbxParts::DrawSkinAnime(Transform& transform, FbxTime time, std::vector<OrientRotateInfo>& orientDatas, bool isShadow)
{
	//�{�[�����Ƃ̌��݂̍s����擾����
	for (int i = 0; i < numBone_; i++)
	{
		FbxAnimEvaluator * evaluator = ppCluster_[i]->GetLink()->GetScene()->GetAnimationEvaluator();
		FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(ppCluster_[i]->GetLink(), time);

		// �s��R�s�[�iFbx�`������DirectX�ւ̕ϊ��j
		XMFLOAT4X4 pose;
		for (DWORD x = 0; x < 4; x++)
		{
			for (DWORD y = 0; y < 4; y++)
			{
				pose(x, y) = (float)mCurrentOrentation.Get(x, y);
			}
		}

		//�I�t�Z�b�g���̃|�[�Y�̍������v�Z����
		pBoneArray_[i].newPose = XMLoadFloat4x4(&pose);
		pBoneArray_[i].diffPose = XMMatrixInverse(nullptr, pBoneArray_[i].bindPose);
		pBoneArray_[i].diffPose *= pBoneArray_[i].newPose;
	}

	//Orient�̌v�Z
	for (const auto& pair : orientDatas)
	{
		// ���s�ړ��s����쐬����
		XMFLOAT3 bonePosition;
		XMStoreFloat3(&bonePosition, pBoneArray_[pair.boneIndex].newPose.r[3]);
		XMMATRIX translationToOrigin = XMMatrixTranslation(bonePosition.x, bonePosition.y, bonePosition.z);

		//��]�s����쐬����
		XMMATRIX mat2 =
			XMMatrixRotationX(XMConvertToRadians(pair.orientRotate.x)) *
			XMMatrixRotationY(XMConvertToRadians(pair.orientRotate.y)) *
			XMMatrixRotationZ(XMConvertToRadians(pair.orientRotate.z));

		//mat2�𕪉����ĉ�]�����𒊏o
		XMVECTOR scale2, rotation2, translation2;
		XMMatrixDecompose(&scale2, &rotation2, &translation2, mat2);

		//mat1�𕪉����ĉ�]�����𒊏o
		XMVECTOR scale1, rotation1, translation1;
		XMMatrixDecompose(&scale1, &rotation1, &translation1, pBoneArray_[pair.boneIndex].newPose);

		//mat1��mat2�����킹����]Matrix�̐���
		XMVECTOR combinedRotation = XMQuaternionMultiply(rotation2, rotation2);
		XMMATRIX rotationMatrix = XMMatrixRotationQuaternion(combinedRotation);

		// mat1�̃X�P�[���ƕ��s�ړ���V�����s��ɓK�p
		XMMATRIX resultMatrix = XMMatrixScalingFromVector(scale1) * rotationMatrix;
		resultMatrix.r[3] = pBoneArray_[pair.boneIndex].newPose.r[3]; // ���s�ړ��������R�s�[
		
		XMMATRIX preMatrix = pBoneArray_[pair.boneIndex - 1].newPose;
		XMVECTOR transVector = pBoneArray_[pair.boneIndex].newPose.r[3];
		
		pBoneArray_[pair.boneIndex].newPose = pBoneArray_[pair.boneIndex].newPose * mat2;
		if (pair.boneIndex == 7) {
		//	pBoneArray_[pair.boneIndex].newPose.r[3] = translationToOrigin.r[3];
		}
		else {
		//	pBoneArray_[pair.boneIndex].newPose.r[3] = transVector;
		}

		pBoneArray_[pair.boneIndex].diffPose = XMMatrixInverse(nullptr, pBoneArray_[pair.boneIndex].bindPose);
		pBoneArray_[pair.boneIndex].diffPose *= pBoneArray_[pair.boneIndex].newPose;
	}

	//�e�{�[���ɑΉ��������_�̕ό`����
	for (DWORD i = 0; i < vertexCount_; i++)
	{
		//�e���_���ƂɁA�u�e������{�[���~�E�F�C�g�l�v�𔽉f�������֐ߍs����쐬����
		XMMATRIX  matrix;
		ZeroMemory(&matrix, sizeof(matrix));
		for (int m = 0; m < numBone_; m++)
		{
			if (pWeightArray_[i].pBoneIndex[m] < 0)
			{
				break;
			}
			matrix += pBoneArray_[pWeightArray_[i].pBoneIndex[m]].diffPose * pWeightArray_[i].pBoneWeight[m];
		}

		//�쐬���ꂽ�֐ߍs����g���āA���_��ό`����
		XMVECTOR Pos = XMLoadFloat3(&pWeightArray_[i].posOrigin);
		XMVECTOR Normal = XMLoadFloat3(&pWeightArray_[i].normalOrigin);
		XMStoreFloat3(&pVertexData_[i].position, XMVector3TransformCoord(Pos, matrix));
		XMFLOAT3X3 mat33;
		XMStoreFloat3x3(&mat33, matrix);
		XMMATRIX matrix33 = XMLoadFloat3x3(&mat33);
		XMStoreFloat3(&pVertexData_[i].normal, XMVector3TransformCoord(Normal, matrix33));
	}
	
	//���_�o�b�t�@�����b�N���āA�ό`��������̒��_���ŏ㏑������
	D3D11_MAPPED_SUBRESOURCE msr = {};
	Direct3D::pContext_->Map(pVertexBuffer_, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	if (msr.pData)
	{
		memcpy_s(msr.pData, msr.RowPitch, pVertexData_, sizeof(VERTEX) * vertexCount_);
		Direct3D::pContext_->Unmap(pVertexBuffer_, 0);
	}

	Draw(transform, isShadow);
}

void FbxParts::DrawMeshAnime(Transform& transform, FbxTime time, FbxScene * scene, bool isShadow)
{
	//// ���̏u�Ԃ̎����̎p���s��𓾂�
	//FbxAnimEvaluator *evaluator = scene->GetAnimationEvaluator();
	//FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(_pNode, time);

	//// Fbx�`���̍s�񂩂�DirectX�`���̍s��ւ̃R�s�[�i4�~4�̍s��j
	//for (DWORD x = 0; x < 4; x++)
	//{
	//	for (DWORD y = 0; y < 4; y++)
	//	{
	//		_localMatrix(x, y) = (float)mCurrentOrentation.Get(x, y);
	//	}
	//}

	Draw(transform, isShadow);
}

bool FbxParts::GetBoneIndex(std::string boneName, int* index)
{
	for (int i = 0; i < numBone_; i++)
	{
		if (boneName == ppCluster_[i]->GetLink()->GetName())
		{
			*index = i;
			return true;
		}
	}
	return false;
}

XMFLOAT3 FbxParts::GetBonePosition(int index)
{
	FbxAMatrix  matrix;
	ppCluster_[index]->GetTransformLinkMatrix(matrix);

	XMFLOAT3 pos = XMFLOAT3();
	pos.x = (float)matrix[3][0];
	pos.y = (float)matrix[3][1];
	pos.z = (float)matrix[3][2];
	return pos;
}

XMFLOAT3 FbxParts::GetBonePosition(int index, FbxTime time)
{
	FbxAnimEvaluator* evaluator = ppCluster_[index]->GetLink()->GetScene()->GetAnimationEvaluator();
	FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(ppCluster_[index]->GetLink(), time);

	XMFLOAT3 pos = XMFLOAT3();
	pos.x = (float)mCurrentOrentation[3][0];
	pos.y = (float)mCurrentOrentation[3][1];
	pos.z = (float)mCurrentOrentation[3][2];
	return pos;
}

XMFLOAT3 FbxParts::GetBoneRotate(int index, FbxTime time)
{
	FbxAnimEvaluator* evaluator = ppCluster_[index]->GetLink()->GetScene()->GetAnimationEvaluator();
	FbxMatrix mCurrentOrentation = evaluator->GetNodeGlobalTransform(ppCluster_[index]->GetLink(), time);

	XMFLOAT3 rot = CalcMatRotateRatio(mCurrentOrentation);
	rot.x = -XMConvertToDegrees(rot.x);
	rot.y = -XMConvertToDegrees(rot.y);
	rot.z = XMConvertToDegrees(rot.z);
	return rot;
}

void FbxParts::RayCast(RayCastData * data)
{
	data->hit = FALSE;

	//�}�e���A����
	for (DWORD i = 0; i < materialCount_; i++)
	{
		//���̃}�e���A���̃|���S����
		for (DWORD j = 0; j < pMaterial_[i].polygonCount; j++)
		{
			//3���_
			XMFLOAT3 ver[3];
			ver[0] = pVertexData_[ppIndexData_[i][j * 3 + 0]].position;
			ver[1] = pVertexData_[ppIndexData_[i][j * 3 + 1]].position;
			ver[2] = pVertexData_[ppIndexData_[i][j * 3 + 2]].position;

			BOOL  hit = FALSE;
			float dist = 0.0f;
			XMVECTOR normal = XMVectorZero();
			hit = Direct3D::Intersect(data->start, data->dir, ver[0], ver[1], ver[2], &dist, &normal);

			if (hit && dist < data->dist)
			{
				data->hit = TRUE;
				data->dist = dist;
				XMStoreFloat3(&data->normal, normal);
			}
		}
	}
}

std::vector<PolygonData> FbxParts::GetAllPolygon(FbxNode* pNode)
{
	FbxMesh* mesh = pNode->GetMesh();
	std::vector<PolygonData> list;

	for (DWORD poly = 0; poly < polygonCount_; poly++)
	{
		//3���_��
		PolygonData data = PolygonData();
		for (int vertex = 0; vertex < 3; vertex++)
		{
			int index = mesh->GetPolygonVertex(poly, vertex);
			FbxVector4 pos = mesh->GetControlPointAt(index);
			data.position_[vertex] = XMFLOAT3((float)pos[0], (float)pos[1], (float)pos[2]);
		}
		list.push_back(data);
	}

	return list;
}
