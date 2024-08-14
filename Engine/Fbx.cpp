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
	// FBXの読み込み
	pFbxManager_ = FbxManager::Create();
	pFbxScene_ = FbxScene::Create(pFbxManager_, "fbxscene");
	FbxString FileName(fileName.c_str());
	FbxImporter *fbxImporter = FbxImporter::Create(pFbxManager_, "imp");
	if (!fbxImporter->Initialize(FileName.Buffer(), -1, pFbxManager_->GetIOSettings()))
	{
		//失敗
		return E_FAIL;
	}
	fbxImporter->Import(pFbxScene_);
	fbxImporter->Destroy();

	// アニメーションのタイムモードの取得
	_frameRate = pFbxScene_->GetGlobalSettings().GetTimeMode();


	//現在のカレントディレクトリを覚えておく
	char defaultCurrentDir[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, defaultCurrentDir);

	//カレントディレクトリをファイルがあった場所に変更
	char dir[MAX_PATH];
	_splitpath_s(fileName.c_str(), nullptr, 0, dir, MAX_PATH, nullptr, 0, nullptr, 0);
	SetCurrentDirectory(dir);



	//ルートノードを取得して
	FbxNode* rootNode = pFbxScene_->GetRootNode();

	//そいつの子供の数を調べて
	int childCount = rootNode->GetChildCount();

	//1個ずつチェック
	for (int i = 0; childCount > i; i++)
	{
		CheckNode(rootNode->GetChild(i), &parts_);
	}



	//カレントディレクトリを元の位置に戻す
	SetCurrentDirectory(defaultCurrentDir);

	return S_OK;
}

void Fbx::CheckNode(FbxNode * pNode, std::vector<FbxParts*>* pPartsList)
{
	//そのノードにはメッシュ情報が入っているだろうか？
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr != nullptr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//パーツを用意
		FbxParts* pParts = new FbxParts;
		pParts->Init(pNode);

		//パーツ情報を動的配列に追加
		pPartsList->push_back(pParts);
	}


	//子ノードにもデータがあるかも！！
	{
		//子供の数を調べて
		int childCount = pNode->GetChildCount();

		//一人ずつチェック
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

	//パーツを1個ずつ描画
	for (int k = 0; k < parts_.size(); k++)
	{
		// その瞬間の自分の姿勢行列を得る
		FbxTime     time;
		time.SetTime(0, 0, 0, frame, 0, 0, _frameRate);

		//スキンアニメーション（ボーン有り）の場合
		if (parts_[k]->GetSkinInfo() != nullptr)
		{
			parts_[k]->DrawSkinAnime(transform, time, orientDatas, isShadow);
		}

		//メッシュアニメーションの場合
		else
		{
			parts_[k]->DrawMeshAnime(transform, time, pFbxScene_, isShadow);
		}
	}
}


//レイキャスト（レイを飛ばして当たり判定）
void Fbx::RayCast(RayCastData * data)
{
	//すべてのパーツと判定
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

	//このノードがメッシュを持っているかどうかを確認
	FbxNodeAttribute* attr = pNode->GetNodeAttribute();
	if (attr && attr->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		//ポリゴンデータがあればリストに追加
		std::vector<PolygonData> poly = parts_[list.size()]->GetAllPolygon(pNode);
		if (poly.empty()) list = poly;
		else list.insert(list.end(), poly.begin(), poly.end());
	}

	//このノードのすべての子ノードに対して再帰的に処理を行う
	for (int i = 0; i < pNode->GetChildCount(); ++i)
	{
		GetAllPolygonRecursive(pNode->GetChild(i), list);
	}
}
