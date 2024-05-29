#include "json.hpp"

namespace JsonReader
{
	//JSON�t�@�C���̓ǂݍ���
	void Load(const std::string& filename);

	nlohmann::json& GetAll();

	//�w�肵���Z�N�V�������擾
	const nlohmann::json& GetSection(const std::string& key);
	const nlohmann::json& GetSection(const std::string& key1, const std::string& key2);

};