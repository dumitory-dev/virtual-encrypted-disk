using System;
using System.Runtime.InteropServices;
using System.Text;

namespace GUI.VEDHelper.VedUnsafeNativeMethods
{
    internal static class NativeVedApi
    {
        [DllImport(DllHelper.Path, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool run_driver(ulong flag_startup, string path_driver, string name_service);

        [DllImport(DllHelper.Path, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool mount(string path, [MarshalAs(UnmanagedType.LPStr)] string password, char letter);

        [DllImport(DllHelper.Path, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool un_mount(char letter);

        [DllImport(DllHelper.Path, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern bool mount_ex(string path, ulong size, [MarshalAs(UnmanagedType.LPStr)] string password, char letter, int crypt_mode);

        [DllImport(DllHelper.Path, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool create_file(string path, ulong size, [MarshalAs(UnmanagedType.LPStr)] string password, int crypt_mode);


        [DllImport(DllHelper.Path, CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool get_mounted_disks(StringBuilder buffer, ref int bufferSize);

        [DllImport(DllHelper.Path, CharSet = CharSet.Unicode, CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr get_error();
    }
}
