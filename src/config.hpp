#pragma once
#include <vector>
#include "json.hpp"
#include "logger.hpp"

using json = nlohmann::json;

namespace sampnode
{
	struct Props_t
	{
		std::string entry_file;
		std::vector<std::string> node_flags;
		LogLevel log_level = LogLevel::LOG_FULL;
	};

	class Config
	{
	public:
		Config();
		~Config();

		bool ParseFile(const std::string& path);
		bool ParseJsonFile(const std::string& path);

		template<typename T, typename... args>
		T get_as(const args&... keys);

		Props_t ReadAsMainConfig();

	private:
		json jsonObject;
	};
};