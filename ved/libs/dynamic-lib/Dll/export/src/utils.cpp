#include "utils.h"
#include "json.hpp"

using namespace utils;
using namespace std;
using namespace nlohmann;

string
utils::convert_mounted_disk_to_json(const vector<disk_info>& disks)
{

	if (disks.empty())
		return "";

	json j_array = json::array();
	for (const auto& [FileSize,
		DriveLetter,
		FileNameLength,
		PasswordLength,
		Password,
		FileName,
		CryptMode] : disks) {


		auto path = wstring(FileName);
		remove_unused_prefix_from_path(path);

		json obj{};

		obj["FilePath"] = string(path.cbegin(), path.cend());
		obj["FileSize"] = FileSize.QuadPart / 1024 / 1024;
		obj["CryptMode"] = CryptMode;
		obj["FilePath"] = string(path.cbegin(), path.cend());
		obj["Letter"] = static_cast<char>(DriveLetter);

		j_array.push_back(obj);
	}

	return j_array.dump();
}