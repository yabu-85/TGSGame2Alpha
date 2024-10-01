#include "Global.h"
#include "Model.h"
#include "../Other/GameManager.h"
#include <assert.h>
#include <vector>

//3Dモデル（FBXファイル）を管理する
namespace Model
{
	//ロード済みのモデルデータ一覧
	std::vector<ModelData*>	_datas;

	//初期化
	void Initialize()
	{
		AllRelease();
	}

	//モデルをロード
	int Load(std::string fileName)
	{
			ModelData* pData = new ModelData;

			//開いたファイル一覧から同じファイル名のものが無いか探す
			bool isExist = false;
			for (int i = 0; i < _datas.size(); i++)
			{
				//すでに開いている場合
				if (_datas[i] != nullptr && _datas[i]->fileName == fileName)
				{
					pData->pFbx = _datas[i]->pFbx;
					isExist = true;
					break;
				}
			}

			//新たにファイルを開く
			if (isExist == false)
			{
				pData->pFbx = new Fbx;
				if (FAILED(pData->pFbx->Load(fileName)))
				{
					//開けなかった
					SAFE_DELETE(pData->pFbx);
					SAFE_DELETE(pData);
					return -1;
				}

				//無事開けた
				pData->fileName = fileName;
			}


			//使ってない番号が無いか探す
			for (int i = 0; i < _datas.size(); i++)
			{
				if (_datas[i] == nullptr)
				{
					_datas[i] = pData;
					return i;
				}
			}

			//新たに追加
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
			//アニメーションを進める
			_datas[handle]->nowFrame += _datas[handle]->animSpeed;

			//最後までアニメーションしたら戻す
			if (_datas[handle]->nowFrame > (float)_datas[handle]->endFrame) {
				if(_datas[handle]->isAnimLoop) _datas[handle]->nowFrame = (float)_datas[handle]->startFrame;
				else _datas[handle]->nowFrame = (float)_datas[handle]->endFrame;
			}
		}
	}

	//描画
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


	//任意のモデルを開放
	void Release(int handle)
	{
		if (handle < 0 || handle >= _datas.size() || _datas[handle] == nullptr)
		{
			return;
		}

		//同じモデルを他でも使っていないか
		bool isExist = false;
		for (int i = 0; i < _datas.size(); i++)
		{
			//すでに開いている場合
			if (_datas[i] != nullptr && i != handle && _datas[i]->pFbx == _datas[handle]->pFbx)
			{
				isExist = true;
				break;
			}
		}

		//使ってなければモデル解放
		if (isExist == false )
		{
			SAFE_DELETE(_datas[handle]->pFbx);
		}


		SAFE_DELETE(_datas[handle]);
	}


	//全てのモデルを解放
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


	//アニメーションのフレーム数をセット
	void SetAnimFrame(int handle, int startFrame, int endFrame, float animSpeed)
	{
		_datas[handle]->SetAnimFrame(startFrame, endFrame, animSpeed);
	}


	//現在のアニメーションのフレームを取得
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
		//相対座標（ボーンの中心からの位置）
		XMFLOAT3 pos = _datas[handle]->pFbx->GetBoneAnimPosition(partIndex, boneIndex, (int)_datas[handle]->nowFrame, _datas[handle]->orientRotateDatas_);
		XMVECTOR vec = XMVector3TransformCoord(XMLoadFloat3(&pos), _datas[handle]->transform.GetWorldMatrix()); //posをワールドマトリックスで計算する
		XMStoreFloat3(&pos, vec);
		return pos;
	}

	XMFLOAT3 GetBoneAnimRotate(int handle, int partIndex, int boneIndex)
	{
		//相対座標（ボーンの中心からの位置）
		XMFLOAT3 rot = _datas[handle]->pFbx->GetBoneAnimRotate(partIndex, boneIndex, (int)_datas[handle]->nowFrame);
		if (rot.x >= 90.0f || rot.x <= -90.0f) rot.y *= -1.0f;
		return rot;
	}

	//ワールド行列を設定
	void SetTransform(int handle, Transform & transform)
	{
		if (handle < 0 || handle >= _datas.size())
		{
			return;
		}

		_datas[handle]->transform = transform;
	}


	//ワールド行列の取得
	XMMATRIX GetMatrix(int handle)
	{
		return _datas[handle]->transform.GetWorldMatrix();
	}


	//レイキャスト（レイを飛ばして当たり判定）
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

		//BoneIndexの計算
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