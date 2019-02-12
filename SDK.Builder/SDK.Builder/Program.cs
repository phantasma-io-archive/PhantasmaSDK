using Phantasma.Core.Utils;
using Phantasma.API;
using Phantasma.Blockchain;
using Phantasma.Cryptography;

using LunarLabs.Templates;
using UnityPacker;

using System;
using System.Reflection;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Linq;

namespace SDK.Builder
{
    public class FixTypeNode : TemplateNode
    {
        private RenderingKey key;
        private Dictionary<string, string> replacements;

        public FixTypeNode(Document document, string key, Dictionary<string, string> replacements) : base(document)
        {
            this.replacements = replacements;
            this.key = RenderingKey.Parse(key, RenderingType.String);
        }

        public override void Execute(RenderingContext context)
        {
            var temp = context.EvaluateObject(key);

            if (temp != null)
            {
                var key = temp.ToString();
                key = key.Replace("Result", "").Replace("[]", "");
                string result = replacements.ContainsKey(key) ? replacements[key] : key;
                context.output.Append(result);
            }
        }
    }

    public class FixArrayNode : TemplateNode
    {
        private RenderingKey key;

        public FixArrayNode(Document document, string key) : base(document)
        {
            this.key = RenderingKey.Parse(key, RenderingType.String);
        }

        public override void Execute(RenderingContext context)
        {
            var temp = context.EvaluateObject(key);

            if (temp != null)
            {
                var result = temp.ToString();
                result = result.Replace("Result", "").Replace("[]", "");
                context.output.Append(result);
            }
        }
    }

    class Program
    {
        static void Log(string text)
        {
            Console.WriteLine(text);
        }

        static string FixPath(string path)
        {
            path = path.Replace(@"/", @"\");
            if (!path.EndsWith(@"\"))
            {
                path += @"\";
            }
            return path;
        }

        static void CopyFolder(string sourceDir, string targetDir, Func<string, bool> filter = null)
        {
            sourceDir = FixPath(sourceDir);

            if (!Directory.Exists(sourceDir))
            {
                return;
            }

            targetDir = FixPath(targetDir);

            var files = Directory.GetFiles(sourceDir);
            CopyFiles(files, targetDir, filter);

            var dirs = Directory.GetDirectories(sourceDir);
            foreach (var dir in dirs)
            {
                CopyFolder(dir, dir.Replace(sourceDir, targetDir), filter);
            }
        }

        static void CopyFiles(IEnumerable<string> files, string targetDir, Func<string, bool> filter = null)
        { 
            if (!Directory.Exists(targetDir))
            {
                Directory.CreateDirectory(targetDir);
            }

            foreach (var fileName in files)
            {
                if (filter != null && !filter(fileName))
                {
                    continue;
                }

                string targetFile = Path.Combine(targetDir, (new FileInfo(fileName)).Name);
                Log($"Copying {targetFile}...");
                File.Copy(fileName, targetFile, true);
            }
        }

        static bool RunCommand(string cmd, string args)
        {
            try
            {
                var proc = new Process();
                proc.StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                proc.StartInfo.CreateNoWindow = true;
                proc.StartInfo.UseShellExecute = false;
                proc.StartInfo.FileName = cmd;
                proc.StartInfo.Arguments = args;
                Log(proc.StartInfo.FileName + " " + proc.StartInfo.Arguments);
                proc.EnableRaisingEvents = true;
                proc.Start();
                proc.WaitForExit();
                int exitCode = proc.ExitCode;
                return exitCode >= 0;
            }
            catch (Exception e)
            {
                Log(e.ToString());
                return false;
            }
        }

        static void RecursiveDelete(DirectoryInfo baseDir)
        {
            if (!baseDir.Exists)
                return;

            foreach (var dir in baseDir.EnumerateDirectories())
            {
                RecursiveDelete(dir);
            }
            try
            {
                baseDir.Delete(true);
            }
            catch
            {
                return;
            }
        }

        static void ZipFile(string inputPath, string outputPath, string version)
        {
            RunCommand("7z.exe", $"a {outputPath}/Phantasma_SDK_v{version}.zip {inputPath}/*");
        }

        public struct MethodEntry
        {
            public APIEntry Info;
            public FieldInfo[] ResultFields;
        }

        static void GenerateBindings(string inputPath, string outputPath)
        {
            if (!Directory.Exists(inputPath))
            {
                return;
            }

            var files = Directory.GetFiles(inputPath);

            if (!Directory.Exists(outputPath))
            {
                Directory.CreateDirectory(outputPath);
            }

            string replacementFile = inputPath + "language.ini";
            var replacements = new Dictionary<string, string>();
            if (File.Exists(replacementFile))
            {
                var lines = File.ReadAllLines(replacementFile);
                foreach (var line in lines)
                {
                    if (line.Contains(","))
                    {
                        var temp = line.Split(new[] { ',' }, 2);
                        replacements[temp[0]] = temp[1];
                    }
                }
            }

            var nexus = new Nexus("test", KeyPair.Generate().Address);
            var api = new NexusAPI(nexus);

            var typeDic = new Dictionary<string, IEnumerable<MetaField>>();
            var apiTypes = api.GetType().Assembly.GetTypes().Where(x => !x.IsInterface && x != typeof(SingleResult) && x != typeof(ArrayResult) && x != typeof(ErrorResult) && typeof(IAPIResult).IsAssignableFrom(x)).ToList();
            foreach (var entry in apiTypes)
            {
                typeDic[entry.Name/*.Replace("Result", "")*/] = GetMetaFields(entry);
            }

            var compiler = new Compiler();
            compiler.ParseNewLines = true;
            compiler.RegisterCaseTags();
            compiler.RegisterTag("fix-type", (doc, x) => new FixTypeNode(doc, x, replacements));
            compiler.RegisterTag("fix-array", (doc, x) => new FixArrayNode(doc, x));

            var data = new Dictionary<string, object>();

            data["methods"] = api.Methods.Select(x => new MethodEntry() { Info = x, ResultFields = x.ReturnType.GetFields() });
            data["types"] = typeDic;

            foreach (var file in files)
            {
                if (file == replacementFile)
                {
                    continue;
                }

                var filePath = file;

                var content = File.ReadAllText(filePath);

                var template = compiler.CompileTemplate(content);
                var queue = new Queue<Document>();

                var context = new RenderingContext();
                context.DataRoot = data;
                context.DataStack = new List<object>();
                context.DataStack.Add(data);
                context.queue = queue;
                context.output = new StringBuilder();
                template.Execute(context);

                filePath = file.Replace(inputPath, outputPath);
                File.WriteAllText(filePath, context.output.ToString());
            }
        }

        public struct MetaField
        {
            public string Name;
            public Type FieldType;
            public string Description;
        }

        private static List<MetaField> GetMetaFields(Type type)
        {
            var fields = type.GetFields();

            var result = new List<MetaField>();
            foreach (var field in fields)
            {
                string desc = "TODO document me";

                var attr = field.GetCustomAttribute<APIDescriptionAttribute>();
                if (attr != null)
                {
                    desc = attr.Description;
                }

                var meta = new MetaField()
                {
                    Name = field.Name,
                    FieldType = field.FieldType,
                    Description = desc
                };

                result.Add(meta);
            }

            return result;
        }

        private static void GenerateUnityPackage(string dllPath, string bindingPath)
        {
            dllPath = FixPath(dllPath);
            bindingPath = FixPath(bindingPath);
            var pluginList = new List<string>() { dllPath + "LunarParser.dll", dllPath + "Phantasma.Core.dll", dllPath + "Phantasma.Cryptography.dll", dllPath + "Phantasma.Numerics.dll" };

            var tempPath = @"Phantasma";
            var pluginPath = tempPath + @"\Plugins";
            Directory.CreateDirectory(pluginPath);
            CopyFiles(pluginList, pluginPath);

            CopyFiles(new[] { bindingPath + "PhantasmaAPI.cs" }, tempPath);

            // Create a package object from the given directory
            var pack = Package.FromDirectory(tempPath, "Phantasma", true, new string[0], new string[0]);
            pack.GeneratePackage();

            CopyFiles(new[] { "Phantasma.unitypackage" }, bindingPath);

//            File.Delete(bindingPath + "PhantasmaAPI.cs");

            RecursiveDelete(new DirectoryInfo(tempPath));
        }

        static void Main(string[] args)
        {
            var arguments = new Arguments(args);

            var inputPath = arguments.GetString("input.path");
            var outputPath = arguments.GetString("output.path", Directory.GetCurrentDirectory() + @"\output");
            var versionNumber = arguments.GetString("output.version");

            Log($"Building SDK {versionNumber}");

            inputPath = FixPath(inputPath);
            outputPath = FixPath(outputPath);

            if (inputPath == outputPath)
            {
                Log("Input path and output path must be different!");
                return;
            }

            var tempPath = outputPath + @"temp\";
            Directory.CreateDirectory(tempPath);

            GenerateBindings(inputPath + @"PhantasmaSDK\Docs\", tempPath + @"\Docs\");

            foreach (var lang in new[] { "C#", "JS", "PHP", "Python", "Go", "C++" })
            {
                CopyFolder(inputPath + @"PhantasmaSDK\" + lang+ @"\Samples\", tempPath + lang + @"\Dapps\");
                GenerateBindings(inputPath + @"PhantasmaSDK\" + lang + @"\Bindings\", tempPath + lang + @"\Libs\");
            }

            GenerateUnityPackage(inputPath + @"PhantasmaSDK\SDK.Builder\SDK.Builder\bin\Debug", tempPath + @"C#\Libs\");

            ///return;
            CopyFolder(inputPath + @"PhantasmaSpook\Spook.CLI\Publish", tempPath + @"Tools\Spook");
            CopyFolder(inputPath + @"PhantasmaWallet\PhantasmaWallet\Publish", tempPath + @"Tools\Wallet");
            CopyFolder(inputPath + @"PhantasmaExplorer\PhantasmaExplorer\Publish", tempPath + @"Tools\Explorer");

            CopyFolder(inputPath + @"PhantasmaCompiler\Compiler.CLI\Examples", tempPath + @"Contracts\Source", (x) => !x.Contains("_old"));
            CopyFolder(inputPath + @"PhantasmaExplorer\PhantasmaExplorer\www", tempPath + @"Tools\Explorer\www", (x) => !x.Contains(".db"));
            CopyFolder(inputPath + @"PhantasmaWallet\PhantasmaWallet\www", tempPath + @"Tools\Wallet\www", (x) => !x.Equals("session"));

            File.WriteAllText(tempPath + "launch_dev_node.bat", "dotnet %~dp0Tools/Spook/Spook.dll -node.wif=L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25 -rpc.enabled=true");
            File.WriteAllText(tempPath + "launch_explorer.bat", "dotnet %~dp0Tools/Explorer/Phantasma.Explorer.dll --path=%~dp0Tools/Explorer/www --port=7072");
            File.WriteAllText(tempPath + "launch_wallet.bat", "dotnet %~dp0Tools/Wallet/Phantasma.Wallet.dll --path=%~dp0Tools/Wallet/www --port=7071");

            ZipFile(tempPath, outputPath, versionNumber);

            Log("Cleaning up temporary files...");
            RecursiveDelete(new DirectoryInfo(tempPath));

            Log("Success!");
        }
    }
}
