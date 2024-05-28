#include "jsonReader.h"

#include <string>
#include <fstream>

namespace JsonReader
{
    // JSON�f�[�^
    nlohmann::json data_;

    // JSON�t�@�C���̓ǂݍ���
    void Load(const std::string& fileName)
    {

        std::ifstream ifs(fileName);
        if (!ifs.is_open())
        {
            //�t�@�C�����J���Ȃ�
            assert(false);
            return;
        }

        nlohmann::json j;
        try
        {
            //�f�[�^�ǂݍ���
            ifs >> j;
        }
        catch (const nlohmann::json::parse_error& e)
        {
            //JSON�I�u�W�F�N�g����Ȃ�����G���[
            assert(false);
            return;
        }

        if (j.empty())
        {
            //��̏ꍇ
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