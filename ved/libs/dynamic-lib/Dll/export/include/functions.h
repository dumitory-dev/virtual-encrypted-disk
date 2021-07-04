#pragma once
#include "json.hpp"
#include "safecall.h"
#include "utils.h"

#define IMLP_TEMPLATE_FUNC_VED_API(name)                                       \
  template<typename... Args>                                                   \
  bool name(Args... param) noexcept                                            \
  {                                                                            \
    return safe_call(&ved::i_ved_manager::name, param...);                     \
  }

IMLP_TEMPLATE_FUNC_VED_API(un_mount)
IMLP_TEMPLATE_FUNC_VED_API(mount)
IMLP_TEMPLATE_FUNC_VED_API(mount_ex)
IMLP_TEMPLATE_FUNC_VED_API(create_file)
IMLP_TEMPLATE_FUNC_VED_API(run_driver)
IMLP_TEMPLATE_FUNC_VED_API(copy_image)

template<typename... Args>
const wchar_t*
get_error(Args...) noexcept
{
	return cur_error.c_str();
}

template<typename... Args>
bool
get_mounted_disks(char* buffer, int* len) noexcept
{
	return noexcept_call([=]() {
		if (!check_arguments(buffer, len))
			throw std::exception("Invalid input argument(null pointer)");

		const auto json_dump =
			utils::convert_mounted_disk_to_json(manager->get_mounted_disks());

		if (*len <= json_dump.size()) {
			*len = json_dump.size() + 1;
			throw std::exception("Too small input buffer!");
		}

		memcpy(buffer, json_dump.c_str(), json_dump.size());
		buffer[json_dump.size()] = '\0';
		});
}
