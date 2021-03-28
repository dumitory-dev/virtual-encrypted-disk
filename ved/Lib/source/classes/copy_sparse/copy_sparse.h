#pragma once
#include "../../../global_headers/pch.h"
#include "../file/file.h"
#include "../exceptions/exception.h"



#include <stdio.h>
#include <windows.h>

#define ALLOWED_ATTRIBUTES (FILE_ATTRIBUTE_ARCHIVE | FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_NOT_CONTENT_INDEXED)

namespace ved
{

	class copy_sparse
	{
	public:

		copy_sparse(const copy_sparse& other) = delete;
		copy_sparse(copy_sparse&& other) noexcept = delete;
		copy_sparse& operator=(const copy_sparse& other) = delete;
		copy_sparse& operator=(copy_sparse&& other) noexcept = delete;
		copy_sparse(void) = delete;
		~copy_sparse(void) = delete;
		static void copy(const std::wstring & in, const std::wstring & out);
	};
	
}



