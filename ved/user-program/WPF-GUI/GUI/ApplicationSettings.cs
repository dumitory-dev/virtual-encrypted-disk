
using System.Collections.Generic;
using System.IO;
using GUI.View;
using Newtonsoft.Json;

namespace GUI
{
    internal class ApplicationSettings
    {
        public string FilePath { get; set; } = "Setting.json";

        private static string ToJson(List<VirtualDisk> disks) => JsonConvert.SerializeObject(disks);
        private static List<VirtualDisk> FromJson(string data) => JsonConvert.DeserializeObject<List<VirtualDisk>>(data);
        private void SaveToFile(string data) => File.WriteAllText(this.FilePath, data);
        private string LoadFile() => File.ReadAllText(this.FilePath);

        public void SaveDisks(List<VirtualDisk> disks) => this.SaveToFile(ToJson(disks));

        public List<VirtualDisk> LoadDisks() => !File.Exists(this.FilePath) ? new List<VirtualDisk>() : FromJson(LoadFile());

    }
}
