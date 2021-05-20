#include "functions.h"

#include "i_ved_manager.h"
#include "ved_manager/ved_manager.h"

static auto manager{ ved::i_ved_manager::create<ved::ved_manager>() };
static std::wstring cur_error{};

template<typename... Types>
bool
check_arguments(Types... args) noexcept
{
  const auto check_null_pointer = [](auto param) {
    if constexpr (std::is_pointer_v<std::decay_t<decltype(param)>>) {
      return static_cast<bool>(param);
    } else {
      return true;
    }
  };
  auto res{ true };
  return ((res &= check_null_pointer(args)), ...);
}

void
set_error(const wchar_t* error)
{
  cur_error = error;
}

template<typename T>
bool
noexcept_call(T&& func) noexcept
{
  try {
    func();
    return true;
  } catch (const ved::c_exception& error) {
    cur_error = error.GetMessageW();
  } catch (...) {
  }
  return false;
}

template<typename T, typename... Args>
auto
create_method_wrapper(T&& func, Args&&... args)
{
  return [&func, &args...]() { (manager.get()->*func)(args...); };
}

template<typename T, typename... Args>
auto
safe_call(T&& func, Args&&... args) noexcept
{
  if (check_arguments(args...)) {
    set_error(L"Invalid input argument(null pointer)");
    return false;
  }
  return noexcept_call(
    create_method_wrapper(std::forward<T>(func), std::forward<Args>(args)...));
}

#define IMLP_TEMPLATE_FUNC_VED_API(name)                                       \
  template<typename... Args>                                                   \
  bool name(Args... param) noexcept                                            \
  {                                                                            \
    return safe_call(&ved::i_ved_manager::name, param...);                     \
  }

// bool
// run_driver(unsigned long flag_startup,
//           const wchar_t* path_driver,
//           const wchar_t* name_service)
//{
//  try {
//    if (!path_driver || !name_service) {
//      cur_error = L"Invalid input arguments(NULL pointer)!";
//    }
//    manager->run_driver(flag_startup, path_driver, name_service);
//    return true;
//  } catch (const ved::c_exception& error) {
//    cur_error = error.GetMessageW();
//  } catch (...) {
//  }
//  return false;
//}

IMLP_TEMPLATE_FUNC_VED_API(un_mount)
IMLP_TEMPLATE_FUNC_VED_API(mount)
IMLP_TEMPLATE_FUNC_VED_API(mount_ex)
IMLP_TEMPLATE_FUNC_VED_API(create_file)
IMLP_TEMPLATE_FUNC_VED_API(run_driver)

template<typename... Args>
const wchar_t*
get_error(Args... param) noexcept
{
  return cur_error.c_str();
}
