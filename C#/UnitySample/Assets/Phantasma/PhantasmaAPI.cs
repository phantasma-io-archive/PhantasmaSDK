using System;
using System.Net;
using System.Collections;
using LunarLabs.Parser;
using LunarLabs.Parser.JSON;
using UnityEngine;
using UnityEngine.Networking;
using Phantasma.Cryptography;

namespace Phantasma.SDK
{
	public static class APIUtils
    {
        public static long GetInt64(this DataNode node, string name)
        {
            return node.GetLong(name);
        }

        public static bool GetBoolean(this DataNode node, string name)
        {
            return node.GetBool(name);
        }
    }

    internal class JSONRPC_Client
    {
        private WebClient client;

        internal JSONRPC_Client()
        {
            client = new WebClient() { Encoding = System.Text.Encoding.UTF8 }; 
        }

        internal IEnumerator SendRequest(string url, string method, Action<DataNode> callback, params object[] parameters)
        {
            string contents;

            DataNode paramData;

            if (parameters!=null && parameters.Length > 0)
            {
                paramData = DataNode.CreateArray("params");
                foreach (var obj in parameters)
                {
                    paramData.AddField(null, obj);
                }
            }
            else
            {
                paramData = null;
            }

            var jsonRpcData = DataNode.CreateObject(null);
            jsonRpcData.AddField("jsonrpc", "2.0");
            jsonRpcData.AddField("method", method);
            jsonRpcData.AddField("id", "1");

            if (paramData != null)
            {
                jsonRpcData.AddNode(paramData);
            }

            UnityWebRequest www;
            string json;

            try
            {
                //client.Headers.Add("Content-Type", "application/json-rpc");
				json = JSONWriter.WriteToString(jsonRpcData);
				//contents = client.UploadString(url, json);
            }
            catch (Exception e)
            {
                throw e;
            }
            
            www = UnityWebRequest.Post(url, json);
            yield return www.SendWebRequest();
            
            if (www.isNetworkError || www.isHttpError)
            {
                Debug.Log(www.error);
				throw new Exception(www.error);
            }
            else
            {
                Debug.Log(www.downloadHandler.text);
				var root = JSONReader.ReadFromString(www.downloadHandler.text);
				
				if (root == null)
				{
					throw new Exception("failed to parse JSON");
				}
				else 
				if (root.HasNode("error")) {
					var errorDesc = root.GetString("error");
					throw new Exception(errorDesc);
				}
				else
				if (root.HasNode("result"))
				{
					var result = root["result"];
					callback(result);
				}
				else {					
					throw new Exception("malformed response");
				}				
            }

			yield break;
        }		
   }
   
   
	public struct AccountResult 
	{
		public string Address;
		public string Name;
		public BalanceSheetResult[] Balances;
	   
		public static AccountResult FromNode(DataNode node) 
		{
			AccountResult result;
						
			result.Address = node.GetString("address");						
			result.Name = node.GetString("name");			
			var Balances_array = node.GetNode("balances");
			if (Balances_array != null) {
				result.Balances = new BalanceSheetResult[Balances_array.ChildCount];
				for (int i=0; i < Balances_array.ChildCount; i++) {
					
					result.Balances[i] = BalanceSheetResult.FromNode(Balances_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Balances = new BalanceSheetResult[0];
			}
			

			return result;			
		}
	}
	
	public struct BalanceSheetResult 
	{
		public string Chain;
		public string Amount;
		public string Symbol;
		public String[] Ids;
	   
		public static BalanceSheetResult FromNode(DataNode node) 
		{
			BalanceSheetResult result;
						
			result.Chain = node.GetString("chain");						
			result.Amount = node.GetString("amount");						
			result.Symbol = node.GetString("symbol");			
			var Ids_array = node.GetNode("ids");
			if (Ids_array != null) {
				result.Ids = new String[Ids_array.ChildCount];
				for (int i=0; i < Ids_array.ChildCount; i++) {
					
					result.Ids[i] = Ids_array.GetNodeByIndex(i).AsString();
				}
			}
			else {
				result.Ids = new String[0];
			}
			

			return result;			
		}
	}
	
	public struct ChainResult 
	{
		public string Name;
		public string Address;
		public string ParentAddress;
		public uint Height;
		public ChainResult[] Children;
	   
		public static ChainResult FromNode(DataNode node) 
		{
			ChainResult result;
						
			result.Name = node.GetString("name");						
			result.Address = node.GetString("address");						
			result.ParentAddress = node.GetString("parentAddress");						
			result.Height = node.GetUInt32("height");			
			var Children_array = node.GetNode("children");
			if (Children_array != null) {
				result.Children = new ChainResult[Children_array.ChildCount];
				for (int i=0; i < Children_array.ChildCount; i++) {
					
					result.Children[i] = ChainResult.FromNode(Children_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Children = new ChainResult[0];
			}
			

			return result;			
		}
	}
	
	public struct AppResult 
	{
		public string Description;
		public string Icon;
		public string Id;
		public string Title;
		public string Url;
	   
		public static AppResult FromNode(DataNode node) 
		{
			AppResult result;
						
			result.Description = node.GetString("description");						
			result.Icon = node.GetString("icon");						
			result.Id = node.GetString("id");						
			result.Title = node.GetString("title");						
			result.Url = node.GetString("url");

			return result;			
		}
	}
	
	public struct AppListResult 
	{
		public AppResult[] Apps;
	   
		public static AppListResult FromNode(DataNode node) 
		{
			AppListResult result;
			
			var Apps_array = node.GetNode("apps");
			if (Apps_array != null) {
				result.Apps = new AppResult[Apps_array.ChildCount];
				for (int i=0; i < Apps_array.ChildCount; i++) {
					
					result.Apps[i] = AppResult.FromNode(Apps_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Apps = new AppResult[0];
			}
			

			return result;			
		}
	}
	
	public struct AccountTransactionsResult 
	{
		public string Address;
		public long Amount;
		public TransactionResult[] Txs;
	   
		public static AccountTransactionsResult FromNode(DataNode node) 
		{
			AccountTransactionsResult result;
						
			result.Address = node.GetString("address");						
			result.Amount = node.GetInt64("amount");			
			var Txs_array = node.GetNode("txs");
			if (Txs_array != null) {
				result.Txs = new TransactionResult[Txs_array.ChildCount];
				for (int i=0; i < Txs_array.ChildCount; i++) {
					
					result.Txs[i] = TransactionResult.FromNode(Txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Txs = new TransactionResult[0];
			}
			

			return result;			
		}
	}
	
	public struct BlockResult 
	{
		public string Hash;
		public string PreviousHash;
		public long Timestamp;
		public long Height;
		public string ChainAddress;
		public long Nonce;
		public string Payload;
		public TransactionResult[] Txs;
		public string MinerAddress;
		public Decimal Reward;
	   
		public static BlockResult FromNode(DataNode node) 
		{
			BlockResult result;
						
			result.Hash = node.GetString("hash");						
			result.PreviousHash = node.GetString("previousHash");						
			result.Timestamp = node.GetInt64("timestamp");						
			result.Height = node.GetInt64("height");						
			result.ChainAddress = node.GetString("chainAddress");						
			result.Nonce = node.GetInt64("nonce");						
			result.Payload = node.GetString("payload");			
			var Txs_array = node.GetNode("txs");
			if (Txs_array != null) {
				result.Txs = new TransactionResult[Txs_array.ChildCount];
				for (int i=0; i < Txs_array.ChildCount; i++) {
					
					result.Txs[i] = TransactionResult.FromNode(Txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Txs = new TransactionResult[0];
			}
									
			result.MinerAddress = node.GetString("minerAddress");						
			result.Reward = node.GetDecimal("reward");

			return result;			
		}
	}
	
	public struct EventResult 
	{
		public string Address;
		public string Data;
		public string Kind;
	   
		public static EventResult FromNode(DataNode node) 
		{
			EventResult result;
						
			result.Address = node.GetString("address");						
			result.Data = node.GetString("data");						
			result.Kind = node.GetString("kind");

			return result;			
		}
	}
	
	public struct RootChainResult 
	{
		public string Name;
		public string Address;
		public uint Height;
	   
		public static RootChainResult FromNode(DataNode node) 
		{
			RootChainResult result;
						
			result.Name = node.GetString("name");						
			result.Address = node.GetString("address");						
			result.Height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct TokenResult 
	{
		public string Symbol;
		public string Name;
		public int Decimals;
		public Boolean IsFungible;
		public string CurrentSupply;
		public string MaxSupply;
		public string Owner;
	   
		public static TokenResult FromNode(DataNode node) 
		{
			TokenResult result;
						
			result.Symbol = node.GetString("symbol");						
			result.Name = node.GetString("name");						
			result.Decimals = node.GetInt32("decimals");						
			result.IsFungible = node.GetBoolean("isFungible");						
			result.CurrentSupply = node.GetString("currentSupply");						
			result.MaxSupply = node.GetString("maxSupply");						
			result.Owner = node.GetString("owner");

			return result;			
		}
	}
	
	public struct TokenListResult 
	{
		public TokenResult[] Tokens;
	   
		public static TokenListResult FromNode(DataNode node) 
		{
			TokenListResult result;
			
			var Tokens_array = node.GetNode("tokens");
			if (Tokens_array != null) {
				result.Tokens = new TokenResult[Tokens_array.ChildCount];
				for (int i=0; i < Tokens_array.ChildCount; i++) {
					
					result.Tokens[i] = TokenResult.FromNode(Tokens_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Tokens = new TokenResult[0];
			}
			

			return result;			
		}
	}
	
	public struct TxConfirmationResult 
	{
		public string Hash;
		public string Chain;
		public int Confirmations;
		public uint Height;
	   
		public static TxConfirmationResult FromNode(DataNode node) 
		{
			TxConfirmationResult result;
						
			result.Hash = node.GetString("hash");						
			result.Chain = node.GetString("chain");						
			result.Confirmations = node.GetInt32("confirmations");						
			result.Height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct TransactionResult 
	{
		public string Txid;
		public string ChainAddress;
		public string ChainName;
		public uint Timestamp;
		public uint BlockHeight;
		public Decimal GasLimit;
		public Decimal GasPrice;
		public string Script;
		public EventResult[] Events;
	   
		public static TransactionResult FromNode(DataNode node) 
		{
			TransactionResult result;
						
			result.Txid = node.GetString("txid");						
			result.ChainAddress = node.GetString("chainAddress");						
			result.ChainName = node.GetString("chainName");						
			result.Timestamp = node.GetUInt32("timestamp");						
			result.BlockHeight = node.GetUInt32("blockHeight");						
			result.GasLimit = node.GetDecimal("gasLimit");						
			result.GasPrice = node.GetDecimal("gasPrice");						
			result.Script = node.GetString("script");			
			var Events_array = node.GetNode("events");
			if (Events_array != null) {
				result.Events = new EventResult[Events_array.ChildCount];
				for (int i=0; i < Events_array.ChildCount; i++) {
					
					result.Events[i] = EventResult.FromNode(Events_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Events = new EventResult[0];
			}
			

			return result;			
		}
	}
	
   
   public class API {	   
		public readonly	string Host;
		private static JSONRPC_Client _client;
	   
		public API(string host) 
		{
			this.Host = host;
			_client = new JSONRPC_Client();
		}
	   
		
		//Returns the account name and balance of given address.
		public IEnumerator GetAccount(String addressText, Action<AccountResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getAccount", (node) => { 
			var result = AccountResult.FromNode(node);
				callback(result);
			} , addressText);		   
		}
		
		
		//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
		public IEnumerator GetBlockTransactionCountByHash(String blockHash, Action<int> callback)  
		{	   
			yield return _client.SendRequest(Host, "getBlockTransactionCountByHash", (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , blockHash);		   
		}
		
		
		//Returns information about a block by hash.
		public IEnumerator GetBlockByHash(String blockHash, Int32 serialized, Action<BlockResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getBlockByHash", (node) => { 
			var result = BlockResult.FromNode(node);
				callback(result);
			} , blockHash, serialized);		   
		}
		
		
		//Returns information about a block by height and chain.
		public IEnumerator GetBlockByHeight(Address chainAddress, UInt32 height, Int32 serialized, Action<BlockResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getBlockByHeight", (node) => { 
			var result = BlockResult.FromNode(node);
				callback(result);
			} , chainAddress, height, serialized);		   
		}
		
		
		//Returns the information about a transaction requested by a block hash and transaction index.
		public IEnumerator GetTransactionByBlockHashAndIndex(String blockHash, Int32 index, Action<TransactionResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTransactionByBlockHashAndIndex", (node) => { 
			var result = TransactionResult.FromNode(node);
				callback(result);
			} , blockHash, index);		   
		}
		
		
		//Returns last X transactions of given address.
		public IEnumerator GetAddressTransactions(String addressText, Int32 amountTx, Action<AccountTransactionsResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getAddressTransactions", (node) => { 
			var result = AccountTransactionsResult.FromNode(node);
				callback(result);
			} , addressText, amountTx);		   
		}
		
		
		//TODO document me
		public IEnumerator GetAddressTransactionCount(String addressText, String chainText, Action<int> callback)  
		{	   
			yield return _client.SendRequest(Host, "getAddressTransactionCount", (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , addressText, chainText);		   
		}
		
		
		//Returns the number of confirmations of given transaction hash and other useful info.
		public IEnumerator GetConfirmations(String hashText, Action<int> callback)  
		{	   
			yield return _client.SendRequest(Host, "getConfirmations", (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , hashText);		   
		}
		
		
		//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
		public IEnumerator SendRawTransaction(String txData, Action<string> callback)  
		{	   
			yield return _client.SendRequest(Host, "sendRawTransaction", (node) => { 
			var result = node.Value;
				callback(result);
			} , txData);		   
		}
		
		
		//Returns information about a transaction by hash.
		public IEnumerator GetTransaction(String hashText, Action<TransactionResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTransaction", (node) => { 
			var result = TransactionResult.FromNode(node);
				callback(result);
			} , hashText);		   
		}
		
		
		//Returns an array of chains with useful information.
		public IEnumerator GetChains(Action<ChainResult[]> callback)  
		{	   
			yield return _client.SendRequest(Host, "getChains", (node) => { 
			var result = new ChainResult[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = ChainResult.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns an array of tokens deployed in Phantasma.
		public IEnumerator GetTokens(Action<TokenResult[]> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTokens", (node) => { 
			var result = new TokenResult[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = TokenResult.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns an array of apps deployed in Phantasma.
		public IEnumerator GetApps(Action<AppResult[]> callback)  
		{	   
			yield return _client.SendRequest(Host, "getApps", (node) => { 
			var result = new AppResult[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = AppResult.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns information about the root chain.
		public IEnumerator GetRootChain(Action<RootChainResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getRootChain", (node) => { 
			var result = RootChainResult.FromNode(node);
				callback(result);
			} );		   
		}
		
		
		//Returns last X transactions of given token.
		public IEnumerator GetTokenTransfers(String tokenSymbol, Int32 amount, Action<TransactionResult[]> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTokenTransfers", (node) => { 
			var result = new TransactionResult[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = TransactionResult.FromNode(child);
			}
				callback(result);
			} , tokenSymbol, amount);		   
		}
		
		
		//Returns the number of transaction of a given token.
		public IEnumerator GetTokenTransferCount(String tokenSymbol, Action<int> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTokenTransferCount", (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , tokenSymbol);		   
		}
		
		
		//Returns the balance for a specific token and chain, given an address.
		public IEnumerator GetTokenBalance(String addressText, String tokenSymbol, String chainInput, Action<BalanceSheetResult> callback)  
		{	   
			yield return _client.SendRequest(Host, "getTokenBalance", (node) => { 
			var result = BalanceSheetResult.FromNode(node);
				callback(result);
			} , addressText, tokenSymbol, chainInput);		   
		}
		
		
	}
}