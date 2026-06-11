using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Net.Http;
using System.Security.Cryptography;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace UGLauncher
{
    class Program
    {
        // Config
        static string ConfigDir = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "UGLauncher");
        static string ConfigFile = Path.Combine(ConfigDir, "config.json");
        static string ManifestFile = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "files_manifest.json");

        static string GTAPath = "";
        static Manifest Manifest = new Manifest();
        static List<ManifestFile> FilesToUpdate = new List<ManifestFile>();

        static async Task Main(string[] args)
        {
            Console.Title = "UG Launcher v1.0";
            Console.SetWindowSize(80, 35);
            Console.SetBufferSize(80, 35);
            Console.CursorVisible = false;

            ShowHeader();

            // Load config
            var config = LoadConfig();
            GTAPath = config.GTAPath ?? "";

            // Auto detect if not set
            if (string.IsNullOrEmpty(GTAPath) || !File.Exists(Path.Combine(GTAPath, "gta_sa.exe")))
            {
                GTAPath = FindGTADir() ?? "";
            }

            if (string.IsNullOrEmpty(GTAPath))
            {
                WriteLine("  Nije pronadjen GTA San Andreas folder!", ConsoleColor.Red);
                WriteLine("  Pritisni [B] da izaberes folder, ili [Q] za izlaz.", ConsoleColor.Yellow);
                
                while (true)
                {
                    var key = Console.ReadKey(true);
                    if (key.Key == ConsoleKey.B)
                    {
                        GTAPath = BrowseFolder();
                        if (!string.IsNullOrEmpty(GTAPath)) break;
                    }
                    if (key.Key == ConsoleKey.Q) return;
                }
            }

            SaveConfig();
            WriteLine($"  GTA folder: {GTAPath}", ConsoleColor.DarkGray);

            // Load manifest
            Manifest = LoadManifest();
            if (Manifest.Files.Count == 0)
            {
                WriteLine("", ConsoleColor.White);
                WriteLine("  Nema fajlova za skidanje. (files_manifest.json je prazan)", ConsoleColor.Yellow);
                WriteLine("  Pokrecem igru...", ConsoleColor.Green);
                LaunchGame();
                return;
            }

            // Check files
            WriteLine("", ConsoleColor.White);
            WriteStatus("Provjera fajlova...", ConsoleColor.Yellow);

            FilesToUpdate = new List<ManifestFile>();
            int totalFiles = Manifest.Files.Count;

            for (int i = 0; i < totalFiles; i++)
            {
                var file = Manifest.Files[i];
                string localPath = Path.Combine(GTAPath, file.LocalPath);

                WriteStatus($"Provjera {i + 1}/{totalFiles}: {Path.GetFileName(file.LocalPath)}", ConsoleColor.Cyan);

                if (!File.Exists(localPath))
                {
                    FilesToUpdate.Add(file);
                    continue;
                }

                if (!string.IsNullOrEmpty(file.Hash))
                {
                    string hash = ComputeMD5(localPath);
                    if (hash != file.Hash.ToLower())
                    {
                        FilesToUpdate.Add(file);
                    }
                }
            }

            // Show results
            WriteLine("", ConsoleColor.White);
            WriteLine($"  Ukupno fajlova:  {totalFiles}", ConsoleColor.White);
            WriteLine($"  Za update:       {FilesToUpdate.Count}", FilesToUpdate.Count > 0 ? ConsoleColor.Red : ConsoleColor.Green);
            WriteLine($"  Azurno:          {totalFiles - FilesToUpdate.Count}", ConsoleColor.Green);

            if (FilesToUpdate.Count == 0)
            {
                WriteLine("", ConsoleColor.White);
                WriteLine("  Svi fajlovi su azurni!", ConsoleColor.Green);
                WriteLine("", ConsoleColor.White);
                ShowPlayMenu();
                return;
            }

            // Download
            WriteLine("", ConsoleColor.White);
            WriteLine($"  Skidam {FilesToUpdate.Count} fajl{(FilesToUpdate.Count > 1 ? "ova" : "")}...", ConsoleColor.Cyan);
            WriteLine("", ConsoleColor.White);

            bool success = await DownloadFiles();

            if (success)
            {
                WriteLine("", ConsoleColor.White);
                WriteLine("  Svi fajlovi skinuti!", ConsoleColor.Green);
            }

            WriteLine("", ConsoleColor.White);
            ShowPlayMenu();
        }

        static void ShowHeader()
        {
            Console.Clear();
            Console.BackgroundColor = ConsoleColor.Black;

            Action<string, ConsoleColor> center = (text, color) =>
            {
                Console.ForegroundColor = color;
                int pad = (80 - text.Length) / 2;
                if (pad < 0) pad = 0;
                Console.WriteLine(new string(' ', pad) + text);
            };

            center("  ___  ____  _   _    ____ ___  _   _  ____ ", ConsoleColor.Red);
            center(" / _\\/ ___|| | | |  / ___/ _ \\| \\ | |/ ___|", ConsoleColor.Red);
            center(" \\  \\| |   | |_| | | |  | | | |  \\| | |  _ ", ConsoleColor.DarkRed);
            center("  _\\ | |___|  _  | | |__| |_| | |\\  | |_| |", ConsoleColor.DarkRed);
            center("  \\__/\\____|_| |_|  \\____\\___/|_| \\_|\\____|", ConsoleColor.DarkRed);
            center("", ConsoleColor.White);
            center("       S A N   A N D R E A S   L A U N C H E R", ConsoleColor.DarkGray);
            center("", ConsoleColor.White);
            WriteLine("  " + new string('─', 76), ConsoleColor.DarkGray);
        }

        static void ShowPlayMenu()
        {
            WriteLine("  [ENTER] IGRAJ    [Q] Izlaz", ConsoleColor.Yellow);
            WriteLine("", ConsoleColor.White);

            while (true)
            {
                var key = Console.ReadKey(true);
                if (key.Key == ConsoleKey.Enter)
                {
                    LaunchGame();
                    return;
                }
                if (key.Key == ConsoleKey.Q)
                {
                    return;
                }
            }
        }

        static async Task<bool> DownloadFiles()
        {
            int total = FilesToUpdate.Count;
            int completed = 0;
            long totalBytes = 0;

            using (var client = new HttpClient())
            {
                client.Timeout = TimeSpan.FromMinutes(10);

                foreach (var file in FilesToUpdate)
                {
                    try
                    {
                        string localPath = Path.Combine(GTAPath, file.LocalPath);
                        string localDir = Path.GetDirectoryName(localPath) ?? "";
                        if (!Directory.Exists(localDir))
                            Directory.CreateDirectory(localDir);

                        string url = !string.IsNullOrEmpty(file.Url) ? file.Url : Manifest.BaseUrl + Path.GetFileName(file.LocalPath);

                        WriteStatus($"  Skidam {completed + 1}/{total}: {Path.GetFileName(file.LocalPath)}", ConsoleColor.Cyan);

                        var response = await client.GetAsync(url);
                        response.EnsureSuccessStatusCode();

                        var data = await response.Content.ReadAsByteArrayAsync();
                        await File.WriteAllBytesAsync(localPath, data);

                        totalBytes += data.Length;
                        completed++;

                        // Progress bar
                        double percent = (double)completed / total * 100;
                        DrawProgressBar(completed, total, percent);

                        // Verify hash
                        if (!string.IsNullOrEmpty(file.Hash))
                        {
                            string hash = ComputeMD5(localPath);
                            if (hash != file.Hash.ToLower())
                            {
                                File.Delete(localPath);
                                WriteLine($"    [X] Hash ne odgovara: {file.LocalPath}", ConsoleColor.Red);
                                return false;
                            }
                        }

                    }
                    catch (Exception ex)
                    {
                        WriteLine($"    [X] Greska: {ex.Message}", ConsoleColor.Red);
                        return false;
                    }
                }
            }

            return true;
        }

        static void DrawProgressBar(int current, int total, double percent)
        {
            int barWidth = 50;
            int filled = (int)(percent / 100 * barWidth);

            Console.ForegroundColor = ConsoleColor.DarkGray;
            Console.Write("  [");
            Console.ForegroundColor = ConsoleColor.Red;
            Console.Write(new string('█', filled));
            Console.ForegroundColor = ConsoleColor.DarkGray;
            Console.Write(new string('░', barWidth - filled));
            Console.Write("] ");
            Console.ForegroundColor = ConsoleColor.White;
            Console.Write($"{percent:F0}%  ");
            Console.ForegroundColor = ConsoleColor.DarkGray;
            Console.Write($"({current}/{total})");
            Console.WriteLine();
        }

        static void LaunchGame()
        {
            string sampPath = Path.Combine(GTAPath, "samp.exe");
            if (!File.Exists(sampPath))
            {
                WriteLine("  samp.exe nije pronadjen!", ConsoleColor.Red);
                WriteLine("  Pritisni bilo koji taster za izlaz...", ConsoleColor.Yellow);
                Console.ReadKey(true);
                return;
            }

            string ip = !string.IsNullOrEmpty(Manifest.ServerIp) ? Manifest.ServerIp : "";

            try
            {
                var psi = new ProcessStartInfo
                {
                    FileName = sampPath,
                    Arguments = ip,
                    WorkingDirectory = GTAPath,
                    UseShellExecute = true
                };
                Process.Start(psi);
                WriteLine("  Pokrecem igru...", ConsoleColor.Green);
            }
            catch (Exception ex)
            {
                WriteLine($"  Greska pri pokretanju: {ex.Message}", ConsoleColor.Red);
            }
        }

        static string FindGTADir()
        {
            string userProfile = Environment.GetFolderPath(Environment.SpecialFolder.UserProfile);
            string[] possiblePaths = new[]
            {
                @"C:\Program Files (x86)\Rockstar Games\GTA San Andreas",
                @"C:\Program Files\Rockstar Games\GTA San Andreas",
                @"C:\Games\GTA San Andreas",
                @"C:\GTA San Andreas",
                Path.Combine(userProfile, "Documents", "GTA San Andreas"),
                Path.Combine(userProfile, @"Steam\steamapps\common\Grand Theft Auto San Andreas"),
            };

            foreach (var p in possiblePaths)
            {
                if (File.Exists(Path.Combine(p, "gta_sa.exe")))
                    return p;
            }

            return null;
        }

        static string BrowseFolder()
        {
            WriteLine("", ConsoleColor.White);
            WriteLine("  Unesi putanju do GTA San Andreas foldera:", ConsoleColor.Yellow);
            WriteLine("  (npr. C:\\Games\\GTA San Andreas)", ConsoleColor.DarkGray);
            Write("  > ", ConsoleColor.White);

            Console.CursorVisible = true;
            string input = Console.ReadLine()?.Trim('"', ' ') ?? "";
            Console.CursorVisible = false;

            if (!string.IsNullOrEmpty(input) && File.Exists(Path.Combine(input, "gta_sa.exe")))
            {
                return input;
            }
            else if (!string.IsNullOrEmpty(input))
            {
                WriteLine("  U ovom folderu nije pronadjen gta_sa.exe!", ConsoleColor.Red);
            }

            return "";
        }

        // ====== Helpers ======

        static string ComputeMD5(string filePath)
        {
            using (var md5 = MD5.Create())
            using (var stream = File.OpenRead(filePath))
            {
                var hash = md5.ComputeHash(stream);
                return BitConverter.ToString(hash).Replace("-", "").ToLower();
            }
        }

        static Manifest LoadManifest()
        {
            if (File.Exists(ManifestFile))
            {
                try
                {
                    var json = File.ReadAllText(ManifestFile);
                    return JsonSerializer.Deserialize<Manifest>(json, new JsonSerializerOptions { PropertyNameCaseInsensitive = true }) ?? new Manifest();
                }
                catch { }
            }
            return new Manifest();
        }

        static LauncherConfig LoadConfig()
        {
            if (File.Exists(ConfigFile))
            {
                try
                {
                    return JsonSerializer.Deserialize<LauncherConfig>(File.ReadAllText(ConfigFile)) ?? new LauncherConfig();
                }
                catch { }
            }
            return new LauncherConfig();
        }

        static void SaveConfig()
        {
            try
            {
                if (!Directory.Exists(ConfigDir)) Directory.CreateDirectory(ConfigDir);
                File.WriteAllText(ConfigFile, JsonSerializer.Serialize(new LauncherConfig { GTAPath = GTAPath }, new JsonSerializerOptions { WriteIndented = true }));
            }
            catch { }
        }

        static void WriteLine(string text, ConsoleColor color)
        {
            Console.ForegroundColor = color;
            Console.WriteLine(text);
        }

        static void Write(string text, ConsoleColor color)
        {
            Console.ForegroundColor = color;
            Console.Write(text);
        }

        static void WriteStatus(string text, ConsoleColor color)
        {
            Console.ForegroundColor = color;
            Console.Write("\r" + text.PadRight(78));
            Console.WriteLine();
        }
    }

    // ====== Models ======
    public class Manifest
    {
        [JsonPropertyName("baseUrl")]
        public string BaseUrl { get; set; } = "";

        [JsonPropertyName("serverIp")]
        public string ServerIp { get; set; } = "";

        [JsonPropertyName("files")]
        public List<ManifestFile> Files { get; set; } = new List<ManifestFile>();
    }

    public class ManifestFile
    {
        [JsonPropertyName("localPath")]
        public string LocalPath { get; set; } = "";

        [JsonPropertyName("url")]
        public string Url { get; set; } = "";

        [JsonPropertyName("hash")]
        public string Hash { get; set; } = "";
    }

    public class LauncherConfig
    {
        [JsonPropertyName("gtaPath")]
        public string? GTAPath { get; set; }
    }
}
