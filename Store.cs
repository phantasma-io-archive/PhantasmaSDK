using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace PhantasmaMail
{
    public static class Store
    {
        private static Dictionary<string, string> _files = null;

        private static void LoadStore() {
            if (_files != null)
            {
                return;
            }

            _files = new Dictionary<string, string>();

            try
            {
                string path = "store";
                Directory.CreateDirectory(path);

                var names = Directory.GetFiles(path, "*.mail");
                foreach (var name in names)
                {
                    var content = File.ReadAllText(name);
                    var hash = Path.GetFileNameWithoutExtension(name);
                    _files[hash] = content;
                }
            }
            catch
            {
                return;
            }
        }

        private static void SaveStore()
        {
            if (_files == null)
            {
                return;
            }

            foreach (var file in _files)
            {
                File.WriteAllText("store/"+file.Key + ".mail", file.Value);
            }
        }
                
        public static string GetFile(string hash)
        {
            LoadStore();

            if (_files.ContainsKey(hash))
            {
                return _files[hash];
            }

            return null;
        }

        public static void PutFile(string hash, string content)
        {
            LoadStore();

            _files[hash] = content;

            File.WriteAllText("store/" + hash + ".mail", content);
        }

        public static string CreateFile(string content)
        {
            var hash = content.MD5();
            PutFile(hash, content);
            return hash;
        }

        public static string MD5(this string input)
        {
            byte[] inputBytes = System.Text.Encoding.ASCII.GetBytes(input);
            return inputBytes.MD5();
        }

        public static string MD5(this byte[] inputBytes)
        {
            // step 1, calculate MD5 hash from input
            var md5 = System.Security.Cryptography.MD5.Create();

            byte[] hash = md5.ComputeHash(inputBytes);

            // step 2, convert byte array to hex string
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < hash.Length; i++)
            {
                sb.Append(hash[i].ToString("X2"));
            }

            return sb.ToString();
        }

    }
}
