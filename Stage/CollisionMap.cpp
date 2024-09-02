#include "CollisionMap.h"
#include "Cell.h"
#include "Stage.h"
#include "Triangle.h"
#include "StageEditor.h"
#include <vector>

#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SphereCollider.h"
#include "../Other/GameManager.h"

namespace {
    float minX = 0;
    float maxX = 100;
    float minY = 0;
    float maxY = 20; 
    float minZ = 0;
    float maxZ = 100;

    //Cellの数
    int numX = 0;
    int numY = 0;
    int numZ = 0;
}

const float CollisionMap::boxSize = 5.0f;

StageModelData::StageModelData() : hModelNum(-1), hRayModelNum(-1), transform{ }
{
    transform.position_ = XMFLOAT3(50.0f, 5.0f, 50.0f);
    rayFileName = "none";
}

//---------------------------------------------------------------------------------

CollisionMap::CollisionMap(GameObject* parent) : GameObject(parent, "CollisionMap"), handle_(-1)
{
    GameManager::SetCollisionMap(this);

}

CollisionMap::~CollisionMap()
{
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            delete[] cells_[y][z];
        }
        delete[] cells_[y];
    }
    delete[] cells_;
}

void CollisionMap::Initialize()
{
    handle_ = Model::Load("DebugCollision/BoxCollider.fbx");
    assert(handle_ >= 0);

    numX = int((abs(maxX) + abs(minX)) / boxSize) + 1;
    numY = int((abs(maxY) + abs(minY)) / boxSize) + 1;
    numZ = int((abs(maxZ) + abs(minZ)) / boxSize) + 1;

    cells_ = new Cell * *[numY];
    for (int y = 0; y < numY; y++) {
        cells_[y] = new Cell * [numZ];
        for (int z = 0; z < numZ; z++) {
            cells_[y][z] = new Cell[numX];
        }
    }

    //「各CELLの左奥下」の座標を設定
    // 最も「左奥下」のセルの座標はMinX,MinZ,MinY（ここが基準）
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            for (int x = 0; x < numX; x++) {
                cells_[y][z][x].SetPosLeng(XMFLOAT3(minX + boxSize * x, minY + boxSize * y, minZ + boxSize * z), boxSize);
            }
        }
    }
}

void CollisionMap::Update()
{
}

void CollisionMap::Draw()
{
    Direct3D::SHADER_TYPE type = Direct3D::GetCurrentShader();
    Direct3D::SetShader(Direct3D::SHADER_UNLIT);

    //CellBox
#if 0
    Transform trans = transform_;
    trans.position_.x = int((Direct3D::PlayerPosition.x - minX) / boxSize) * boxSize;
    trans.position_.y = int((Direct3D::PlayerPosition.y - minY) / boxSize) * boxSize;
    trans.position_.z = int((Direct3D::PlayerPosition.z - minZ) / boxSize) * boxSize;
    trans.position_.x += boxSize * 0.5f;
    trans.position_.y += boxSize * 0.5f;
    trans.position_.z += boxSize * 0.5f;
    trans.scale_ = XMFLOAT3(boxSize, boxSize, boxSize);
    Model::SetTransform(handle_, trans);
    Model::Draw(handle_);
#endif

#if 0
    Cell* pCell = GetCell(Direct3D::PlayerPosition);
    if (pCell) {
        OutputDebugString("Floar triangles : ");
        OutputDebugStringA(std::to_string(GetCell(Direct3D::PlayerPosition)->GetFloarTriangles().size()).c_str());
        OutputDebugString("\nWall  triangles : ");
        OutputDebugStringA(std::to_string(GetCell(Direct3D::PlayerPosition)->GetWallTriangles().size()).c_str());
        OutputDebugString("\n\n");
    }
#endif
    
    //Collision表示
#if 1
    if (type != Direct3D::SHADER_SHADOWMAP) 
    for (auto e : modelList_) {
        Transform t = e.transform;
        t.scale_.x = -t.scale_.x;

        if (e.hRayModelNum >= 0) {
            Model::SetTransform(e.hRayModelNum, t);
            Model::Draw(e.hRayModelNum);
        }
        else {
            Model::SetTransform(e.hModelNum, t);
            Model::Draw(e.hModelNum);
        }
    }
#endif

    Direct3D::SetShader(type);

    //普通のマップ表示
#if 1
    for (auto e : modelList_) {
        Model::SetTransform(e.hModelNum, e.transform);
        Model::Draw(e.hModelNum);
    }
#endif

}

void CollisionMap::Release()
{
}

void CollisionMap::SetStageModelList(std::string fileName)
{
    modelList_ = StageEditor::LoadFileStage(fileName);
}

void CollisionMap::StageModelListReset()
{
    modelList_.clear();
}

void CollisionMap::CreatIntersectDataTriangle()
{
    for (int i = 0; i < modelList_.size(); i++) {
        //Collision用のモデルポリゴンを取得
        std::vector<PolygonData> polyList;

        //コリジョン用モデルある場合
        if (modelList_[i].hRayModelNum >= 0) Model::GetAllPolygon(modelList_[i].hRayModelNum, polyList);
        else Model::GetAllPolygon(modelList_[i].hModelNum, polyList);

        //モデルデータの座標を計算
        for (int j = 0; j < polyList.size(); j++) {
            XMFLOAT3 pos = modelList_[i].transform.position_;
            XMFLOAT3 rot = modelList_[i].transform.rotate_;
            XMFLOAT3 sca = modelList_[i].transform.scale_;

            XMMATRIX matTranslate_ = XMMatrixTranslation( pos.x, pos.y, pos.z);
            XMMATRIX matRotate_ = XMMatrixRotationZ(XMConvertToRadians(rot.z)) * XMMatrixRotationX(XMConvertToRadians(rot.x)) * XMMatrixRotationY(XMConvertToRadians(rot.y));
            XMMATRIX matScale_ = XMMatrixScaling(sca.x, sca.y, sca.z);
            XMMATRIX mat = matScale_ * matRotate_ * matTranslate_;
        
            for (int k = 0; k < 3; k++) {
                XMVECTOR posVec = XMLoadFloat3(&polyList[j].position_[k]);
                posVec = XMVector3Transform(posVec, mat);
                XMStoreFloat3(&polyList[j].position_[k], posVec);
            }
        }
        
        //Triangleに割当たるポリゴンを取得しCreatしてリストに追加
        for (int i = 0; i < polyList.size(); i++) {
            Triangle* tri = new Triangle(polyList[i].position_[0], polyList[i].position_[1], polyList[i].position_[2]);
            SetCellTriangle(*tri);
        }
    }
}

void CollisionMap::IntersectDataReset()
{
    //「各CELL」に含まれる三角ポリゴンを登録
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            for (int x = 0; x < numX; x++) {
                Cell* c = &cells_[y][z][x];
                c->ResetTriangles(); // 三角形のリストをクリア
            }
        }
    }
}

bool CollisionMap::CellFloarRayCast(XMFLOAT3 plaPos, RayCastData* _data)
{
    Cell* cell = GetCell(plaPos);
    if (!cell) return false;
    return cell->SegmentVsFloarTriangle(_data);
}

bool CollisionMap::CellWallRayCast(XMFLOAT3 plaPos, RayCastData* _data)
{
    Cell* cell = GetCell(plaPos);
    if (!cell) return false;
    return cell->SegmentVsWallTriangle(_data);
}

bool CollisionMap::CellSphereVsTriangle(SphereCollider* collid, XMVECTOR& push)
{
    //まずCenterの場所で判定
    XMFLOAT3 pos = Float3Add(collid->center_, collid->pGameObject_->GetWorldPosition());
    Cell* cell = GetCell(pos);
    bool hit = false;
    if (!cell) return false;
    return cell->SphereVsTriangle(collid, push);
}

void CollisionMap::RaySelectCellVsSegment(XMFLOAT3 target, RayCastData* _data)
{
    int startX = int((_data->start.x - minX) / boxSize);
    int startY = int((_data->start.y - minY) / boxSize);
    int startZ = int((_data->start.z - minZ) / boxSize);
    int targetX = int((target.x - minX) / boxSize);
    int targetY = int((target.y - minY) / boxSize);
    int targetZ = int((target.z - minZ) / boxSize);

    // 座標の範囲を制限
    startX = (int)max(0, min(startX, maxX / boxSize - 1));
    startY = (int)max(0, min(startY, maxY / boxSize - 1));
    startZ = (int)max(0, min(startZ, maxZ / boxSize - 1));
    targetX = (int)max(0, min(targetX, maxX / boxSize - 1));
    targetY = (int)max(0, min(targetY, maxY / boxSize - 1));
    targetZ = (int)max(0, min(targetZ, maxZ / boxSize - 1));

    int stepX = (targetX >= startX) ? 1 : -1;
    int stepY = (targetY >= startY) ? 1 : -1;
    int stepZ = (targetZ >= startZ) ? 1 : -1;

    // 座標の処理 : 常にStartPositionからループ回したいから座標によって増減を決める
    for (int x = startX; x != targetX + stepX; x += stepX) {
        for (int y = startY; y != targetY + stepY; y += stepY) {
            for (int z = startZ; z != targetZ + stepZ; z += stepZ) {

                // line ベクトルに垂直なベクトルを計算
                XMVECTOR upVector = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // もし他のベクトルが必要なら変更
                XMVECTOR lineNormal = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&_data->dir), upVector));

                const XMFLOAT3 floatBoxPos[4] = {
                    { 0.0f, 0.0f, 0.0f},
                    { boxSize, 0.0f, 0.0f },
                    { boxSize, 0.0f, boxSize },
                    { 0.0f, 0.0f, boxSize }
                };

                // 四角形の四隅の点を求める / その点からstartまでの距離を計算 / 内積により線の方向を計算
                XMVECTOR lineBase = XMLoadFloat3(&_data->start);
                XMVECTOR c[4];
                float dp[4] = {};
                for (int i = 0; i < 4; i++) {
                    // 四角形の四隅の点の座標を求める
                    XMFLOAT3 boxPos = XMFLOAT3(x * boxSize + floatBoxPos[i].x, y * boxSize + floatBoxPos[i].y, z * boxSize + floatBoxPos[i].z);
                    c[i] = XMLoadFloat3(&boxPos);

                    // その点からstartまでの距離を計算
                    c[i] -= lineBase;

                    // 内積により線の方向を求める
                    dp[i] = XMVectorGetY(XMVector3Dot(lineNormal, c[i]));
                }

                // 全部同じ方向にあれば、線と四角形が当たっていることはない
                if ((dp[0] * dp[1] <= 0) || (dp[1] * dp[2] <= 0) || (dp[2] * dp[3] <= 0)) {
                    Cell* cell = &cells_[y][z][x];
                    if (!cell) continue;

                    // Ray内にあったからそのCellで当たり判定をする
                    bool isHitWall = cell->SegmentVsWallTriangle(_data);
                    float wallDist = _data->dist;
                    bool isHitFloar = cell->SegmentVsFloarTriangle(_data);

                    //どれかに当たった時点で終了
                    if (isHitWall || isHitFloar) {
                        //短い距離に合わせる
                        if (_data->dist > wallDist) _data->dist = wallDist;
                        return;
                    }
                }
            }
        }
    }
}

Cell* CollisionMap::GetCell(XMFLOAT3 pos)
{
    int x = int((pos.x - minX) / boxSize);
    int y = int((pos.y - minY) / boxSize);
    int z = int((pos.z - minZ) / boxSize);

    //ここ最大を超えないようにしてるけど、将来なくてもいいように設計したならいらない
    if (x < 0 || y < 0 || z < 0 || x > maxX / boxSize || y > maxY / boxSize || z > maxZ / boxSize) {
        return nullptr;
    }

    return &cells_[y][z][x];
}

void CollisionMap::SetCellTriangle(Triangle t)
{
    //「各CELL」に含まれる三角ポリゴンを登録
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            for (int x = 0; x < numX; x++) {
                cells_[y][z][x].SetTriangle(t);
            }
        }
    }
}
