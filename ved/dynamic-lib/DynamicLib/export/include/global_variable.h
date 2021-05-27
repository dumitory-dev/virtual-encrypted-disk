#pragma once
#include "i_ved_manager.h"
#include "ved_manager/ved_manager.h"

inline auto manager = ved::i_ved_manager::create<ved::ved_manager>();
inline auto cur_error{ std::wstring(L"None error!") };


