#include "jsonReader.h"
#include <string>
#include <fstream>
#include <Windows.h>

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
            //�t�@�C�����J���Ȃ��ꍇ�̃G���[�n���h�����O
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
            //�����f�[�^���Ȃ���json���ƔF���ł��Ȃ���̑΍�
            OutputDebugString("Some kind of error");
            assert(false);
            return;
        }

        if (j.empty())
        {
            //JSON����̏ꍇ�̃G���[�n���h�����O
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