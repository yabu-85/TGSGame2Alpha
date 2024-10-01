#include "Global.h"
#include "Model.h"
#include "../Other/GameManager.h"
#include <assert.h>
#include <vector>

//3D���f���iFBX�t�@�C���j���Ǘ�����
namespace Model
{
	//���[�h�ς݂̃��f���f�[�^�ꗗ
	std::vector<ModelData*>	_datas;

	//������
	void Initialize()
	{
		AllRelease();
	}

	//���f�������[�h
	int Load(std::string fileName)
	{
			ModelData* pData = new ModelData;

			//�J�����t�@�C���ꗗ���瓯���t�@�C�����̂��̂��������T��
			bool isExist = false;
			for (int i = 0; i < _datas.size(); i++)
			{
				//���łɊJ���Ă���ꍇ
				if (_datas[i] != nullptr && _datas[i]->fileName == fileName)
				{
					pData->pFbx = _datas[i]->pFbx;
					isExist = true;
					break;
				}
			}

			//�V���Ƀt�@�C�����J��
			if (isExist == false)
			{
				pData->pFbx = new Fbx;
				if (FAILED(pData->pFbx->Load(fileName)))
				{
					//�J���Ȃ�����
					SAFE_DELETE(pData->pFbx);
					SAFE_DELETE(pData);
					return -1;
				}

				//�����J����
				pData->fileName = fileName;
			}


			//�g���ĂȂ��ԍ����������T��
			for (int i = 0; i < _datas.size(); i++)
			{
				if (_datas[i] == nullptr)
				{
					_datas[i] = pData;
					return i;
				}
			}

			//�V���ɒǉ�
			_datas.push_back(pData);
			return (int)_datas.size() - 1;
	}

	void Update(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		if (!_datas[handle]->isAnimStop) {
			//�A�j���[�V������i�߂�
			_datas[handle]->nowFrame += _datas[handle]->animSpeed;

			//�Ō�܂ŃA�j���[�V����������߂�
			if (_datas[handle]->nowFrame > (float)_datas[handle]->endFrame) {
				if(_datas[handle]->isAnimLoop) _datas[handle]->nowFrame = (float)_datas[handle]->startFrame;
				else _datas[handle]->nowFrame = (float)_datas[handle]->endFrame;
			}
		}
	}

	//�`��
	void Draw(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		if (_datas[handle]->pFbx)
		{
			bool shadow = _datas[handle]->isShadow && GameManager::IsShadowDraw();
			if (_datas[handle]->isBlending) {
				_datas[handle]->pFbx->Draw(_datas[handle]->transform, (int)_datas[handle]->nowFrame, _datas[handle]->orientRotateDatas_, shadow);
			}
			else {
				_datas[handle]->pFbx->Draw(_datas[handle]->transform, (int)_datas[handle]->nowFrame, _datas[handle]->orientRotateDatas_, shadow);
			}

		}
	}


	//�C�ӂ̃��f�����J��
	void Release(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		//�������f���𑼂ł��g���Ă��Ȃ���
		bool isExist = false;
		for (int i = 0; i < _datas.size(); i++)
		{
			//���łɊJ���Ă���ꍇ
			if (_datas[i] != nullptr && i != handle && _datas[i]->pFbx == _datas[handle]->pFbx)
			{
				isExist = true;
				break;
			}
		}

		//�g���ĂȂ���΃��f�����
		if (isExist == false )
		{
			SAFE_DELETE(_datas[handle]->pFbx);
		}


		SAFE_DELETE(_datas[handle]);
	}


	//�S�Ẵ��f�������
	void AllRelease()
	{
		for (int i = 0; i < _datas.size(); i++)
		{
			if (_datas[i] != nullptr)
			{
				Release(i);
			}
		}
		_datas.clear();
	}


	//�A�j���[�V�����̃t���[�������Z�b�g
	void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed)
	{
		_datas[handle]->SetAnimFrame(startFrame, endFrame, animSpeed);
	}


	//���݂̃A�j���[�V�����̃t���[�����擾
	int GetAnimFrame(int handle)
	{
		return (int)_datas[handle]->nowFrame;
	}

	void AnimStart(int handle)
	{
		_datas[handle]->isAnimStop = false;	
	}

	void AnimStop(int handle)
	{
		_datas[handle]->isAnimStop = true;
	}

	void SetAnimLoop(int handle, bool b)
	{
		_datas[handle]->isAnimLoop = b;
	}

	void SetBlend(int handle, bool b)
	{
		_datas[handle]->isBlending = b;
	}
	
	bool GetPartBoneIndex(int handle, std::string boneName, int* partIndex, int* boneIndex)
	{
		return _datas[handle]->pFbx->GetPartBoneIndex(boneName, partIndex, boneIndex);
	}

	int GetPartIndex(int handle, std::string boneName)
	{
		int partIndex = 0;
		int boneIndex = 0;
		_datas[handle]->pFbx->GetPartBoneIndex(boneName, &partIndex, &boneIndex);
		return partIndex;
	}

	int GetBoneIndex(int handle, std::string boneName)
	{
		int partIndex = 0;
		int boneIndex = 0;
		_datas[handle]->pFbx->GetPartBoneIndex(boneName, &partIndex, &boneIndex);
		return boneIndex;
	}

	XMFLOAT3 GetBonePosition(int handle, int partIndex, int boneIndex)
	{
		XMFLOAT3 pos = _datas[handle]->pFbx->GetBonePosition(partIndex, boneIndex);
		XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pos), _datas[handle]->transform.GetWorldMatrix());
		XMStoreFloat3(&pos, vec);
		return pos;
	}

	XMFLOAT3 GetBoneAnimPosition(int handle, int partIndex, int boneIndex)
	{
		//���΍��W�i�{�[���̒��S����̈ʒu�j
		XMFLOAT3 pos = _datas[handle]->pFbx->GetBoneAnimPosition(partIndex, boneIndex, (int)_datas[handle]->nowFrame, _datas[handle]->orientRotateDatas_);
		XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pos), _datas[handle]->transform.GetWorldMatrix()); //pos�����[���h�}�g���b�N�X�Ōv�Z����
		XMStoreFloat3(&pos, vec);
		return pos;
	}

	XMFLOAT3 GetBoneAnimRotate(int handle, int partIndex, int boneIndex)
	{
		//���΍��W�i�{�[���̒��S����̈ʒu�j
		XMFLOAT3 rot = _datas[handle]->pFbx->GetBoneAnimRotate(partIndex, boneIndex, (int)_datas[handle]->nowFrame);
		if (rot.x >= 90.0f || rot.x <= -90.0f) rot.y *= -1.0f;
		return rot;
	}

	//���[���h�s���ݒ�
	void SetTransform(int handle, Transform & transform)
	{
		if (handle < 0 || handle >= _datas.size())
		{
			return;
		}

		_datas[handle]->transform = transform;
	}


	//���[���h�s��̎擾
	XMMATRIX GetMatrix(int handle)
	{
		return _datas[handle]->transform.GetWorldMatrix();
	}


	//���C�L���X�g�i���C���΂��ē����蔻��j
	void RayCast(int handle, RayCastData *data)
	{
			XMFLOAT3 target = Transform::Float3Add(data->start, data->dir);
			XMMATRIX matInv = XMMatrixInverse(nullptr, _datas[handle]->transform.GetWorldMatrix());
			XMVECTOR vecStart = XMVector3TransformCoord(XMLoadFloat3(&data->start), matInv);
			XMVECTOR vecTarget = XMVector3TransformCoord(XMLoadFloat3(&target), matInv);
			XMVECTOR vecDir = vecTarget - vecStart;

			XMStoreFloat3(&data->start, vecStart);
			XMStoreFloat3(&data->dir, vecDir);

			_datas[handle]->pFbx->RayCast(data); 
	}

	int AddOrientRotateBone(int handle, std::string boneName, std::string parentBoneName)
	{
		OrientRotateInfo data;

		//BoneIndex�̌v�Z
		data.boneIndex = GetBoneIndex(handle, boneName);
		assert(data.boneIndex >= 0);

		if (!parentBoneName.empty()) {
			data.parentBoneIndex = GetBoneIndex(handle, parentBoneName);
			assert(data.parentBoneIndex >= 0);
		}
		else data.parentBoneIndex = -1;

		data.orientRotate = XMFLOAT3();
		_datas[handle]->orientRotateDatas_.push_back(data);
		int size = (int)_datas[handle]->orientRotateDatas_.size() - 1;
		return size;
	}

	void ResetOrientRotateBone(int handle)
	{
		_datas[handle]->orientRotateDatas_.clear();
	}

	void SetOrietnRotateBone(int handle, int listIndex, XMFLOAT3 rotate)
	{
		_datas[handle]->orientRotateDatas_[listIndex].orientRotate = rotate;
	}

	void GetAllPolygon(int handle, std::vector<PolygonData>& list)
	{
		_datas[handle]->pFbx->GetAllPolygon(list);
	}

	void SetShadow(int handle, bool b)
	{
		_datas[handle]->isShadow = b;
	}

}