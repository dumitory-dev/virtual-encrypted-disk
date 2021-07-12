#pragma once
#include "pch.h"

#include <utility>
#include <Windows.h>
#include <winternl.h>

namespace ved {
	/**
	 * \brief Own exception classes. This is needed to generate a Win32 error description.
	 * Error description language is English 
	 */
	class c_exception
	{

	public:
		c_exception(const c_exception&) = default;
		c_exception(c_exception&&) = default;
		c_exception& operator=(const c_exception&) = default;
		c_exception& operator=(c_exception&&) = default;

	protected:
		std::wstring m_ws_message_{};
		using ULONG = unsigned long;
		mutable ULONG m_ul_code_{};

	public:
		explicit c_exception(const std::wstring& ws_message)
			: c_exception(ws_message, {})
		{}

		explicit c_exception(const ULONG ul_code)
			: c_exception({}, ul_code)
		{}

		explicit c_exception(std::wstring ws_message, const ULONG ulCode)
			: m_ws_message_(std::move(ws_message))
			, m_ul_code_(ulCode)
		{}

		virtual ~c_exception(void) = default;

		virtual std::wstring GetMessageW(void) const { return this->m_ws_message_; }

		auto get_code(void) const noexcept { return this->m_ul_code_; }
	};

	class c_win_api_exception : public c_exception
	{

	public:
		using c_exception::c_exception;
		/**
		* \brief The method gets its description from the error code.
		* \return std::wstring description from the error code.
		*/
		std::wstring GetMessageW(void) const override
		{

			if (!c_exception::m_ul_code_)
				return c_exception::m_ws_message_;

			LPVOID lp_msg_buf{};

			if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				{},
				c_exception::m_ul_code_,
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&lp_msg_buf),
				{},
				{}))
				return c_exception::m_ws_message_;

			std::wostringstream wos{};
			wos << c_exception::m_ws_message_ << std::endl;
			wos << L"Error code " << c_exception::m_ul_code_ << std::endl;
			wos << static_cast<LPCWSTR>(lp_msg_buf);

			::LocalFree(lp_msg_buf);

			return wos.str();
		}
	};

	class c_command_line_argument_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class c_hash_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class handle_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class map_file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class i_file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class sparse_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class file_exception final : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	class service_exception : public c_win_api_exception
	{

	public:
		using c_win_api_exception::c_win_api_exception;
	};

	/**
	 *
	 * \brief A class for translating errors from driver to WinApi errors.
	 * In general, most NTSTATUS errors can be translated into Win32 and get their description.
	 * We can say that we are lucky in this respect...
	 */
	class driver_exception : public c_win_api_exception
	{
	public:
		using c_win_api_exception::c_win_api_exception;

		static DWORD convert_nt_status_to_win32_error(NTSTATUS nt_status)
		{
#pragma warning(push)

#pragma warning(disable : 4005)
			return RtlNtStatusToDosError(nt_status);
#pragma warning(pop)
		}

		std::wstring GetMessageW(void) const override
		{

			if (!c_exception::m_ul_code_)
				return c_exception::m_ws_message_;

			LPVOID lp_msg_buf{};

			if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM,
				{},
				c_exception::m_ul_code_,
				MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
				reinterpret_cast<LPWSTR>(&lp_msg_buf),
				{},
				{})) {

				c_exception::m_ul_code_ =
					driver_exception::convert_nt_status_to_win32_error(
						c_exception::m_ul_code_);

				if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
					FORMAT_MESSAGE_FROM_SYSTEM,
					{},
					c_exception::m_ul_code_,
					MAKELANGID(LANG_ENGLISH, SUBLANG_DEFAULT),
					reinterpret_cast<LPWSTR>(&lp_msg_buf),
					{},
					{})) {
					return c_exception::m_ws_message_;
				}
			}

			std::wostringstream wos{};
			wos << c_exception::m_ws_message_ << std::endl;
			wos << L"Error code " << c_exception::m_ul_code_ << std::endl;
			wos << static_cast<LPCWSTR>(lp_msg_buf);

			::LocalFree(lp_msg_buf);

			return wos.str();
		}


	};

}