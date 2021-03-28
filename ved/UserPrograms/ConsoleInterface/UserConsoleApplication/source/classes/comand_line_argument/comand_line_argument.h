#pragma once
#include "../global_headers/pch.h"
#include "classes/exceptions/exception.h"

namespace ved
{
	class command_line_argument
	{
				
	public:
		command_line_argument(void) = delete;
		command_line_argument(const command_line_argument&) = delete;
		command_line_argument(command_line_argument&&) = delete;
		command_line_argument& operator = (const command_line_argument&) = delete;
		command_line_argument& operator = (command_line_argument&&) = delete;
		~command_line_argument(void) = delete;

		
		static std::wstringstream get_command_lines_stream(void);
		static std::vector<std::wstring> get_command_lines_vector(void);

	};
}
