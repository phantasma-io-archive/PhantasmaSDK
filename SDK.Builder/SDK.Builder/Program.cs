using Phantasma.Core.Utils;
using Phantasma.API;

using System;
using System.Reflection;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Phantasma.Blockchain;
using Phantasma.Cryptography;
using LunarLabs.Templates;
using System.Text;
using System.Linq;
using LunarLabs.Parser;

namespace SDK.Builder
{
    public class ArrayTypeNode : TemplateNode
    {
        private RenderingKey key;

        public ArrayTypeNode(Document document, string key) : base(document)
        {
            this.key = RenderingKey.Parse(key, RenderingType.String);
        }

        public override void Execute(RenderingContext context)
        {
            var temp = context.EvaluateObject(key);

            if (temp != null)
            {
                var result = temp.ToString().Replace("[]", "");
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
            baseDir.Delete(true);
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

            var nexus = new Nexus("test", KeyPair.Generate().Address);
            var api = new NexusAPI(nexus);

            var typeDic = new Dictionary<string, FieldInfo[]>();
            var apiTypes = api.GetType().Assembly.GetTypes().Where(x => !x.IsInterface && x != typeof(SingleResult) && x != typeof(ArrayResult) && x != typeof(ErrorResult) && typeof(IAPIResult).IsAssignableFrom(x)).ToList();
            foreach (var entry in apiTypes)
            {
                typeDic[entry.Name/*.Replace("Result", "")*/] = entry.GetFields();
            }

            var compiler = new Compiler();
            compiler.RegisterCaseTags();
            compiler.RegisterTag("array-type", (doc, x) => new ArrayTypeNode(doc, x));

            var data = new Dictionary<string, object>();

            data["methods"] = api.Methods.Select(x => new MethodEntry() { Info = x, ResultFields = x.ReturnType.GetFields() });
            data["types"] = typeDic;

            foreach (var file in files)
            {
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

            foreach (var lang in new[] { "C#", "JS", "PHP" })
            {
                CopyFolder(inputPath + @"PhantasmaSDK\" + lang+ @"\Samples\", tempPath + lang + @"\Dapps\");
                GenerateBindings(inputPath + @"PhantasmaSDK\" + lang + @"\Bindings\", tempPath + lang + @"\Libs\");
            }

            return;
            CopyFolder(inputPath + @"PhantasmaSpook\Spook.CLI\Publish", tempPath + @"Tools\Spook");
            CopyFolder(inputPath + @"PhantasmaWallet\PhantasmaWallet\Publish", tempPath + @"Tools\Wallet");
            CopyFolder(inputPath + @"PhantasmaExplorer\PhantasmaExplorer\Publish", tempPath + @"Tools\Explorer");

            CopyFolder(inputPath + @"PhantasmaCompiler\Compiler.CLI\Examples", tempPath + @"Contracts\Source", (x) => !x.Contains("_old"));

            File.WriteAllText(tempPath + "launch_testnet_node.bat", "dotnet %~dp0Tools/Spook/Spook.dll -node.wif=L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25 -rpc.enabled=true");

            ZipFile(tempPath, outputPath, versionNumber);

            Log("Cleaning up temporary files...");
            RecursiveDelete(new DirectoryInfo(tempPath));

            Log("Success!");
        }
    }
}
