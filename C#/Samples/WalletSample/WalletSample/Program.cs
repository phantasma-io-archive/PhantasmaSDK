using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Phantasma.Blockchain;
using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Tokens;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.RpcClient;
using Phantasma.RpcClient.Client;
using Phantasma.RpcClient.DTOs;
using Phantasma.VM.Utils;
using WalletSample.Utils;

namespace WalletSample
{
    internal static class Program
    {
        private static PhantasmaRpcService _phantasmaApiService;
        private static AccountDto _account;
        private static KeyPair _key;
        private static List<ChainDto> _chains;
        private static IList<TokenDto> _tokens;

        private static void Main(string[] args)
        {
            AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;
            var bufferSize = 1024 * 67 + 128;
            var inputStream = Console.OpenStandardInput(bufferSize);
            Console.SetIn(new StreamReader(inputStream, Console.InputEncoding, false, bufferSize));
            StartWallet().Wait();
        }

        private static async Task StartWallet()
        {
            Console.WriteLine("Welcome to Phantasma Wallet sample.");
            Console.WriteLine("Initializing...");
            Console.WriteLine("Fetching data...");

            _phantasmaApiService = new PhantasmaRpcService(new RpcClient(new Uri("http://localhost:7077/rpc")));

            _chains = (List<ChainDto>)await _phantasmaApiService.GetChains.SendRequestAsync();
            _tokens = await _phantasmaApiService.GetTokens.SendRequestAsync();

            Console.WriteLine("Enter your WIF:");

            var loggedIn = false;

            while (!loggedIn)
            {
                try
                {
                    var wif = Console.ReadLine();
                    _key = KeyPair.FromWIF(wif); //KeyPair.Generate();
                    loggedIn = true;
                }
                catch (Exception)
                {
                    Console.WriteLine("Incorrect wif, enter again:");
                }
            }
            try
            {
                await RunConsole();
            }
            catch (Exception ex)
            {
                Console.WriteLine($"An error occured: {ex.Message}");
                StartWallet().Wait();
            }
        }

        private static async Task RunConsole()
        {
            var logout = false;
            while (!logout)
            {
                Console.WriteLine();
                Console.WriteLine();
                Console.WriteLine("MENU");
                Console.WriteLine("1 - show address");
                Console.WriteLine("2 - show balance and account name");
                Console.WriteLine("3 - register name");
                Console.WriteLine("4 - send tokens");
                Console.WriteLine("5 - list last 10 transactions");
                Console.WriteLine("6 - logout");
                var option = Console.ReadLine();
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
                        await RegisterName();
                        break;
                    case "4":
                        await CrossChainTransfer();
                        break;
                    case "5":
                        await ListTransactions();
                        break;
                    case "6":
                        logout = true;
                        break;
                }

                Console.WriteLine();
            }
        }

        private static async Task ListTransactions()
        {
            var txs = await _phantasmaApiService.GetAddressTxs.SendRequestAsync(_key.Address.ToString(), 0, 10);
            foreach (var tx in txs.AccountTransactionsDto.Txs)
            {
                Console.WriteLine(Utils.Helper.GetTxDescription(tx, _chains, _tokens));
            }
        }

        private static async Task ShowBalance()
        {
            _account = await _phantasmaApiService.GetAccount.SendRequestAsync(_key.Address.ToString());
            var name = _account.Name;
            Console.WriteLine();
            Console.WriteLine($"Address Name: {name}");
            Console.WriteLine();
            if (_account.Tokens == null || !_account.Tokens.Any())
            {
                Console.WriteLine("No funds");
            }
            else
            {
                if (_tokens == null) _tokens = await _phantasmaApiService.GetTokens.SendRequestAsync();
                foreach (var balanceSheetDto in _account.Tokens)
                {
                    Console.WriteLine("********************");
                    Console.WriteLine($"Token: {balanceSheetDto.Symbol}");
                    Console.WriteLine($"Chain: {balanceSheetDto.ChainName}");
                    Console.WriteLine($"Amount: {UnitConversion.ToDecimal(BigInteger.Parse(balanceSheetDto.Amount), Helper.GetTokenDecimals(balanceSheetDto.Symbol, _tokens))}");
                    Console.WriteLine();
                }
            }
        }

        private static bool HaveTokenBalanceToTransfer(decimal amount = 0)
        {
            var test = _account.Tokens.Where(p => decimal.Parse(p.Amount) > amount);
            return test.Any();
        }

        private static async Task CrossChainTransfer()//todo
        {
            if (!HaveTokenBalanceToTransfer())
            {
                Console.WriteLine("No tokens to tranfer");
                return;
            }

            Console.WriteLine("Select token and chain: ");

            for (int i = 0; i < _account.Tokens.Count; i++)
            {
                Console.WriteLine($"{i + 1} - {_account.Tokens[i].Symbol} in {_account.Tokens[i].ChainName} chain");
            }

            var selectedTokenOption = int.Parse(Console.ReadLine());
            var token = _account.Tokens[selectedTokenOption - 1];

            Console.WriteLine("Select destination chain:");

            for (int i = 0; i < _chains.Count; i++)
            {
                Console.WriteLine($"{i + 1} - {_chains[i].Name}");
            }

            var selectedChainOption = int.Parse(Console.ReadLine());
            var destinationChain = _chains[selectedChainOption - 1];


            Console.WriteLine($"Enter amount: (max {UnitConversion.ToDecimal(BigInteger.Parse(token.Amount), Helper.GetTokenDecimals(token.Symbol, _tokens))}");
            var amount = Console.ReadLine();

            Console.WriteLine("Enter destination address: ");
            var destinationAddress = Console.ReadLine();

            if (!Address.IsValidAddress(destinationAddress))
            {
                Console.WriteLine("Invalid address");
                return;
            }

            int cont = 1;
            if (token.ChainName == destinationChain.Name)
            {
                await SameChainTransfer(destinationAddress, amount, token.Symbol, destinationChain.Name);
            }
            else
            {
                Console.WriteLine("TODO");
                //var listSteps = Helper.GetShortestPath(token.ChainName, destinationChain.Name, _chains);
                //if (listSteps.Count >= 2)
                //{
                //    while (listSteps.Count >= 2)
                //    {
                //        Console.WriteLine($"Sending {cont} transaction of {listSteps.Count}");
                //        var txHash = await CrossChainTransferToken(destinationAddress, listSteps[0].Name, listSteps[1].Name, token.Symbol,
                //            amount);
                //        var confirmationDto = await _phantasmaApiService.GetConfirmations.SendRequestAsync(txHash);
                //        while (!confirmationDto.IsConfirmed) await Task.Delay(100);
                //        Console.WriteLine($"Settling block...");
                //        var settleTx = await SettleBlock(listSteps[0].Address, confirmationDto.Hash, listSteps[1].Address);
                //        listSteps.RemoveAt(0);
                //        cont++;
                //    }
                //}
            }
        }

        private static async Task SameChainTransfer(string addressTo, string amount, string tokenSymbol, string chain)
        {
            var destinationAddress = Address.FromText(addressTo);
            int decimals = Helper.GetTokenDecimals(tokenSymbol, _tokens);
            var bigIntAmount = UnitConversion.ToBigInteger(decimal.Parse(amount), decimals);

            var script = ScriptUtils.BeginScript()
                .AllowGas(_key.Address, Address.Null, 1, 9999)
                .TransferTokens(tokenSymbol, _key.Address, destinationAddress, bigIntAmount)
                .SpendGas(_key.Address)
                .EndScript();

            await SignAndSendTx(script, chain);
        }

        private static async Task<string> SettleBlock(string sourceChainAddress, string blockHash, string destinationChainAddress)
        {
            var sourceChain = Address.FromText(sourceChainAddress);
            var destinationChainName = _chains.SingleOrDefault(c => c.Address == destinationChainAddress).Name;

            var block = Hash.Parse(blockHash);

            var settleTxScript = ScriptUtils.BeginScript()
                .CallContract("token", "SettleBlock", sourceChain, block)
                .AllowGas(_key.Address, Address.Null, 1, 9999)
                .SpendGas(_key.Address)
                .EndScript();
            return await SignAndSendTx(settleTxScript, destinationChainName);

        }

        private static async Task<string> CrossChainTransferToken(string addressTo, string chainName, string destinationChain, string symbol, string amount)
        {
            var toChain = _chains.Find(p => p.Name == destinationChain);
            var destinationAddress = Address.FromText(addressTo);
            int decimals = Helper.GetTokenDecimals(symbol, _tokens);
            var bigIntAmount = UnitConversion.ToBigInteger(decimal.Parse(amount), decimals);
            var fee = UnitConversion.ToBigInteger(0.0001m, 8);

            var script = ScriptUtils.BeginScript()
                    .AllowGas(_key.Address, Address.Null, 1, 9999)
                    .CrossTransferToken(Address.FromText(toChain.Address), symbol, _key.Address,
                        _key.Address, fee)
                    .CrossTransferToken(Address.FromText(toChain.Address), symbol, _key.Address,
                        destinationAddress, bigIntAmount)
                    .SpendGas(_key.Address)
                    .EndScript();

            return await SignAndSendTx(script, chainName);
        }

        private static async Task RegisterName()
        {
            if (!HaveTokenBalanceToTransfer())
            {
                Console.WriteLine("Insuficient funds");
                return;
            }
            Console.WriteLine("Enter name for address: ");
            var name = Console.ReadLine();
            var script = ScriptUtils.BeginScript()
                .AllowGas(_key.Address, Address.Null, 1, 9999)
                .CallContract("account", "Register", _key.Address, name)
                .SpendGas(_key.Address)
                .EndScript();

            await SignAndSendTx(script, "main");
        }

        private static async Task<string> SignAndSendTx(byte[] script, string chain)
        {
            try
            {
                Console.WriteLine("Sending transaction...");
                var tx = new Transaction("simnet", chain, script, DateTime.UtcNow + TimeSpan.FromHours(1));
                tx.Sign(_key);
                var txResult = await _phantasmaApiService.SendRawTx.SendRequestAsync(tx.ToByteArray(true).Encode());

                Console.WriteLine($"Transaction sent. Tx hash: {txResult}");
                return txResult;
            }
            catch (RpcResponseException ex)
            {
                Console.WriteLine($"Something happened. Error: {ex.RpcError}");
                return null;
            }
        }

        #region Console
        private static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            using (var fs = new FileStream("error.log", FileMode.Create, FileAccess.Write, FileShare.None))
            using (var w = new StreamWriter(fs))
            {
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
        }

        private static void PrintErrorLogs(StreamWriter writer, Exception ex)
        {
            writer.WriteLine(ex.GetType());
            writer.WriteLine(ex.Message);
            writer.WriteLine(ex.StackTrace);
            if (ex is AggregateException ex2)
            {
                foreach (var inner in ex2.InnerExceptions)
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
        #endregion
    }
}