#pragma once
#include "classes/exceptions/exception.h"
#include "interface/i_file.h"
#include "pch.h"

namespace ved {
	/**
	 * \brief The class encapsulates methods of addressing the device descriptor...
	 */
	class file final : public i_file
	{

	public:
		file(void) = default;
		file(const file&) = delete;
		file(file&&) = delete;
		file& operator=(const file&) = delete;
		file& operator=(file&&) = delete;

		/**
		 * \brief Kind of a good practice...
		 * The default is to open the file for reading
		 */
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

		explicit file(settings settings)
			: settings_(std::move(settings))
		{}

		~file(void) override = default;

		/**
		 * The method creates an instance of our class
		 * \param[in] settings File opening settings
		 * \return unique_ptr<i_file> 
		 */
		static auto create(const settings& settings)
		{
			return i_file::create<file, file::settings>(settings);
		}

		/**
		 * The method returns the file size in kilobytes
		 *
		 * \return LARGE_INTEGER 
		 */
		[[nodiscard]] LARGE_INTEGER get_size(void) const override;

		/**
		 * The method returns file attributes
		 *
		 * \param[in] ws_file File path
		 * \return DWORD file attributes
		 */
		static DWORD get_attributes(const std::wstring& ws_file);

		/**
		 * The method returns file extension
		 *
		 * \param[in] ws_file File path
		 * \return wstring file extension
		 */
		static std::wstring get_extension(const std::wstring& ws_file);

	private:
		settings settings_{};

		i_file& create(void) override;

		i_file& connect(LPOVERLAPPED = {}) const override
		{
			throw file_exception(L"File::Connect", ERROR_NOT_SUPPORTED);
		}

		[[nodiscard]] i_file& disconnect(void) const override
		{
			throw file_exception(L"File::Disconnect", ERROR_NOT_SUPPORTED);
		}
	};

}
