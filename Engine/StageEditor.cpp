#include "StageEditor.h"
#include "../Json/JsonReader.h"
#include "../Stage/CollisionMap.h"
#include "../AI/Node.h"
#include "../Engine/Model.h"
#include "../Engine/ImGui/imgui.h"
#include "../Engine/ImGui/imgui_impl_dx11.h"
#include "../Engine/ImGui/imgui_impl_win32.h"

#include <fstream>
using namespace std;

namespace StageEditor {
    CollisionMap* pCMap = nullptr;

}

std::vector<StageModelData> StageEditor::LoadFileStage(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        // ファイルが開けない場合のエラーハンドリング
        return {};
    }

    nlohmann::json j;
    try
    {
        ifs >> j;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        // JSONのパースエラーのエラーハンドリング
        return {};
    }

    if (j.empty())
    {
        // JSONが空の場合のエラーハンドリング
        return {};
    }

    std::vector<StageModelData> stage;
    for (const auto& objJson : j["objects"])
    {
        StageModelData obj;
        obj.fileName = objJson["fileName"];
        obj.hRayModelNum = Model::Load(obj.fileName);
        obj.transform.position_ = { objJson["position"]["x"], objJson["position"]["y"], objJson["position"]["z"] };
        obj.transform.scale_ = { objJson["scale"]["x"], objJson["scale"]["y"], objJson["scale"]["z"] };
        obj.transform.rotate_ = { objJson["rotate"]["x"], objJson["rotate"]["y"], objJson["rotate"]["z"] };
        stage.push_back(obj);
    }

    return stage;
}

void StageEditor::SaveFileStage(const std::vector<StageModelData>& stage, const std::string& fileName)
{
    nlohmann::json j;
    for (const auto& obj : stage)
    {
        nlohmann::json objJson;
        objJson["fileName"] = obj.fileName;
        objJson["position"] = { {"x", obj.transform.position_.x}, {"y", obj.transform.position_.y}, {"z", obj.transform.position_.z} };
        objJson["scale"] = { {"x", obj.transform.scale_.x}, {"y", obj.transform.scale_.y}, {"z", obj.transform.scale_.z} };
        objJson["rotate"] = { {"x", obj.transform.rotate_.x}, {"y", obj.transform.rotate_.y}, {"z", obj.transform.rotate_.z} };
        j["objects"].push_back(objJson);
    }

    std::ofstream ofs(fileName);
    if (!ofs.is_open())
    {
        assert(false);
        return;
    }

    ofs << j.dump(4);
}

void StageEditor::SetCollisionMap(CollisionMap* map)
{
    pCMap = map;
}

void StageEditor::DrawStageEditor()
{
    if (!pCMap) return;

    //パスの数カウント用
    int count = 0;

    std::vector<StageModelData> &stageList = pCMap->GetModelList();
    ImGui::Begin("StageEditor");
    
    //セーブボタン
    if (ImGui::Button("CreateCollisionMap")) {
        SaveFileStage(stageList, "TestStage.json");
        pCMap->IntersectDataReset();
        pCMap->CreatIntersectDataTriangle();
    }
    
    //区切り線
    ImGui::Separator();

    // ボタンの有効/無効を制御
    static char input[256] = "";
    int modelHandle = -1;
    if (ImGui::InputText("FileName", input, IM_ARRAYSIZE(input))) {
        modelHandle = Model::Load(input);
    }

    //ボタンを無効にする
    bool isButtonEnabled = strlen(input) > 0;
    if (!isButtonEnabled) ImGui::BeginDisabled();
    
    // セーブボタン
    if (ImGui::Button("AddObject")) {
        StageModelData data = StageModelData();
        data.fileName = input;
        stageList.push_back(data);
        input[0] = '\0';
        isButtonEnabled = true;
    }

    if (!isButtonEnabled) {
        ImGui::EndDisabled(); // 無効化を終了
        Model::Release(modelHandle);
    }

    //区切り線
    ImGui::Separator();

    for (int index = 0; index < stageList.size(); ++index)
    {
        StageModelData& modelData = stageList[index];

        // バッファサイズを増やす
        char name[256];
        sprintf_s(name, sizeof(name), "number : %d %s", count++, modelData.fileName.c_str());

        if (ImGui::TreeNode(name)) {
            const float PosMaxValue = 100.0f;
            const float ScaMaxValue = 10.0f;
            const float RotMaxValue = 360.0f;

            //Positionセット
            if (ImGui::TreeNode("Position")) {
                XMFLOAT3 pos = modelData.transform.position_;
                ImGui::SliderFloat("x", &pos.x, 0.0f, PosMaxValue);
                ImGui::SliderFloat("y", &pos.y, 0.0f, PosMaxValue);
                ImGui::SliderFloat("z", &pos.z, 0.0f, PosMaxValue);
                ImGui::TreePop();
                modelData.transform.position_ = pos;
            }

            //Scaleセット
            if (ImGui::TreeNode("Scale")) {
                XMFLOAT3 sca = modelData.transform.scale_;
                ImGui::SliderFloat("x", &sca.x, 0.0f, ScaMaxValue);
                ImGui::SliderFloat("y", &sca.y, 0.0f, ScaMaxValue);
                ImGui::SliderFloat("z", &sca.z, 0.0f, ScaMaxValue);
                ImGui::TreePop();
                modelData.transform.scale_ = sca;
            }

            //Rotateセット
            if (ImGui::TreeNode("Rotate")) {
                XMFLOAT3 rot = modelData.transform.rotate_;
                ImGui::SliderFloat("x", &rot.x, 0.0f, RotMaxValue);
                ImGui::SliderFloat("y", &rot.y, 0.0f, RotMaxValue);
                ImGui::SliderFloat("z", &rot.z, 0.0f, RotMaxValue);
                ImGui::TreePop();
                modelData.transform.rotate_ = rot;
            }

            //削除ボタン
            if (ImGui::Button("Remove")) {
                stageList.erase(stageList.begin() + index);
                --index;
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();

}

//----------------------------------------------------------------------------------

std::vector<Node*> StageEditor::LoadFileNode(const std::string& fileName)
{
    std::ifstream ifs(fileName);
    if (!ifs.is_open())
    {
        // ファイルが開けない場合のエラーハンドリング
        return {};
    }

    nlohmann::json j;
    try
    {
        ifs >> j;
    }
    catch (const nlohmann::json::parse_error& e)
    {
        // JSONのパースエラーのエラーハンドリング
        return {};
    }

    if (j.empty())
    {
        // JSONが空の場合のエラーハンドリング
        return {};
    }

    std::vector<Node*> nodes;
    std::unordered_map<int, Node*> nodeMap;

    for (const auto& nodeJson : j["nodes"]) {
        int id = nodeJson["id"];
        XMFLOAT3 position = {
            nodeJson["position"]["x"],
            nodeJson["position"]["y"],
            nodeJson["position"]["z"]
        };
        Node* node = new Node(id, position);
        //if (nodeJson["type"] == "jump") { node = new JumpNode(id, position); }
        
        nodes.push_back(node);
        nodeMap[id] = node;
    }
    
    for (const auto& edgeJson : j["edges"]) {
        int from = edgeJson["from"];
        int to = edgeJson["to"];
        float cost = edgeJson["cost"];
        if (nodeMap.find(from) != nodeMap.end() && nodeMap.find(to) != nodeMap.end()) {
            nodeMap[from]->GetEdges().push_back({ to, cost });
        }
    }

    return nodes;
}

void StageEditor::SaveFileNode(std::vector<Node*>& nodes, const std::string& fileName)
{
    nlohmann::json j;
    for (auto& node : nodes) {
        nlohmann::json nodeJson;
        nodeJson["id"] = node->GetId();
        nodeJson["position"] = {
            {"x", node->GetPosition().x},
            {"y", node->GetPosition().y},
            {"z", node->GetPosition().z}
        };
        nodeJson["type"] = "normal";
        j["nodes"].push_back(nodeJson);
    }

    for (auto& node : nodes) {
        for (auto& edge : node->GetEdges()) {
            nlohmann::json edgeJson;
            edgeJson["from"] = node->GetId();
            edgeJson["to"] = edge.connectId;
            edgeJson["cost"] = edge.cost;
            j["edges"].push_back(edgeJson);
        }
    }
    std::ofstream file(fileName);
    file << j.dump(4);
}

#include "../AI/RouteSearch.h"
void StageEditor::DrawNodeEditor()
{
    std::vector<Node*>& nodeList = RouteSearch::GetNodeList();
    ImGui::Begin("StageNodeEditor");

    //セーブボタン
    if (ImGui::Button("Save NodeList")) {
        SaveFileNode(nodeList, "TestStageNode.json");
    }

    //Node追加ボタン
    if (ImGui::Button("Add Node")) {
        Node* data = new Node((int)nodeList.size(), XMFLOAT3(50.0f, 5.0f, 50.0f));
        nodeList.push_back(data);
    }

    //地面にくっつかせる
    if (ImGui::Button("StickToGround")) {
        for (int index = 0; index < nodeList.size(); ++index)
        {
            Node* modelData = nodeList[index];
            XMFLOAT3 cellPos = modelData->GetPosition();
            RayCastData rayData = RayCastData();
            rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
            rayData.start = cellPos;
            pCMap->CellFloarRayCast(cellPos, &rayData);
            float minDist = rayData.dist;

            rayData = RayCastData();
            rayData.dir = XMFLOAT3(0.0f, -1.0f, 0.0f);
            rayData.start = cellPos;
            cellPos.y -= CollisionMap::boxSize;
            pCMap->CellFloarRayCast(cellPos, &rayData);
            if (rayData.dist < minDist) minDist = rayData.dist;

            const float ErrorValue = 0.00001f;  //誤差
            if (minDist <= 100.0f) {
                XMFLOAT3 newPos = modelData->GetPosition();
                newPos.y -= minDist;
                modelData->SetPosition(newPos);
            }
        }
    }

    //区切り線
    ImGui::Separator();

    for (int index = 0; index < nodeList.size(); ++index)
    {
        Node* modelData = nodeList[index];

        // バッファサイズを増やす
        char name[256];
        sprintf_s(name, sizeof(name), "id : %d", modelData->GetId());

        if (ImGui::TreeNode(name)) {
            const float PosMaxValue = 100.0f;

            XMFLOAT3 pos = modelData->GetPosition();
            ImGui::SliderFloat("x", &pos.x, 0.0f, PosMaxValue);
            ImGui::SliderFloat("y", &pos.y, 0.0f, PosMaxValue);
            ImGui::SliderFloat("z", &pos.z, 0.0f, PosMaxValue);
            modelData->SetPosition(pos);

            //区切り線
            ImGui::Separator();

            //Edge追加
            std::vector<Edge>& edgeList = modelData->GetEdges();
            if (ImGui::Button("Add Edge")) {
                Edge edge = Edge();
                edgeList.push_back(edge);
            }

            for (int i = 0; i < (int)edgeList.size(); i++) {
                char edgeName[256];
                sprintf_s(edgeName, "Remove Edge %d##%d_%d", i, index, i);

                //Edge削除
                if (ImGui::Button(edgeName)) {
                    edgeList.erase(edgeList.begin() + i);
                    --i;
                    continue;
                }

                char costName[256];
                sprintf_s(costName, "cost##%d_%d", index, i);
                ImGui::InputFloat(costName, &edgeList.at(i).cost, 0, 100.0f);

                char connectIdName[256];
                sprintf_s(connectIdName, "connectId##%d_%d", index, i);
                ImGui::InputInt(connectIdName, &edgeList.at(i).connectId, 0, 100);
                
                //区切り線
                ImGui::Separator();
            }

            //削除ボタン
            if (ImGui::Button("Remove Node")) {
                nodeList.erase(nodeList.begin() + index);
                --index;
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();
}
