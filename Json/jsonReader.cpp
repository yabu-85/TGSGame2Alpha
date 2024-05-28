#include "jsonReader.h"

#include <string>
#include <fstream>

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
            //ファイルが開けない
            assert(false);
            return;
        }

        nlohmann::json j;
        try
        {
            //データ読み込み
            ifs >> j;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            //JSONオブジェクトじゃないからエラー
            assert(false);
            return;
        }

        if (j.empty())
        {
            //空の場合
            assert(false);
            return;
        }

        data_ = j;
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