#pragma once

#include "global_variable.h"

template<typename... Types>
bool
check_arguments(Types... args)
{
	const auto check_null_pointer = [](auto param) {
		if constexpr (std::is_pointer_v<std::decay_t<decltype(param)>>) {
			return static_cast<bool>(param);
		}
		else {
			return true;
		}
	};
	auto res{ true };
	return ((res &= check_null_pointer(args)), ...);
}

inline void
set_error(const wchar_t* error)
{
	cur_error = error;
}

template<typename T>
bool
noexcept_call(T&& func) noexcept
{
	try {
		func();
		return true;
	}
	catch (const ved::c_exception& error) {
		cur_error = error.GetMessageW();
	}
	catch (const std::exception& error) {
		const auto str_error = std::string(error.what());
		cur_error = std::wstring{ str_error.cbegin(), str_error.cend() };
	}
	return false;
}

template<typename T, typename... Args>
auto
create_method_wrapper(T&& func, Args&&... args)
{
	return[... args = std::forward<Args>(args), &func]() mutable {
		(manager.get()->*func)(std::forward<Args>(args)...);
	};
}

template<typename T, typename... Args>
auto
safe_call(T&& func, Args&&... args) noexcept
{
	if (!check_arguments(args...)) {
		set_error(L"Invalid input argument(null pointer)");
		return false;
	}
	return noexcept_call(
		create_method_wrapper(std::forward<T>(func), std::forward<Args>(args)...));
}
