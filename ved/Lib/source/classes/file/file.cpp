#pragma once
#include "File.h"

namespace ved
{

	i_file& file::create(void)
	{


		i_file::h_ = std::make_unique<ved::handle>(::CreateFile(this->settings_.ws_file_name.c_str(),
			this->settings_.dw_desired_access,
			this->settings_.dw_share_mode,
			this->settings_.lp_security_attributes,
			this->settings_.dw_creation_disposition,
			this->settings_.dw_flags_and_attributes,
			nullptr));

		if (!*i_file::h_)
			throw file_exception(L"Error CreateFile",
				GetLastError());


		return *this;


	}


	LARGE_INTEGER file::get_size(void) const
	{
		
		LARGE_INTEGER liReturn{};
		if (!::GetFileSizeEx(*i_file::h_,
			&liReturn))
			throw file_exception(L"Error GetFileSizeEx",
				GetLastError());


		return liReturn;


	}

	std::wstring file::get_extension(const std::wstring& ws_file)
	{
		const auto pos_extension = ws_file.rfind('.');

		if (pos_extension == std::wstring::npos)
			return std::wstring();


		return ws_file.substr(pos_extension);
	}

	DWORD file::get_attributes(const std::wstring& ws_file)
	{
		const auto dw_return = ::GetFileAttributes(ws_file.c_str());

		if (dw_return == INVALID_FILE_ATTRIBUTES)
			throw file_exception(L"Error GetFileAttributes",
			                     GetLastError());

		return dw_return;
	}
}
