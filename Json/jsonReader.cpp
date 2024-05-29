#include "jsonReader.h"
#include <string>
#include <fstream>
#include <Windows.h>

namespace JsonReader
{
    // JSONデータ
    nlohmann::json data_;

    // JSONファイルの読み込み
    void Load(const std::string& fileName)
    {
        std::ifstream ifs(fileName);
        if (!ifs.is_open())
        {
            //ファイルが開けない場合のエラーハンドリング
            OutputDebugString("Not open file");
            assert(false);
            return;
        }

        nlohmann::json j;
        try
        {
            ifs >> j;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            //何もデータがないとjsonだと認識できないやつの対策
            OutputDebugString("Some kind of error");
            assert(false);
            return;
        }

        if (j.empty())
        {
            //JSONが空の場合のエラーハンドリング
            OutputDebugString("Json none");
            assert(false); 
            return;
        }

        data_ = j;
    }

    nlohmann::json& GetAll()
    {
        return data_;
    }

    const nlohmann::json& GetSection(const std::string& key)
    {
        return data_[key];
    }

    const nlohmann::json& GetSection(const std::string& key1, const std::string& key2)
    {
        return data_[key1][key2];
    }
}