using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace GUI.View
{
    class VirtualDisk
    {

        [JsonProperty("Letter")]
        public char Letter { get; set; }

        [JsonProperty("FilePath")]
        public string Path { get; set; }

        [JsonProperty("FileSize")]
        public string Size { get; set; }

        public bool IsMounted { get; set; }

    }
}
