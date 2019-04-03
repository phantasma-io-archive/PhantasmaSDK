using Phantasma.Cryptography;
using Phantasma.Pay;
using System;
using System.Collections.Generic;
using System.Linq;
using LunarLabs.Parser;
using System.Threading;

namespace PaySample
{
    class Program
    {
        static void Main(string[] args)
        {
            var wif = args[0];
            var keys = KeyPair.FromWIF(wif);
            Console.WriteLine("WIF: " + keys.ToWIF());
            var manager = new WalletManager(keys, WalletKind.Bitcoin, WalletKind.Ethereum, WalletKind.Neo);

            int waitingCount = 0;
            foreach (var wallet in manager.Wallets)
            {
                waitingCount++;
                wallet.SyncBalances((ready) =>
                {
                    waitingCount--;
                    Console.WriteLine(wallet.Kind + "\n" + wallet.Address);

                    if (wallet.Balances.Any())
                    {
                        foreach (var balance in wallet.Balances)
                        {
                            Console.WriteLine("\t" + balance.Amount + " " + balance.Symbol);
                        }
                    }
                    else
                    {
                        Console.WriteLine("\tEmpty");
                    }
                });
            }

            while (waitingCount > 0)
            {
                Thread.Sleep(1000);
            }

            Console.WriteLine("Ready!");
            Console.ReadKey();
        }
    }
}
