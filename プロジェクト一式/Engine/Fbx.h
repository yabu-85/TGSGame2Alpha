#pragma once
#include <d3d11.h>
#include <fbxsdk.h>
#include <vector>
#include <string>
#include "Transform.h"

class FbxParts;
struct OrientRotateInfo;
struct BlendData;

//���C�L���X�g�p�\����
struct RayCastData
{
	XMFLOAT3	start;	//���C���ˈʒu
	XMFLOAT3	dir;	//���C�̌����x�N�g��
	float       dist;	//�Փ˓_�܂ł̋���
	BOOL        hit;	//���C������������
	XMFLOAT3 normal;	//�@��

	RayCastData() { dist = 99999.0f; start = XMFLOAT3(); dir = XMFLOAT3(); hit = FALSE; normal = XMFLOAT3(); }
};

//FbxParts�ɑ���p�̃f�[�^
struct FbxBlendData {
	FbxTime time;
	float factor;

	FbxBlendData() : factor(0.0f) {}
};

struct PolygonData {
	XMFLOAT3 position_[3];
	XMFLOAT3 normal;
	PolygonData() : position_{ XMFLOAT3(), XMFLOAT3(), XMFLOAT3() }, normal(XMFLOAT3()) {}
	PolygonData(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT3 nor) : position_{ p1, p2, p3 }, normal(XMFLOAT3()) { }
};

//-----------------------------------------------------------
//�@FBX�t�@�C���������N���X
//�@�قƂ�ǂ̏����͊e�p�[�c���Ƃ�FbxParts�N���X�ōs��
//-----------------------------------------------------------
class Fbx
{
	//FbxPart�N���X���t�����h�N���X�ɂ���
	//FbxPart��private�Ȋ֐��ɂ��A�N�Z�X��
	friend class FbxParts;


	//���f���̊e�p�[�c�i�������邩���j
	std::vector<FbxParts*>	parts_;

	//Parts�ɑ���u�����h���
	std::vector<FbxBlendData> fbxBlendDatas_;

	//FBX�t�@�C���������@�\�̖{��
	FbxManager* pFbxManager_;

	//FBX�t�@�C���̃V�[���iMaya�ō�������ׂĂ̕��́j������
	FbxScene*	pFbxScene_;

	// �A�j���[�V�����̃t���[�����[�g
	FbxTime::EMode	_frameRate;

	//�m�[�h�̒��g�𒲂ׂ�
	//�����FpNode		���ׂ�m�[�h
	//�����FpPartsList	�p�[�c�̃��X�g
	void CheckNode(FbxNode* pNode, std::vector<FbxParts*> *pPartsList);

public:
	Fbx();
	~Fbx();

	//���[�h
	//�����FfileName	�t�@�C����
	//�ߒl�F�����������ǂ���
	virtual HRESULT Load(std::string fileName);

	//�`��
	void Draw(Transform& transform, int frame, std::vector<OrientRotateInfo> &orientDatas, bool isShadow);

	//�u�����h����̕`��
	void Draw(Transform& transform, int frame, std::vector<OrientRotateInfo>& orientDatas, bool isShadow, std::vector <BlendData> &blendDats);

	//���
	void Release();

	//�{�[���̃C���f�b�N�X�擾
	bool GetPartBoneIndex(std::string boneName, int* partIndex, int* boneIndex);

	//�C�ӂ̃{�[���̈ʒu���擾
	//�����FboneName	�擾�������{�[���̈ʒu
	//�ߒl�F�{�[���̈ʒu
	XMFLOAT3 GetBonePosition(int partIndex, int boneIndex);

	//�C�ӂ̃A�j���[�V�������̃{�[���̈ʒu���擾
	XMFLOAT3 GetBoneAnimPosition(int partIndex, int boneIndex, int frame, std::vector<OrientRotateInfo>& orientDatas);

	//�C�ӂ̃A�j���[�V�������̃{�[���̉�]���擾
	XMFLOAT3 GetBoneAnimRotate(int partIndex, int boneIndex, int frame);

	//���C�L���X�g�i���C���΂��ē����蔻��j
	//�����Fdata	�K�v�Ȃ��̂��܂Ƃ߂��f�[�^
	void RayCast(RayCastData *data);

	//���ׂẴ|���S���擾
	void GetAllPolygon(std::vector<PolygonData>& list);

	//�u�����h���̎擾
	std::vector<FbxBlendData>& GetBlendData();

	//�u�����h���̒ǉ�
	void AddBlendData(FbxBlendData data);

	//�A�j���[�V�����t���[�����[�g�擾
	FbxTime::EMode GetFrameRate();

private:
	//���ׂẴ|���S���擾�v�Z�p�֐�
	void GetAllPolygonRecursive(FbxNode* pNode, std::vector<PolygonData>& list);

};

