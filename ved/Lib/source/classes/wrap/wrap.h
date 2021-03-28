#pragma once
#include <Windows.h>
#include "../general_headers/open_file_info.h"
#include "../general_headers/constant.h"
#include "../../../global_headers/pch.h"
#include "../../classes/exceptions/Exception.h"

namespace ved {

	class handle
	{

	public:

		handle(const handle&) = delete;
		handle(handle&&) = delete;
		handle& operator = (const handle&) = delete;
		handle& operator = (handle&&) = delete;

		explicit handle(HANDLE = {}) noexcept;

		virtual ~handle(void) noexcept;

		operator HANDLE (void) const noexcept;

		bool operator ! (void) const noexcept;

	private:

		HANDLE h_{};

	};

	namespace service
	{

		class handle
		{
						
		public:
			handle(void) = delete;
			handle(const handle&) = delete;
			handle& operator = (const handle&) = delete;
			handle(handle&&) = delete;
			handle& operator = (handle&&) = delete;
			~handle(void) noexcept;

			static std::unique_ptr<handle> create(SC_HANDLE);

			operator SC_HANDLE(void) const noexcept;

			bool operator ! (void) const noexcept;

		private:
			SC_HANDLE h_{};

			explicit handle(SC_HANDLE) noexcept;

		};

	}

}
