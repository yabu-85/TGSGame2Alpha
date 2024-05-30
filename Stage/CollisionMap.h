#pragma once
#include "../Engine/GameObject.h"
#include <vector>
#include <string>

class Triangle;
class Cell;
struct RayCastData;

struct StageModelData {
    int hRayModelNum;
    std::string fileName;
    Transform transform;
    StageModelData();
};

class CollisionMap : public GameObject
{
    int handle_;
    XMFLOAT3 targetPos_;

    Cell*** cells_ = 0;

public:
    static const float boxSize; //Cell�̃T�C�Y

private:
    //�w�肵��Cell�̃|�C���^���擾(�͈͊O�Ȃ�nullptr)
    Cell* GetCell(XMFLOAT3 pos);

    //Triangles��͈͓���Cell�ɃZ�b�g
    void SetCellTriangle(Triangle t);

    std::vector<StageModelData> modelList_;

public:

    CollisionMap(GameObject* parent);
    ~CollisionMap();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //Stage����f�[�^�擾����Triangle�쐬
    void CreatIntersectDataTriangle();
    //�����CollisionMap���Z�b�g
    void IntersectDataReset();

    //Triangle�ɓ��������ꍇ�̍ŏ�������Ԃ�
    bool CellFloarRayCast(XMFLOAT3 plaPos, RayCastData* _data);
    bool CellWallRayCast(XMFLOAT3 plaPos, RayCastData* _data);

    //�ǂƓV��ɑ΂��Ĕ��肷��
    bool CellSphereVsTriangle(SphereCollider* collid, XMVECTOR& push);

    //ray.Start����target�̒����`�͈̔͂ŁARay.Dir���ʂ�Cell���v�Z���Ă�����CellRay���肷��
    void RaySelectWallCellVsSegment(XMFLOAT3 target, RayCastData* _data);

    //�Q�b�g�֐�
    std::vector<StageModelData>& GetModelList() { return modelList_; }

};

