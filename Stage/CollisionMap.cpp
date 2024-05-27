#include "CollisionMap.h"
#include "Cell.h"
#include "Stage.h"
#include "Triangle.h"
#include <vector>

#include "../Engine/Fbx.h"
#include "../Engine/Model.h"
#include "../Engine/Global.h"
#include "../Engine/Direct3D.h"
#include "../Engine/SphereCollider.h"
#include "../Engine/StageEditor.h"

namespace {
    const float boxSize = 5.0f;
    float minX = 0;
    float maxX = 100;
    float minY = 0;
    float maxY = 20; 
    float minZ = 0;
    float maxZ = 100;

    //Cell�̐�
    int numX = 0;
    int numY = 0;
    int numZ = 0;
}

CollisionMap::CollisionMap(GameObject* parent)
    : GameObject(parent, "CollisionMap"), handle_(-1), targetPos_(XMFLOAT3())
{
    StageEditor::SetCollisionMap(this);
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

    //�u�eCELL�̍������v�̍��W��ݒ�
    // �ł��u�������v�̃Z���̍��W��MinX,MinZ,MinY�i��������j
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
    //Direct3D::SetShader(Direct3D::SHADER_UNLIT);

#if 0
    Transform trans = transform_;
    trans.position_ = targetPos_;
    trans.position_.x += boxSize * 0.5f;
    trans.position_.y += boxSize * 0.5f;
    trans.position_.z += boxSize * 0.5f;
    trans.scale_ = XMFLOAT3(boxSize, boxSize, boxSize);
    Model::SetTransform(handle_, trans);
    Model::Draw(handle_);
#endif

    for (auto e : modelList_) {
        Model::SetTransform(e.hRayModelNum, e.transform);
        Model::Draw(e.hRayModelNum);
    }

    Direct3D::SetShader(type);
}

void CollisionMap::Release()
{
}

void CollisionMap::CreatIntersectDataTriangle()
{
    modelList_ = StageEditor::LoadFileStage("TestStage.json");

    for (int i = 0; i < modelList_.size(); i++) {
        //�R���W�����p���f�����Ȃ��΂�
        if (modelList_[i].hRayModelNum <= -1) continue;
        
        //Collision�p�̃��f���|���S�����擾
        Fbx* pFbx = Model::GetFbx(modelList_[i].hRayModelNum);
        std::vector<PolygonData> polyList;
        pFbx->GetAllPolygon(polyList);

        //���f���f�[�^�̍��W���v�Z
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
        
        //Triangle�Ɋ�������|���S�����擾��Creat���ă��X�g�ɒǉ�
        for (int i = 0; i < polyList.size(); i++) {
            Triangle* tri = new Triangle(polyList[i].position_[0], polyList[i].position_[1], polyList[i].position_[2]);
            SetCellTriangle(*tri);
        }
    }
}

void CollisionMap::IntersectDataReset()
{
    //�u�eCELL�v�Ɋ܂܂��O�p�|���S����o�^
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            for (int x = 0; x < numX; x++) {
                Cell* c = &cells_[y][z][x];
                c->ResetTriangles(); // �O�p�`�̃��X�g���N���A
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
    XMFLOAT3 pos = Float3Add(collid->center_, collid->pGameObject_->GetWorldPosition());
    Cell* cell = GetCell(pos);
    if (!cell) return false;

    return cell->SphereVsTriangle(collid, push);
}

Cell* CollisionMap::GetCell(XMFLOAT3 pos)
{
    int x = int((pos.x - minX) / boxSize);
    int y = int((pos.y - minY) / boxSize);
    int z = int((pos.z - minZ) / boxSize);

    //�����ő�𒴂��Ȃ��悤�ɂ��Ă邯�ǁA�����Ȃ��Ă������悤�ɐ݌v�����Ȃ炢��Ȃ�
    if (x < 0 || y < 0 || z < 0 || x > maxX / boxSize || y > maxY / boxSize || z > maxZ / boxSize) {
        return nullptr;
    }

    targetPos_ = XMFLOAT3((float)x * boxSize, (float)y * boxSize, (float)z * boxSize);
    return &cells_[y][z][x];
}

void CollisionMap::SetCellTriangle(Triangle t)
{
    //�u�eCELL�v�Ɋ܂܂��O�p�|���S����o�^
    for (int y = 0; y < numY; y++) {
        for (int z = 0; z < numZ; z++) {
            for (int x = 0; x < numX; x++) {
                cells_[y][z][x].SetTriangle(t);
            }
        }
    }
}