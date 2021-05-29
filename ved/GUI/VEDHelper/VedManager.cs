using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using GUI.VEDHelper.VedUnsafeNativeMethods;

namespace GUI.VEDHelper
{
    internal class VedManager
    {
        public enum CryptMode
        {
            Aes = 1,
            Rc4
        }

        public enum StartType
        {
            AutoStart = 2,
            Start = 3
        }

        public VedManager()
        {
            CheckAndRunDriver();
        }

        private static string GetLastError()
        {
            var nativeError = NativeVedApi.get_error();
            var strError = Marshal.PtrToStringAuto(nativeError);
            return strError;
        }

        private static void ThrowException(string context)
        {
            var strError = GetLastError();
            if (strError == null)
                throw new Exception("Error get error string from dll!");
            throw new Exception("Error " + context + "! - " + strError);
        }

        private static void StartDriver(string pathDriver, string nameService, VedManager.StartType startType)
        {
            // if (!NativeVedApi.run_driver((int)StartType.Start, pathDriver, nameService))
            //    ThrowException("start driver");
        }

        private static void CheckAndRunDriver()
        {
            if (!File.Exists(DllHelper.Path))
                throw new Exception(DllHelper.Path + " not found!");

            StartDriver(DllHelper.PathToDriver, DllHelper.NameService, StartType.Start);
        }

        public string GetMountedDisksJson()
        {
            const string possibleError = "small input";
            const string curError = "get mounted disks from driver";

            var size = 1;
            var buffer = new StringBuilder(size);


            if (NativeVedApi.get_mounted_disks(buffer, ref size))
                return buffer.ToString();

            if (GetLastError().Contains(possibleError))
            {
                buffer = new StringBuilder(size);
                if (!NativeVedApi.get_mounted_disks(buffer, ref size))
                    ThrowException(curError);
            }
            else
                ThrowException(curError);

            return buffer.ToString();
        }

        public void Mount(string path, string password, char letter)
        {
            if (!NativeVedApi.mount(path, password, letter))
                ThrowException("mount disk");
        }

        public void UnMount(char letter)
        {
            if (!NativeVedApi.un_mount(letter))
                ThrowException("un mount disk");
        }

        public void MountEx(string path, ulong fileSize, string password, char letter, VedManager.CryptMode mode)
        {
            if (!NativeVedApi.mount_ex(path, fileSize, password, letter, (int)mode))
                ThrowException("mount disk");
        }

        public void CreateDiskFile(string path, ulong size, string password, CryptMode mode)
        {
            if (!NativeVedApi.create_file(path, size, password, (int)mode))
                ThrowException("create disk file");
        }
    }
}
