#include "json.hpp"

namespace JsonReader
{
	//JSONファイルの読み込み
	void Load(const std::string& filename);

	nlohmann::json& GetAll();

	//指定したセクションを取得
	const nlohmann::json& GetSection(const std::string& key);
	const nlohmann::json& GetSection(const std::string& key1, const std::string& key2);

};