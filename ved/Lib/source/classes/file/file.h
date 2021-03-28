#pragma once
#include "../../../global_headers/pch.h"
#include "../../interface/IFile.h"

namespace ved
{

	class file : public i_file
	{
		
	public:
		file(void) = default;
		file(const file&) = delete;
		file(file&&) = delete;
		file& operator = (const file&) = delete;
		file& operator = (file&&) = delete;

		struct settings
		{
			
			std::wstring ws_file_name{};
			DWORD dw_desired_access{ GENERIC_READ };
			DWORD dw_share_mode{ FILE_SHARE_READ };
			LPSECURITY_ATTRIBUTES lp_security_attributes{};
			DWORD dw_creation_disposition{ OPEN_EXISTING };
			DWORD dw_flags_and_attributes{ FILE_ATTRIBUTE_NORMAL };
			HANDLE h_template_file{};


		};
			
		explicit file(settings settings) : settings_(std::move(settings)) { }
		
		virtual ~file(void) = default;
		
		static auto create(const settings&settings) {
				return i_file::create<file, file::settings>(settings);
		}

		[[nodiscard]] LARGE_INTEGER get_size(void) const override;

		static DWORD get_attributes(const std::wstring& ws_file);

		static std::wstring get_extension(const std::wstring& ws_file);

	private:
			
		settings settings_{};
		
		i_file& create(void) override;


		i_file& connect(LPOVERLAPPED = {}) const override {

			throw file_exception(L"File::Connect",
				ERROR_NOT_SUPPORTED);
			
		}
		
		[[nodiscard]] i_file& disconnect(void) const override {

			throw file_exception(L"File::Disconnect",
				ERROR_NOT_SUPPORTED);
			
		}

	};


}
