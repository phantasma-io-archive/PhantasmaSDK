using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Contracts.Native;
using Phantasma.Blockchain.Tokens;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.RpcClient.DTOs;

namespace WalletSample.Utils
{
    internal static class Helper
    {
        public static int GetTokenDecimals(string tokenSymbol, IList<TokenDto> tokens)
        {
            return tokens.SingleOrDefault(p => p.Symbol.Equals(tokenSymbol)).Decimals;
        }

        public static List<ChainDto> SelectShortestPath(string from, string to, List<string> paths, List<ChainDto> phantasmaChains)
        {
            var finalPath = "";
            foreach (var path in paths)
            {
                if (path.IndexOf(from, StringComparison.Ordinal) != -1 && path.IndexOf(to, StringComparison.Ordinal) != -1)
                {
                    if (finalPath == "")
                    {
                        finalPath = path;
                    }
                    else if (path.Count(d => d == ',') < finalPath.Count(d => d == ','))
                    {
                        finalPath = path;
                    }
                }
            }
            var listStrLineElements = finalPath.Split(',').ToList();
            List<ChainDto> chainPath = new List<ChainDto>();
            foreach (var element in listStrLineElements)
            {
                chainPath.Add(phantasmaChains.Find(p => p.Name == element.Trim()));
            }
            return chainPath;
        }

        public static List<ChainDto> GetShortestPath(string from, string to, List<ChainDto> phantasmaChains)
        {
            var vertices = new List<string>();
            var edges = new List<Tuple<string, string>>();

            var children = new Dictionary<string, List<ChainDto>>();
            foreach (var chain in phantasmaChains)
            {
                var childs = phantasmaChains.Where(p => p.ParentAddress.Equals(chain.Address));
                if (childs.Any())
                {
                    children[chain.Name] = childs.ToList();
                }
            }

            foreach (var chain in phantasmaChains)
            {
                vertices.Add(chain.Name);
                if (children.ContainsKey(chain.Name))
                {
                    foreach (var child in children[chain.Name])
                    {
                        edges.Add(new Tuple<string, string>(chain.Name, child.Name));
                    }
                }
            }

            var graph = new Graph<string>(vertices, edges);

            var shortestPath = Algorithms.ShortestPathFunction(graph, from);

            List<string> allpaths = new List<string>();
            foreach (var vertex in vertices)
            {
                allpaths.Add(string.Join(", ", shortestPath(vertex)));
            }

            foreach (var allpath in allpaths)
            {
                Debug.WriteLine(allpath);
            }

            return SelectShortestPath(from, to, allpaths, phantasmaChains);
        }

        public static string GetTxDescription(TransactionDto tx, List<ChainDto> phantasmaChains, IList<TokenDto> phantasmaTokens)
        {
            string description = null;

            string senderToken = null;
            Address senderChain = Address.FromText(tx.ChainAddress);
            Address senderAddress = Address.Null;

            string receiverToken = null;
            Address receiverChain = Address.Null;
            Address receiverAddress = Address.Null;

            BigInteger amount = 0;

            foreach (var evt in tx.Events) //todo move this
            {
                Event nativeEvent;
                if (evt.Data != null)
                {
                    nativeEvent = new Event((Phantasma.Blockchain.Contracts.EventKind)evt.EventKind,
                        Address.FromText(evt.EventAddress), evt.Data.Decode());
                }
                else
                {
                    nativeEvent =
                        new Event((Phantasma.Blockchain.Contracts.EventKind)evt.EventKind, Address.FromText(evt.EventAddress));
                }

                switch (evt.EventKind)
                {
                    case Phantasma.RpcClient.DTOs.EventKind.TokenSend:
                        {
                            var data = nativeEvent.GetContent<TokenEventData>();
                            amount = data.value;
                            senderAddress = nativeEvent.Address;
                            senderToken = (data.symbol);
                        }
                        break;

                    case Phantasma.RpcClient.DTOs.EventKind.TokenReceive:
                        {
                            var data = nativeEvent.GetContent<TokenEventData>();
                            amount = data.value;
                            receiverAddress = nativeEvent.Address;
                            receiverChain = data.chainAddress;
                            receiverToken = data.symbol;
                        }
                        break;

                    case Phantasma.RpcClient.DTOs.EventKind.TokenEscrow:
                        {
                            var data = nativeEvent.GetContent<TokenEventData>();
                            amount = data.value;
                            var amountDecimal = UnitConversion.ToDecimal(amount,
                                phantasmaTokens.SingleOrDefault(p => p.Symbol == data.symbol).Decimals);
                            receiverAddress = nativeEvent.Address;
                            receiverChain = data.chainAddress;
                            var chain = GetChainName(receiverChain.Text, phantasmaChains);
                            description =
                                $"{amountDecimal} {data.symbol} tokens escrowed for address {receiverAddress} in {chain}";
                        }
                        break;
                    case Phantasma.RpcClient.DTOs.EventKind.AddressRegister:
                        {
                            var name = nativeEvent.GetContent<string>();
                            description = $"{nativeEvent.Address} registered the name '{name}'";
                        }
                        break;

                    case Phantasma.RpcClient.DTOs.EventKind.AddFriend:
                        {
                            var address = nativeEvent.GetContent<Address>();
                            description = $"{nativeEvent.Address} added '{address} to friends.'";
                        }
                        break;

                    case Phantasma.RpcClient.DTOs.EventKind.RemoveFriend:
                        {
                            var address = nativeEvent.GetContent<Address>();
                            description = $"{nativeEvent.Address} removed '{address} from friends.'";
                        }
                        break;
                }
            }

            if (description == null)
            {
                if (amount > 0 && senderAddress != Address.Null && receiverAddress != Address.Null &&
                    senderToken != null && senderToken == receiverToken)
                {
                    var amountDecimal = UnitConversion.ToDecimal(amount,
                        phantasmaTokens.SingleOrDefault(p => p.Symbol == senderToken).Decimals);
                    description =
                        $"{amountDecimal} {senderToken} sent from {senderAddress.Text} to {receiverAddress.Text}";
                }
                else if (amount > 0 && receiverAddress != Address.Null && receiverToken != null)
                {
                    var amountDecimal = UnitConversion.ToDecimal(amount,
                        phantasmaTokens.SingleOrDefault(p => p.Symbol == receiverToken).Decimals);
                    description = $"{amountDecimal} {receiverToken} received on {receiverAddress.Text} ";
                }
                else
                {
                    description = "Custom transaction";
                }

                if (receiverChain != Address.Null && receiverChain != senderChain)
                {
                    description +=
                        $" from {GetChainName(senderChain.Text, phantasmaChains)} chain to {GetChainName(receiverChain.Text, phantasmaChains)} chain";
                }
            }

            return description;
        }

        private static string GetChainName(string address, List<ChainDto> phantasmaChains)
        {
            foreach (var element in phantasmaChains)
            {
                if (element.Address == address) return element.Name;
            }

            return string.Empty;
        }
    }
}
