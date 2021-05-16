#pragma once

#ifdef DLL_EXPORTS
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#define DELC_TEMPLATE_FUNC(name, ret_type)                                     \
  template<typename... T>                                                      \
  DLL_API ret_type name(T... t);

#define DELC_TEMPLATE_FUNC_VED_API(name) DELC_TEMPLATE_FUNC(name, bool)

DELC_TEMPLATE_FUNC_VED_API(un_mount);
DELC_TEMPLATE_FUNC_VED_API(mount);
DELC_TEMPLATE_FUNC_VED_API(mount_ex);
DELC_TEMPLATE_FUNC_VED_API(create_file);
DELC_TEMPLATE_FUNC_VED_API(run_driver);
DELC_TEMPLATE_FUNC(get_error, const wchar_t*);

extern "C"
{

  extern template DLL_API bool un_mount(wchar_t letter);

  extern template DLL_API bool mount(const wchar_t* path,
                                     const char* password,
                                     wchar_t letter);

  extern template DLL_API bool mount_ex(const wchar_t* path,
                                        unsigned long long size,
                                        const char* password,
                                        wchar_t letter,
                                        int crypt_mode);

  extern template DLL_API bool create_file(const wchar_t* path,
                                           unsigned long long size,
                                           const char* password,
                                           int crypt_mode);

  extern template DLL_API bool run_driver(
    unsigned long flag_startup,
    const wchar_t* path_driver,
    const wchar_t* name_service);

   //extern template DLL_API bool run_driver(
   // unsigned long flag_startup = 3, // SERVICE_DEMAND_START
   // const wchar_t* path_driver =
   //   L"%SystemRoot%\\System32\\Drivers\\DriverVED.sys",
   // const wchar_t* name_service = L"VEDriver");

  extern template DLL_API const wchar_t* get_error();
}
