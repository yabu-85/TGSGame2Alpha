#pragma once
#include <DirectXMath.h>
#include <string>
#include "Fbx.h"
#include "Transform.h"

//UE�Ō����ALookAt�̃f�[�^
struct OrientRotateInfo
{
	int boneIndex;			//�{�[���̃C���f�b�N�X
	XMFLOAT3 orientRotate;	//��]��
	OrientRotateInfo() : boneIndex(-1), orientRotate(XMFLOAT3()) {}
};

//-----------------------------------------------------------
//3D���f���iFBX�t�@�C���j���Ǘ�����
//-----------------------------------------------------------
namespace Model
{
	//���f�����
	struct ModelData
	{
		//�t�@�C����
		std::string fileName;

		//���[�h�������f���f�[�^�̃A�h���X
		Fbx*		pFbx;

		//�s��
		Transform 	transform;

		//�A�j���[�V�����̃t���[��
		float nowFrame, animSpeed;
		int startFrame, endFrame;

		std::vector<OrientRotateInfo> orientRotateDatas_;
		
		//������
		ModelData() : pFbx(nullptr), nowFrame(0), startFrame(0), endFrame(0), animSpeed(0)
		{
		}

		//�A�j���[�V�����̃t���[�������Z�b�g
		//�����FstartFrame	�J�n�t���[��
		//�����FendFrame	�I���t���[��
		//�����FanimSpeed	�A�j���[�V�������x
		void SetAnimFrame(int start, int end, float speed)
		{
			nowFrame = (float)start;
			startFrame = start;
			endFrame = end;
			animSpeed = speed;
		}
	};



	//������
	void Initialize();

	//���f�������[�h
	//�����FfileName�@�t�@�C����
	//�ߒl�F���̃��f���f�[�^�Ɋ��蓖�Ă�ꂽ�ԍ�
	int Load(std::string fileName);

	//�`��
	//�����Fhandle	�`�悵�������f���̔ԍ�
	//�����Fmatrix	���[���h�s��
	void Draw(int handle);

	//�C�ӂ̃��f�����J��
	//�����Fhandle	�J�����������f���̔ԍ�
	void Release(int handle);

	//�S�Ẵ��f�������
	//�i�V�[�����؂�ւ��Ƃ��͕K�����s�j
	void AllRelease();

	//�A�j���[�V�����̃t���[�������Z�b�g
	//�����Fhandle		�ݒ肵�������f���̔ԍ�
	//�����FstartFrame	�J�n�t���[��
	//�����FendFrame	�I���t���[��
	//�����FanimSpeed	�A�j���[�V�������x
	void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed);

	//���݂̃A�j���[�V�����̃t���[�����擾
	int GetAnimFrame(int handle);

	//�p�[�c�ƃ{�[���̃C���f�b�N�X�擾
	bool GetPartBoneIndex(int handle, std::string boneName, int* partIndex, int* boneIndex);

	//�p�[�c�̃C���f�b�N�X�擾
	int GetPartIndex(int handle, std::string boneName);

	//�{�[���̃C���f�b�N�X�擾
	int GetBoneIndex(int handle, std::string boneName);

	//�C�ӂ̃{�[���̈ʒu���擾
	//�����Fhandle		���ׂ������f���̔ԍ�
	//�����FboneName	���ׂ����{�[���̖��O
	//�ߒl�F�{�[���̈ʒu�i���[���h���W�j
	XMFLOAT3 GetBonePosition(int handle, int partIndex, int boneIndex);

	//�A�j���[�V�������̃{�[���̈ʒu���擾
	XMFLOAT3 GetBoneAnimPosition(int handle, int partIndex, int boneIndex);
	
	//�A�j���[�V�������̃{�[���̉�]���擾
	XMFLOAT3 GetBoneAnimRotate(int handle, int partIndex, int boneIndex);

	//���[���h�s���ݒ�
	//�����Fhandle	�ݒ肵�������f���̔ԍ�
	//�����Fmatrix	���[���h�s��
	void SetTransform(int handle, Transform& transform);

	//���[���h�s��̎擾
	//�����Fhandle	�m�肽�����f���̔ԍ�
	//�ߒl�F���[���h�s��
	XMMATRIX GetMatrix(int handle);

	//���C�L���X�g�i���C���΂��ē����蔻��j
	//�����Fhandle	���肵�������f���̔ԍ�
	//�����Fdata	�K�v�Ȃ��̂��܂Ƃ߂��f�[�^
	void RayCast(int handle, RayCastData *data);

	//���X�g�ɒǉ�
	int AddOrientRotateBone(int handle, std::string boneName);

	//���X�g������
	void ResetOrientRotateBone(int handle);
	
	//��]���Z�b�g
	void SetOrietnRotateBone(int handle, int listIndex, XMFLOAT3 rotate);

	//���ׂẴ|���S���擾
	void GetAllPolygon(int handle, std::vector<PolygonData>& list);

};