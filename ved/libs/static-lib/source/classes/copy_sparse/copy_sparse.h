#pragma once
#include "pch.h"

namespace ved {

	class copy_sparse {
	public:
		copy_sparse(const copy_sparse& other) = delete;
		copy_sparse(copy_sparse&& other) noexcept = delete;
		copy_sparse& operator=(const copy_sparse& other) = delete;
		copy_sparse& operator=(copy_sparse&& other) noexcept = delete;
		copy_sparse(void) = delete;
		~copy_sparse(void) = delete;
		static void copy(const std::wstring& in, const std::wstring& out);
	};

}
