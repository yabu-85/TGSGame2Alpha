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
    StageModelData() : hRayModelNum(-1) { }
};

class CollisionMap : public GameObject
{
    int handle_;
    XMFLOAT3 targetPos_;

    Cell*** cells_ = 0;

    //指定したCellのポインタを取得(範囲外ならnullptr)
    Cell* GetCell(XMFLOAT3 pos);

    //Trianglesを範囲内のCellにセット
    void SetCellTriangle(Triangle t);

    std::vector<StageModelData> modelList_;

public:
    CollisionMap(GameObject* parent);
    ~CollisionMap();
    void Initialize() override;
    void Update() override;
    void Draw() override;
    void Release() override;

    //Stageからデータ取得してTriangle作成
    void CreatIntersectDataTriangle();
    //作ったCollisionMapリセット
    void IntersectDataReset();

    //Triangleに当たった場合の最小距離を返す
    bool CellFloarRayCast(XMFLOAT3 plaPos, RayCastData* _data);
    bool CellWallRayCast(XMFLOAT3 plaPos, RayCastData* _data);

    //壁と天井に対して判定する
    bool CellSphereVsTriangle(SphereCollider* collid, XMVECTOR& push);

    //ゲット関数
    std::vector<StageModelData>& GetModelList() { return modelList_; }

};

