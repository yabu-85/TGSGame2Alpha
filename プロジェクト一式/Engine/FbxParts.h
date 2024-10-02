#pragma once
#include <fbxsdk.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "Texture.h"
#include "Transform.h"

using namespace DirectX;

class Fbx;
struct RayCastData;
struct PolygonData;
struct OrientRotateInfo;
struct FbxBlendData;

//-----------------------------------------------------------
//FBX�̂P�̃p�[�c�������N���X
//-----------------------------------------------------------
class FbxParts
{
	// ��̒��_�����i�[����\����
	struct VERTEX
	{
		XMFLOAT3 position;
		XMFLOAT3 normal;
		XMFLOAT3 uv;
	};

	//�y�R���X�^���g�o�b�t�@�[�z
	// GPU(�V�F�[�_��)�֑��鐔�l���܂Ƃ߂��\����
	//Simple3D.hlsl�̃O���[�o���ϐ��ƑΉ�������
	struct CONSTANT_BUFFER
	{
		XMMATRIX	matWVP;
		XMMATRIX	matNormal;
		XMMATRIX	matWorld;
		XMMATRIX	mWLP;
		XMMATRIX	mWLPT;
		XMFLOAT4	diffuseColor;
		XMFLOAT4	specular;
		XMFLOAT4	camPos;
		XMFLOAT4	lightPos;
		FLOAT		shininess;
		BOOL		isTexture;
		BOOL		isShadow;
	};

	// �}�e���A�����i�����̏��j
	struct  MATERIAL
	{
		DWORD		polygonCount;		//�}�e���A���̃|���S����
		XMFLOAT4	diffuse;			//�g�U���ˌ��i�f�B�t���[�Y�j�ւ̔��ˋ��x
		XMFLOAT4	ambient;			//�����i�A���r�G���g�j�ւ̔��ˋ��x
		XMFLOAT4	specular;			//���ʔ��ˌ��i�X�y�L�����j�ւ̔��ˋ��x
		float		shininess;			//�n�C���C�g�̋����i�T�C�Y�j
		Texture*	pTexture;			//�e�N�X�`��
	}*pMaterial_;

	// �{�[���\���́i�֐ߏ��j
	struct  Bone
	{
		XMMATRIX  bindPose;      // �����|�[�Y���̃{�[���ϊ��s��
		XMMATRIX  newPose;       // �A�j���[�V�����ŕω������Ƃ��̃{�[���ϊ��s��
		XMMATRIX  diffPose;      // mBindPose �ɑ΂��� mNowPose �̕ω���
	};

	// �E�F�C�g�\���́i�{�[���ƒ��_�̊֘A�t���j
	struct Weight
	{
		XMFLOAT3	posOrigin;		// ���X�̒��_���W
		XMFLOAT3	normalOrigin;	// ���X�̖@���x�N�g��
		int*		pBoneIndex;		// �֘A����{�[����ID
		float*		pBoneWeight;	// �{�[���̏d��
	};



	//�e�f�[�^�̌�
	DWORD vertexCount_;		//���_��
	DWORD polygonCount_;		//�|���S��
	DWORD indexCount_;		//�C���f�b�N�X��
	DWORD materialCount_;		//�}�e���A���̌�
	DWORD polygonVertexCount_;//�|���S�����_�C���f�b�N�X�� 

		
	VERTEX *pVertexData_;
	DWORD** ppIndexData_;


	//�y���_�o�b�t�@�z
	//�e���_�̏��i�ʒu�Ƃ��F�Ƃ��j���i�[����Ƃ���
	//���_�����̔z��ɂ��Ďg��
	ID3D11Buffer *pVertexBuffer_;

	//�y�C���f�b�N�X�o�b�t�@�z
	//�u�ǂ̒��_�v�Ɓu�ǂ̒��_�v�Ɓu�ǂ̒��_�v�łR�p�`�|���S���ɂȂ邩�̏����i�[����Ƃ���
	ID3D11Buffer **ppIndexBuffer_ = NULL;

	//�y�萔�o�b�t�@�z
	//�V�F�[�_�[�iSimple3D.hlsl�j�̃O���[�o���ϐ��ɒl��n�����߂̂���
	ID3D11Buffer *pConstantBuffer_;


	// �{�[��������
	FbxSkin*		pSkinInfo_;		// �X�L�����b�V�����i�X�L�����b�V���A�j���[�V�����̃f�[�^�{�́j
	FbxCluster**	ppCluster_;		// �N���X�^���i�֐߂��ƂɊ֘A�t����ꂽ���_���j
	int				numBone_;		// FBX�Ɋ܂܂�Ă���֐߂̐�
	Bone*			pBoneArray_;	// �e�֐߂̏��
	Weight*			pWeightArray_;	// �E�F�C�g���i���_�̑΂���e�֐߂̉e���x�����j

	/////////private�Ȋ֐��iInit�֐�����Ă΂��j//////////////////////////
	void InitVertex(fbxsdk::FbxMesh * mesh);	//���_�o�b�t�@����
	void InitMaterial(fbxsdk::FbxNode * pNode);	//�}�e���A������
	void InitTexture(fbxsdk::FbxSurfaceMaterial * pMaterial, const DWORD &i);	//�e�N�X�`������
	void InitIndex(fbxsdk::FbxMesh * mesh);		//�C���f�b�N�X�o�b�t�@����
	void InitSkelton(FbxMesh * pMesh);			//���̏�������
	void IntConstantBuffer();	//�R���X�^���g�o�b�t�@�i�V�F�[�_�[�ɏ��𑗂��j����

	//RotateOrient�̌v�Z������
	void RotateOrient();

	//FbxMatrix�����]�s��̌v�Z�����郉�W�A��
	XMFLOAT3 CalcMatRotateRatio(const fbxsdk::FbxMatrix& mat);

public:
	FbxParts();
	~FbxParts();

	//FBX�t�@�C������������[�h���ď��X��������
	//�����FpNode�@��񂪓����Ă���m�[�h
	//�ߒl�F����
	HRESULT Init(FbxNode * pNode);


	//�`��
	//�����Fworld	���[���h�s��
	void Draw(Transform& transform, bool isShadow);

	//�{�[���L��̃��f����`��
	//�����Ftransform	�s����
	//�����Ftime		�t���[�����i�P�A�j���[�V�������̍��ǂ����j
	void DrawSkinAnime(Transform& transform, FbxTime time, std::vector<OrientRotateInfo>& orientDatas, bool isShadow);
	
	//�{�[���L��̃��f����`��
	//�����Ftransform	�s����
	//�����Ftime		�t���[�����i�P�A�j���[�V�������̍��ǂ����j
	void DrawBlendedSkinAnim(Transform& transform, FbxTime time, std::vector<OrientRotateInfo>& orientDatas, bool isShadow, std::vector<FbxBlendData> &blendDatas);

	//�{�[�������̃��f����`��
	//�����Ftransform	�s����
	//�����FisShadow�@�@�e���肩������
	void DrawMeshAnime(Transform& transform, bool isShadow);

	//�{�[���̃C���f�b�N�X���擾
	bool GetBoneIndex(std::string boneName, int* index);

	//�C�ӂ̃{�[���̈ʒu���擾
	//�����FboneName	�擾�������{�[���̈ʒu
	//�����Fposition	���[���h���W�ł̈ʒu�yout�z
	//�ߒl�F�������true
	XMFLOAT3 GetBonePosition(int index);

	//�C�ӂ̃{�[���̈ʒu���擾
	XMFLOAT3 GetBonePosition(int index, FbxTime time, std::vector<OrientRotateInfo>& orientDatas);

	//�C�ӂ̃{�[���̉�]���擾
	XMFLOAT3 GetBoneRotate(int index, FbxTime time);

	//�X�L�����b�V�������擾
	//�ߒl�F�X�L�����b�V�����
	FbxSkin* GetSkinInfo() { return pSkinInfo_; }

	//���C�L���X�g�i���C���΂��ē����蔻��j
	//�����Fdata	�K�v�Ȃ��̂��܂Ƃ߂��f�[�^
	void RayCast(RayCastData *data);

	std::vector<PolygonData> GetAllPolygon(FbxNode* pNode);

};

