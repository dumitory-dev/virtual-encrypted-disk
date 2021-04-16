#pragma once
#include "interface/i_ved_manager.h"
#include "classes/install_manager/install_manager.h"
#include "../comand_line_argument/comand_line_argument.h"
#include "classes/ved_manager/ved_manager.h"

namespace  ved
{

	class engine
	{
		using uptr_ved_manager = std::unique_ptr<ved::i_ved_manager>;
	public:

		enum class Param
		{
			
		};
		engine(const engine& other) = delete;
		engine(engine&& other) noexcept = delete;
		engine& operator=(const engine& other) = delete;
		engine& operator=(engine&& other) noexcept = delete;


		static engine& get_instance(void)
		{
			static engine engine_;
			return engine_;
		}

		void set_manager(uptr_ved_manager & manager)
		{
			this->ved_manager_ = std::move(manager);
		}

		void execute(void);
				
	private:
	    void set_arguments(void);
		
		void mount(void);
		void create_and_mount(void);
		void un_mount(void);
		void create_file(void);
		void start_driver(void);
		void stop_driver(void) const;
		void help(void) const;
		void show_mounted_disks(void) const;
		void copy(void);
		
		
		std::map<std::wstring,std::function<void(engine&)>> functions_ = {
			{L"-mount",&engine::mount},
			{L"-camount",&engine::create_and_mount},
			{L"-unmount",&engine::un_mount},
			{L"-create",&engine::create_file},
			{L"-start",&engine::start_driver},
			{L"-stop",&engine::stop_driver},
			{L"-help",&engine::help},
			{L"-show",&engine::show_mounted_disks},
			{L"-copy",&engine::copy}
			
		};
		
	
		std::wstringstream arguments_stream_{};
		uptr_ved_manager ved_manager_;
						
		engine(void):ved_manager_(ved::i_ved_manager::create<ved::ved_manager>())
		{
		
		}
		~engine(void) = default;
	};

}
