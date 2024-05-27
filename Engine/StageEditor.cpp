#include "StageEditor.h"
#include "../Json/JsonReader.h"
#include "../Stage/CollisionMap.h"
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
        assert(false);
        return {};
    }

    nlohmann::json j;
    ifs >> j;
    if (j.empty())
    {
        assert(false);
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