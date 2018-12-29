using Phantasma.Core.Utils;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;

namespace SDK.Builder
{
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

        static void CopyFolder(string sourceDir, string targetDir)
        {
            sourceDir = FixPath(sourceDir);
            targetDir = FixPath(targetDir);

            string[] files = Directory.GetFiles(sourceDir);
            CopyFiles(files, targetDir);
        }

        static void CopyFiles(IEnumerable<string> files, string targetDir)
        { 
            if (!Directory.Exists(targetDir))
            {
                Directory.CreateDirectory(targetDir);
            }

            foreach (var fileName in files)
            {
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

            CopyFolder(inputPath + @"PhantasmaSpook\Spook.CLI\Publish", outputPath + @"Tools\Spook");

            File.WriteAllText(outputPath + "launch_testnet_node.bat", "dotnet %~dp0Tools/Spook/Spook.dll -node.wif=L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25 -rpc.enabled=true");

            Log("Success!");
        }
    }
}
