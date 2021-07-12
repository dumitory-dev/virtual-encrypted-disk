#pragma once
#include "pch.h"

namespace ved {

	/**
	* \brief A class for copying "Sparse files"
	*/
	class copy_sparse {
	public:
		copy_sparse(const copy_sparse& other) = delete;
		copy_sparse(copy_sparse&& other) noexcept = delete;
		copy_sparse& operator=(const copy_sparse& other) = delete;
		copy_sparse& operator=(copy_sparse&& other) noexcept = delete;
		copy_sparse(void) = delete;
		~copy_sparse(void) = delete;
		/**
			All files we create can be "sparse". They have to be copied in a special way.
			See also: https://en.wikipedia.org/wiki/Sparse_file
			\param[in] in Where to copy from
			\param[in] out Where to copy 
		*/
		static void copy(const std::wstring& in, const std::wstring& out);
	};

}
