#include "StageEditor.h"
#include "../Json/JsonReader.h"
#include "../Stage/CollisionMap.h"
#include "../Player/Player.h"
#include "../AI/Node.h"
#include "../Engine/Model.h"
#include "../Engine/ImGui/imgui.h"
#include "../Engine/ImGui/imgui_impl_dx11.h"
#include "../Engine/ImGui/imgui_impl_win32.h"
#include "../Engine/Global.h"

#include <fstream>
using namespace std;

namespace StageEditor {
    CollisionMap* pCMap = nullptr;
    Player* pPlayer = nullptr;
}

std::vector<StageModelData> StageEditor::LoadFileStage(const std::string& fileName)
{
    JsonReader::Load(fileName);

    std::vector<StageModelData> stage;
    for (const auto& objJson : JsonReader::GetAll()["objects"])
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
            const float PosMaxValue = 70.0f;            
            const float PosMinValue = 30.0f;
            const float ScaMaxValue = 10.0f;
            const float RotMaxValue = 360.0f;

            //Positionセット
            ImGui::Columns(3);
            XMFLOAT3 pos = modelData.transform.position_;
            ImGui::SliderFloat("Px", &pos.x, PosMinValue, PosMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("Py", &pos.y, 0, 30.0f);
            ImGui::NextColumn();
            ImGui::SliderFloat("Pz", &pos.z, PosMinValue, PosMaxValue);
            modelData.transform.position_ = pos;
            ImGui::Columns(1);

            //Scaleセット
            ImGui::Columns(3);
            XMFLOAT3 sca = modelData.transform.scale_;
            ImGui::SliderFloat("Sx", &sca.x, 0.0f, ScaMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("Sy", &sca.y, 0.0f, ScaMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("Sz", &sca.z, 0.0f, ScaMaxValue);
            modelData.transform.scale_ = sca;
            ImGui::Columns(1);

            //Rotateセット
            ImGui::Columns(3);
            XMFLOAT3 rot = modelData.transform.rotate_;
            ImGui::SliderFloat("Rx", &rot.x, 0.0f, RotMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("Ry", &rot.y, 0.0f, RotMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("Rz", &rot.z, 0.0f, RotMaxValue);
            modelData.transform.rotate_ = rot;
            ImGui::Columns(1);

            //削除ボタン
            if (ImGui::Button("Remove")) {
                stageList.erase(stageList.begin() + index);
                --index;
            }
            ImGui::SameLine();
            //コピーボタン
            if (ImGui::Button("Copy&Paste")) {
                StageModelData data = *(stageList.begin() + index);
                stageList.push_back(data);
            }

            ImGui::TreePop();
        }
    }
    ImGui::End();

}

//----------------------------------------------------------------------------------

std::vector<Node*> StageEditor::LoadFileNode(const std::string& fileName)
{
    JsonReader::Load(fileName);

    std::vector<Node*> nodes;
    std::unordered_map<int, Node*> nodeMap;

    //Node本体の追加
    for (const auto& nodeJson : JsonReader::GetAll()["nodes"]) {
        int id = nodeJson["id"];
        XMFLOAT3 position = {
            nodeJson["position"]["x"],
            nodeJson["position"]["y"],
            nodeJson["position"]["z"]
        };
        Node* node = new Node(id, position);
        nodes.push_back(node);
        nodeMap[id] = node;
    }
    
    //Edgeの追加
    for (const auto& edgeJson : JsonReader::GetAll()["edges"]) {
        EdgeType type = EdgeType::NORMAL;
        if(edgeJson["type"] == "jump") type = EdgeType::JUMP;
        int from = edgeJson["from"];
        int to = edgeJson["to"];
        float cost = edgeJson["cost"];
        if (nodeMap.find(from) != nodeMap.end() && nodeMap.find(to) != nodeMap.end()) {
            nodeMap[from]->GetEdges().push_back({ to, cost, type });
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
        j["nodes"].push_back(nodeJson);
    }

    for (auto& node : nodes) {
        for (auto& edge : node->GetEdges()) {
            nlohmann::json edgeJson;
            if(edge.type == EdgeType::NORMAL) edgeJson["type"] = "normal";
            else if(edge.type == EdgeType::JUMP) edgeJson["type"] = "jump";
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
    ImGui::SameLine();
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

    //Node追加ボタン
    if (ImGui::Button("Add Node")) {
        Node* data = new Node((int)nodeList.size(), XMFLOAT3(50.0f, 7.0f, 50.0f));
        nodeList.push_back(data);
    }
    ImGui::SameLine();
    if (ImGui::Button("Add Node Player Position")) {
        Node* data = new Node((int)nodeList.size(), pPlayer->GetPosition());
        nodeList.push_back(data);
    }

    //相互接続
    static bool interconnection = false;
    static int preNodeId = -1;
    if (interconnection) {
        ImGui::Text("InterConnection On , PreId : %d", preNodeId);
        XMFLOAT3 pPos = pPlayer->GetPosition();
        for (int i = 0; i < (int)nodeList.size(); i++) {
            XMFLOAT3 nPos = nodeList.at(i)->GetPosition();
            float dist = CalculationDistance(pPos, nPos);
            if (dist <= 1.0f) { 

                //距離範囲内だから、相互接続させる
                if (preNodeId != i && preNodeId != -1) {
                    //プレイヤーと距離が近い奴
                    bool exist = false;
                    for (int j = 0; j < (int)nodeList.at(i)->GetEdges().size(); j++) {
                        int id = nodeList.at(i)->GetEdges().at(j).connectId;
                        if (id == preNodeId) {
                            exist = true;
                            break;
                        }
                    }
                    if (!exist) {
                        Edge edge = Edge(EdgeType::NORMAL);
                        edge.connectId = preNodeId;
                        nodeList.at(i)->GetEdges().push_back(edge);
                    }

                    //前に接触したPreNode
                    exist = false;
                    for (int j = 0; j < (int)nodeList.at(preNodeId)->GetEdges().size(); j++) {
                        int id = nodeList.at(preNodeId)->GetEdges().at(j).connectId;
                        if (id == i) {
                            exist = true;
                            break;
                        }
                    }
                    if (!exist) {
                        Edge edge = Edge(EdgeType::NORMAL);
                        edge.connectId = i;
                        nodeList.at(preNodeId)->GetEdges().push_back(edge);
                    }
                }
                preNodeId = i; 
                break;
            }
        }
    }
    else ImGui::Text("InterConnection Off");
    //相互接続、切り替え
    ImGui::SameLine();
    if (ImGui::Button("Change Mode")) {
        interconnection = !interconnection;
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
            const float PosMinValue = 30.0f;
            const float PosMaxValue = 70.0f;

            ImGui::SameLine();

            //削除ボタン
            if (ImGui::Button("Remove Node")) {
                nodeList.erase(nodeList.begin() + index);
                --index;
            }

            ImGui::Columns(3);
            XMFLOAT3 pos = modelData->GetPosition();
            ImGui::SliderFloat("x", &pos.x, PosMinValue, PosMaxValue);
            ImGui::NextColumn();
            ImGui::SliderFloat("y", &pos.y, 0, 30.0f);
            ImGui::NextColumn(); 
            ImGui::SliderFloat("z", &pos.z, PosMinValue, PosMaxValue);
            modelData->SetPosition(pos);
            ImGui::Columns(1);

            ImGui::Separator();

            //Edge追加
            std::vector<Edge>& edgeList = modelData->GetEdges();

            //列を2つに分割
            if (ImGui::Button("Add Edge Normal")) {
                Edge edge = Edge(EdgeType::NORMAL);
                edgeList.push_back(edge);
            }
            ImGui::SameLine();
            if (ImGui::Button("Add Edge Jump")) {
                Edge edge = Edge(EdgeType::JUMP);
                edgeList.push_back(edge);
            }

            for (int i = 0; i < (int)edgeList.size(); i++) {
                char edgeName[256];
                if (edgeList.at(i).type == EdgeType::NORMAL) sprintf_s(edgeName, "Edge %d type: Normal", i);
                else if (edgeList.at(i).type == EdgeType::JUMP) sprintf_s(edgeName, "Edge %d type: Jump", i);

                if (ImGui::TreeNode(edgeName)) {
                    char costName[256];
                    sprintf_s(costName, "cost##%d_%d", index, i);
                    ImGui::InputFloat(costName, &edgeList.at(i).cost, 0, 100.0f);

                    char connectIdName[256];
                    sprintf_s(connectIdName, "connectId##%d_%d", index, i);
                    ImGui::InputInt(connectIdName, &edgeList.at(i).connectId, 0, 100);
                    
                    //Edge削除
                    if (ImGui::Button("Remove Edge")) {
                        edgeList.erase(edgeList.begin() + i);
                        --i;
                        continue;
                    }

                    ImGui::TreePop();
                }

            }
            
            ImGui::Separator();
            ImGui::TreePop();
        }
    }
    ImGui::End();
}

//---------------------------------------------------------------------

void StageEditor::SetCollisionMap(CollisionMap* map)
{
    pCMap = map;
}

void StageEditor::SetPlayer(Player* player)
{
    pPlayer = player;
}
