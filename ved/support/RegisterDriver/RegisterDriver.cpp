#include "classes/exceptions/exception.h"
#include "classes/ved_manager/ved_manager.h"
#include "interface/i_ved_manager.h"
#include <iostream>

int
main()
{
  try {
    auto ved_manager = ved::i_ved_manager::create<ved::ved_manager>();
    ved_manager->run_driver(SERVICE_AUTO_START);
    std::cout
      << "The driver was launched successfully!\nPress any key to continue.\n";
    std::cin.get();
  } catch (const std::exception& error) {
    std::cerr << error.what() << std::endl;
  } catch (const ved::c_win_api_exception& error) {
    std::wcerr << error.GetMessageW() << std::endl;
  }
}
