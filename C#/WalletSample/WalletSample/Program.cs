using System;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Phantasma.Blockchain;
using Phantasma.Blockchain.Tokens;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.RpcClient;
using Phantasma.RpcClient.Client;
using Phantasma.RpcClient.DTOs;

namespace WalletSample
{
    static class Program
    {
        private static PhantasmaRpcService _phantasmaApiService;

        private static AccountDto _account;

        private static KeyPair _key;

        static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            var bufferSize = 1024 * 67 + 128;
            Stream inputStream = Console.OpenStandardInput(bufferSize);
            Console.SetIn(new StreamReader(inputStream, Console.InputEncoding, false, bufferSize));
            StartWallet().Wait();
        }

        private static async Task StartWallet()
        {
            Console.WriteLine("Welcome to Phantasma Wallet sample.");
            Console.WriteLine("Enter your WIF:");

            _phantasmaApiService = new PhantasmaRpcService(new RpcClient(new Uri("http://localhost:7077/rpc")));

            bool loggedIn = false;
            bool logout = false;
            while (!loggedIn)
            {
                try
                {
                    string wif = Console.ReadLine();
                    _key = KeyPair.Generate();;//KeyPair.FromWIF(wif);
                    loggedIn = true;
                }
                catch (Exception)
                {
                    Console.WriteLine("Incorrect wif, enter again:");
                }
            }
            //
            while (!logout)
            {
                Console.WriteLine();
                Console.WriteLine();
                Console.WriteLine("MENU");
                Console.WriteLine("1 - show address");
                Console.WriteLine("2 - show balance and account name");
                Console.WriteLine("3 - send tokens");
                Console.WriteLine("4 - logout");
                string option = Console.ReadLine();
                Console.WriteLine();
                switch (option)
                {
                    case "1":
                        Console.WriteLine(_key.Address);
                        break;
                    case "2":
                        await ShowBalance();
                        break;
                    case "3":
                        await SendTokens();
                        break;
                    case "4":
                        logout = true;
                        break;
                }
                Console.WriteLine();
            }
        }

        private static async Task ShowBalance()
        {
            _account = await _phantasmaApiService.GetAccount.SendRequestAsync(_key.Address.ToString());
            string name = _account.Name;
            Console.WriteLine();
            Console.WriteLine($"Address Name: {name}");
            foreach (var balanceSheetDto in _account.Tokens)
            {
                Console.WriteLine("********************");
                Console.WriteLine($"Token: {balanceSheetDto.Symbol}");
                Console.WriteLine($"Chain: {balanceSheetDto.ChainName}");
                Console.WriteLine($"Amount: {TokenUtils.ToDecimal(BigInteger.Parse(balanceSheetDto.Amount), 8)}");
                Console.WriteLine();
            }
        }

        private static async Task SendTokens()
        {
            string addressTo = null;
            if (_account == null)
            {
                _account = await _phantasmaApiService.GetAccount.SendRequestAsync(_key.Address.ToString());
            }

            if (!HaveSoulToTransfer())
            {
                Console.WriteLine("No SOUL to tranfer");
                return;
            }

            Console.WriteLine($"Enter destination address: ");
            addressTo = Console.ReadLine();
            if (!Address.IsValidAddress(addressTo))
            {
                Console.WriteLine("Incorrect address");
            }
            else
            {
                var destinationAddress = Address.FromText(addressTo);
                // var token = SelectToken();
                string token = "SOUL";
                Console.WriteLine("Enter amount: ");

                var amount = Console.ReadLine();
                var amountDecimal = decimal.Parse(amount);
                if (!HaveSoulToTransfer(amountDecimal))
                {
                    Console.WriteLine("Insuficient funds");
                    return;
                }

                var bigIntAmount = TokenUtils.ToBigInteger(amountDecimal, 8);

                var script = ScriptUtils.BeginScript()
                    .AllowGas(_key.Address, 1, 9999)
                    .TransferTokens(token, _key.Address, destinationAddress, bigIntAmount)
                    .SpendGas(_key.Address)
                    .EndScript();

                var tx = new Transaction("simnet", "main", script,
                    DateTime.UtcNow + TimeSpan.FromHours(1), 0);
                tx.Sign(_key);

                var txResult = await _phantasmaApiService.SendRawTx.SendRequestAsync(tx.ToByteArray(true).Encode());

                if (!txResult.HasError)
                {
                    Console.WriteLine($"Transaction sent. Tx hash: {txResult.Hash}");
                }
                else
                {
                    Console.WriteLine($"Something happened. Error: {txResult.Error}");
                }
            }
        }

        private static bool HaveSoulToTransfer(decimal amount = 0)
        {
            var test = _account.Tokens.Where(p => decimal.Parse(p.Amount) > amount && p.Symbol == "SOUL");
            return test.Any();
        }

        private static string SelectToken()
        {
            // Console.WriteLine();
            //Console.WriteLine("Select token: ");
            //int option = 1;
            //foreach (var accountToken in _account.Tokens)
            //{
            //    Console.WriteLine($"{option} - {accountToken.Symbol}");
            //    option++;
            //}

            //var selectedTokenOption = int.Parse(Console.ReadLine());
            //var token = _account.Tokens[selectedTokenOption];
            return "";
        }

        private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            using (FileStream fs = new FileStream("error.log", FileMode.Create, FileAccess.Write, FileShare.None))
            using (StreamWriter w = new StreamWriter(fs))
                if (e.ExceptionObject is Exception ex)
                {
                    PrintErrorLogs(w, ex);
                }
                else
                {
                    w.WriteLine(e.ExceptionObject.GetType());
                    w.WriteLine(e.ExceptionObject);
                }
        }

        private static void PrintErrorLogs(StreamWriter writer, Exception ex)
        {
            writer.WriteLine(ex.GetType());
            writer.WriteLine(ex.Message);
            writer.WriteLine(ex.StackTrace);
            if (ex is AggregateException ex2)
            {
                foreach (Exception inner in ex2.InnerExceptions)
                {
                    writer.WriteLine();
                    PrintErrorLogs(writer, inner);
                }
            }
            else if (ex.InnerException != null)
            {
                writer.WriteLine();
                PrintErrorLogs(writer, ex.InnerException);
            }
        }
    }
}
