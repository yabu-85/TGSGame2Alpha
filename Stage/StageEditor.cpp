#include "StageEditor.h"
#include "CollisionMap.h"
#include "../Json/JsonReader.h"
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
        obj.hRayModelNum = Model::Load(objJson["fileName"]);
        obj.transform.position_ = { objJson["position"]["x"], objJson["position"]["y"], objJson["position"]["z"] };
        obj.transform.scale_ = { objJson["scale"]["x"], objJson["scale"]["y"], objJson["scale"]["z"] };
        obj.transform.rotate_ = { objJson["rotate"]["x"], objJson["rotate"]["y"], objJson["rotate"]["z"] };
        stage.push_back(obj);
    }

    return stage;
}

void StageEditor::SaveFileStage(const std::vector<LoadStageModel>& stage, const std::string& fileName)
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

    std::vector<StageModelData> list = pCMap->GetModelList();
    ImGui::Begin("CreateHermiteSplinePath", NULL);
    for (auto i = list.begin(); i != list.end(); i++)
    {
        //iをPathの後ろにたす
        char name[16];
        sprintf_s(name, "Path %d", count++);
        
        if (ImGui::TreeNode(name)) {

            XMFLOAT3 pos = (*i).transform.position_;

            //Positionセット
            ImGui::SliderFloat("x", &pos.x, -200.0f, 200.0f);
            ImGui::SliderFloat("y", &pos.y, -200.0f, 200.0f);
            ImGui::SliderFloat("z", &pos.z, -200.0f, 200.0f);

            if (ImGui::TreeNode("InputPosition")) {

                ImGui::Text("x");
                ImGui::InputFloat("x", &pos.x, -200.0f, 200.0f);
                ImGui::Text("y");
                ImGui::InputFloat("y", &pos.y, -200.0f, 200.0f);
                ImGui::Text("z");
                ImGui::InputFloat("z", &pos.z, -200.0f, 200.0f);

                ImGui::TreePop();
            }

            //位置更新
            (*i).transform.position_ = pos;

            ImGui::TreePop();
        }
    }

    //終わり
    ImGui::End();
}