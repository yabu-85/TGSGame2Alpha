#include "Fbx.h"
#include "Direct3D.h"
#include "FbxParts.h"
#include "Model.h"

Fbx::Fbx() : _frameRate(), pFbxScene_(nullptr), pFbxManager_(nullptr)
{
}

Fbx::~Fbx()
{
	for (int i = 0; i < parts_.size(); i++)
	{
		delete parts_[i];
	}
	parts_.clear();

	pFbxScene_->Destroy();
	pFbxManager_->Destroy();
}

HRESULT Fbx::Load(std::string fileName)
{
	// FBX�̓ǂݍ���
	pFbxManager_ = FbxManager::Create();
	pFbxScene_ = FbxScene::Create(pFbxManager_, "fbxscene");
	FbxString FileName(fileName.c_str());
	FbxImporter *fbxImporter = FbxImporter::Create(pFbxManager_, "imp");
	if (!fbxImporter->Initialize(FileName.Buffer(), -1, pFbxManager_->GetIOSettings()))
	{
		//���s
		return E_FAIL;
	}
	fbxImporter->Import(pFbxScene_);
	fbxImporter->Destroy();

	// �A�j���[�V�����̃^�C�����[�h�̎擾
	_frameRate = pFbxScene_->GetGlobalSettings().GetTimeMode();


	//���݂̃J�����g�f�B���N�g�����o���Ă���
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//�J�����g�f�B���N�g�����t�@�C�����������ꏊ�ɕύX
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(dir);



	//���[�g�m�[�h���擾����
	FbxNode* rootNode = pFbxScene_->GetRootNode();

	//�����̎q���̐��𒲂ׂ�
	int childCount = rootNode->GetChildCount();

	//1���`�F�b�N
	for (int i = 0; childCount > i; i++)
	{
		CheckNode(rootNode->GetChild(i), &parts_);
	}



	//�J�����g�f�B���N�g�������̈ʒu�ɖ߂�
	SetCurrentDirectory(defaultCurrentDir);

	return S_OK;
}

void Fbx::CheckNode(FbxNode * pNode, std::vector<FbxParts*>* pPartsList)
{
	//���̃m�[�h�ɂ̓��b�V����񂪓����Ă��邾�낤���H
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//�p�[�c��p��
		FbxParts* pParts = new FbxParts;
		pParts->Init(pNode);

		//�p�[�c���𓮓I�z��ɒǉ�
		pPartsList->push_back(pParts);
	}


	//�q�m�[�h�ɂ��f�[�^�����邩���I�I
	{
		//�q���̐��𒲂ׂ�
		int childCount = pNode->GetChildCount();

		//��l���`�F�b�N
		for (int i = 0; i < childCount; i++)
		{
			CheckNode(pNode->GetChild(i), pPartsList);
		}
	}
}


void Fbx::Release()
{

}

bool Fbx::GetPartBoneIndex(std::string boneName, int* partIndex, int* boneIndex)
{
	*partIndex = -1;
	*boneIndex = -1;

	for (int i = 0; i < parts_.size(); i++)
	{
		if (parts_[i]->GetBoneIndex(boneName, boneIndex)) {
			*partIndex = i;
			return true;
		}
	}
	return false;
}

XMFLOAT3 Fbx::GetBonePosition(int partIndex, int boneIndex)
{
	return parts_[partIndex]->GetBonePosition(boneIndex);
}

XMFLOAT3 Fbx::GetBoneAnimPosition(int partIndex, int boneIndex, int frame)
{
	FbxTime time;
	time.SetTime(0, 0, 0, frame, 0, 0, _frameRate);
	return parts_[partIndex]->GetBonePosition(boneIndex, time);
}

XMFLOAT3 Fbx::GetBoneAnimRotate(int partIndex, int boneIndex, int frame)
{
	FbxTime time;
	time.SetTime(0, 0, 0, frame, 0, 0, _frameRate);
	return parts_[partIndex]->GetBoneRotate(boneIndex, time);
}

void Fbx::Draw(Transform& transform, int frame, std::vector<OrientRotateInfo> &orientDatas, bool isShadow)
{
	Direct3D::SetBlendMode(Direct3D::BLEND_DEFAULT);

	//�p�[�c��1���`��
	for (int k = 0; k < parts_.size(); k++)
	{
		// ���̏u�Ԃ̎����̎p���s��𓾂�
		FbxTime     time;
		time.SetTime(0, 0, 0, frame, 0, 0, _frameRate);

		//�X�L���A�j���[�V�����i�{�[���L��j�̏ꍇ
		if (parts_[k]->GetSkinInfo() != nullptr)
		{
			parts_[k]->DrawSkinAnime(transform, time, orientDatas, isShadow);
		}

		//���b�V���A�j���[�V�����̏ꍇ
		else
		{
			parts_[k]->DrawMeshAnime(transform, time, pFbxScene_, isShadow);
		}
	}
}


//���C�L���X�g�i���C���΂��ē����蔻��j
void Fbx::RayCast(RayCastData * data)
{
	//���ׂẴp�[�c�Ɣ���
	for (int i = 0; i < parts_.size(); i++)
	{
		parts_[i]->RayCast(data);
	}
}

void Fbx::GetAllPolygon(std::vector<PolygonData>& list)
{
	FbxNode* rootNode = pFbxScene_->GetRootNode();
	int childCount = rootNode->GetChildCount();
	for (int i = 0; childCount > i; i++) {
		GetAllPolygonRecursive(rootNode->GetChild(i), list);
	}
}

void Fbx::GetAllPolygonRecursive(FbxNode* pNode, std::vector<PolygonData>& list)
{
	if (!pNode) return;

	//���̃m�[�h�����b�V���������Ă��邩�ǂ������m�F
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//�|���S���f�[�^������΃��X�g�ɒǉ�
		std::vector<PolygonData> poly = parts_[list.size()]->GetAllPolygon(pNode);
		if (poly.empty()) list = poly;
		else list.insert(list.end(), poly.begin(), poly.end());
	}

	//���̃m�[�h�̂��ׂĂ̎q�m�[�h�ɑ΂��čċA�I�ɏ������s��
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		GetAllPolygonRecursive(pNode->GetChild(i), list);
	}
}
