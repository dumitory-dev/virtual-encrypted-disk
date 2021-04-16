#include "../../../Lib/source/classes/ved_manager/ved_manager.h"
#include "classes/copy_sparse/copy_sparse.h"
#include "source/classes/engine/engine.h"

int main(int argc, wchar_t ** argv)
{
	try
	{
				
		ved::engine::get_instance().execute();
	
		std::wcout<<"\nSuccess!"<<std::endl;
							
	}

	catch (const ved::driver_exception & error)
	{
		std::wcout<<error.GetMessageW()<<std::endl;
	}
	catch (const ved::c_win_api_exception & error)
	{
		std::wcout<<error.GetMessageW()<<std::endl;
	}
	catch(const std::exception & error)
	{
		std::wcout<<error.what()<<std::endl;
	}
	catch (...)
	{
		std::wcout<<"error!"<<std::endl;
	}
  
}


