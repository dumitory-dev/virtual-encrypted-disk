#pragma once
#include "global_variable.h"
#include <string>

namespace utils {

	using disk_info = OPEN_FILE_INFORMATION;

	inline void
		remove_unused_prefix_from_path(std::wstring& path)
	{
		path.erase(0, 4); // remove ////??////
	}

	std::string
		convert_mounted_disk_to_json(const std::vector<disk_info>& disks);
}
