#pragma once
#include "../Engine/GameObject.h"
#include <vector>

class Triangle;
class Cell;
struct RayCastData;

struct StageModelData {
    int hRayModelNum;
    Transform transform;
    StageModelData() : hRayModelNum(-1) { }
};

class CollisionMap : public GameObject
{
    int handle_;
    XMFLOAT3 targetPos_;

    Cell*** cells_ = 0;

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

    bool CellRayCast(XMFLOAT3 plaPos, RayCastData* _data);

};

