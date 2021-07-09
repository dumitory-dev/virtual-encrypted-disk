using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Principal;

namespace GUI.Utils
{
    static class Utils
    {

        private static List<char> GetUnavailableLetters() => DriveInfo.GetDrives().Select(drive => drive.Name[0]).ToList();
        
        public static List<char> GetAvailableDiskLetters()
        {
            var alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ".ToCharArray();
            var unLetters =  GetUnavailableLetters();
            return alphabet.Except(unLetters).ToList();
        }

        public static bool IsAdminRights()
        {
            using var identity = WindowsIdentity.GetCurrent();
            var principal = new WindowsPrincipal(identity);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

    }
}
