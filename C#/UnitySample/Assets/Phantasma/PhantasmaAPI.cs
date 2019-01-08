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
    public enum EPHANTASMA_SDK_ERROR_TYPE
    {
        API_ERROR,
        WEB_REQUEST_ERROR,
        FAILED_PARSING_JSON,
        MALFORMED_RESPONSE
    }

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

        internal IEnumerator SendRequest(string url, string method, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback, 
                                            Action<DataNode> callback, params object[] parameters)
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

                errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.WEB_REQUEST_ERROR, www.error);

				//throw new Exception(www.error);
            }
            else
            {
                Debug.Log(www.downloadHandler.text);
                var root = JSONReader.ReadFromString(www.downloadHandler.text);

                if (root == null)
                {
                    // TODO FIXE BUG -  qd é invalid address está a entrar aqui quando devia entrar no else if de baixo
                    //throw new Exception("failed to parse JSON");
                    errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.FAILED_PARSING_JSON, "failed to parse JSON");
                }
                else if (root.HasNode("error"))
                {
                    var errorDesc = root.GetString("error");
                    //throw new Exception(errorDesc);
                    errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.API_ERROR, errorDesc);
                }
                else if (root.HasNode("result"))
                {
                    var result = root["result"];
                    callback(result);
                }
                else
                {
                    //throw new Exception("malformed response");
                    errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.MALFORMED_RESPONSE, "malformed response");
                }
            }

            yield break;
        }		
   }
   
   
	public struct Account 
	{
		
public string Address;

		
public string Name;

		
public Balance[] Balances;

	   
		public static Account FromNode(DataNode node) 
		{
			Account result;
						
			result.Address = node.GetString("address");						
			result.Name = node.GetString("name");			
			var Balances_array = node.GetNode("balances");
			if (Balances_array != null) {
				result.Balances = new Balance[Balances_array.ChildCount];
				for (int i=0; i < Balances_array.ChildCount; i++) {
					
					result.Balances[i] = Balance.FromNode(Balances_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Balances = new Balance[0];
			}
			

			return result;			
		}
	}
	
	public struct Balance 
	{
		
public string Chain;

		
public string Amount;

		
public string Symbol;

		
public String[] Ids;

	   
		public static Balance FromNode(DataNode node) 
		{
			Balance result;
						
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
	
	public struct Chain 
	{
		
public string Name;

		
public string Address;

		
public string ParentAddress;

		
public uint Height;

		
public Chain[] Children;

	   
		public static Chain FromNode(DataNode node) 
		{
			Chain result;
						
			result.Name = node.GetString("name");						
			result.Address = node.GetString("address");						
			result.ParentAddress = node.GetString("parentAddress");						
			result.Height = node.GetUInt32("height");			
			var Children_array = node.GetNode("children");
			if (Children_array != null) {
				result.Children = new Chain[Children_array.ChildCount];
				for (int i=0; i < Children_array.ChildCount; i++) {
					
					result.Children[i] = Chain.FromNode(Children_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Children = new Chain[0];
			}
			

			return result;			
		}
	}
	
	public struct App 
	{
		
public string Description;

		
public string Icon;

		
public string Id;

		
public string Title;

		
public string Url;

	   
		public static App FromNode(DataNode node) 
		{
			App result;
						
			result.Description = node.GetString("description");						
			result.Icon = node.GetString("icon");						
			result.Id = node.GetString("id");						
			result.Title = node.GetString("title");						
			result.Url = node.GetString("url");

			return result;			
		}
	}
	
	public struct AppList 
	{
		
public App[] Apps;

	   
		public static AppList FromNode(DataNode node) 
		{
			AppList result;
			
			var Apps_array = node.GetNode("apps");
			if (Apps_array != null) {
				result.Apps = new App[Apps_array.ChildCount];
				for (int i=0; i < Apps_array.ChildCount; i++) {
					
					result.Apps[i] = App.FromNode(Apps_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Apps = new App[0];
			}
			

			return result;			
		}
	}
	
	public struct AccountTransactions 
	{
		
public string Address;

		
public long Amount;

		
public Transaction[] Txs;

	   
		public static AccountTransactions FromNode(DataNode node) 
		{
			AccountTransactions result;
						
			result.Address = node.GetString("address");						
			result.Amount = node.GetInt64("amount");			
			var Txs_array = node.GetNode("txs");
			if (Txs_array != null) {
				result.Txs = new Transaction[Txs_array.ChildCount];
				for (int i=0; i < Txs_array.ChildCount; i++) {
					
					result.Txs[i] = Transaction.FromNode(Txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Txs = new Transaction[0];
			}
			

			return result;			
		}
	}
	
	public struct Block 
	{
		
public string Hash;

		
public string PreviousHash;

		
public long Timestamp;

		
public long Height;

		
public string ChainAddress;

		
public long Nonce;

		
public string Payload;

		
public Transaction[] Txs;

		
public string MinerAddress;

		
public Decimal Reward;

	   
		public static Block FromNode(DataNode node) 
		{
			Block result;
						
			result.Hash = node.GetString("hash");						
			result.PreviousHash = node.GetString("previousHash");						
			result.Timestamp = node.GetInt64("timestamp");						
			result.Height = node.GetInt64("height");						
			result.ChainAddress = node.GetString("chainAddress");						
			result.Nonce = node.GetInt64("nonce");						
			result.Payload = node.GetString("payload");			
			var Txs_array = node.GetNode("txs");
			if (Txs_array != null) {
				result.Txs = new Transaction[Txs_array.ChildCount];
				for (int i=0; i < Txs_array.ChildCount; i++) {
					
					result.Txs[i] = Transaction.FromNode(Txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Txs = new Transaction[0];
			}
									
			result.MinerAddress = node.GetString("minerAddress");						
			result.Reward = node.GetDecimal("reward");

			return result;			
		}
	}
	
	public struct Event 
	{
		
public string Address;

		
public string Data;

		
public string Kind;

	   
		public static Event FromNode(DataNode node) 
		{
			Event result;
						
			result.Address = node.GetString("address");						
			result.Data = node.GetString("data");						
			result.Kind = node.GetString("kind");

			return result;			
		}
	}
	
	public struct RootChain 
	{
		
public string Name;

		
public string Address;

		
public uint Height;

	   
		public static RootChain FromNode(DataNode node) 
		{
			RootChain result;
						
			result.Name = node.GetString("name");						
			result.Address = node.GetString("address");						
			result.Height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct Token 
	{
		
public string Symbol;

		
public string Name;

		
public int Decimals;

		
public Boolean IsFungible;

		
public string CurrentSupply;

		
public string MaxSupply;

		
public string Owner;

	   
		public static Token FromNode(DataNode node) 
		{
			Token result;
						
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
	
	public struct TokenList 
	{
		
public Token[] Tokens;

	   
		public static TokenList FromNode(DataNode node) 
		{
			TokenList result;
			
			var Tokens_array = node.GetNode("tokens");
			if (Tokens_array != null) {
				result.Tokens = new Token[Tokens_array.ChildCount];
				for (int i=0; i < Tokens_array.ChildCount; i++) {
					
					result.Tokens[i] = Token.FromNode(Tokens_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Tokens = new Token[0];
			}
			

			return result;			
		}
	}
	
	public struct TxConfirmation 
	{
		
public string Hash;

		
public string Chain;

		
public int Confirmations;

		
public uint Height;

	   
		public static TxConfirmation FromNode(DataNode node) 
		{
			TxConfirmation result;
						
			result.Hash = node.GetString("hash");						
			result.Chain = node.GetString("chain");						
			result.Confirmations = node.GetInt32("confirmations");						
			result.Height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct Transaction 
	{
		
public string Txid;

		
public string ChainAddress;

		
public string ChainName;

		
public uint Timestamp;

		
public uint BlockHeight;

		
public Decimal GasLimit;

		
public Decimal GasPrice;

		
public string Script;

		
public Event[] Events;

	   
		public static Transaction FromNode(DataNode node) 
		{
			Transaction result;
						
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
				result.Events = new Event[Events_array.ChildCount];
				for (int i=0; i < Events_array.ChildCount; i++) {
					
					result.Events[i] = Event.FromNode(Events_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.Events = new Event[0];
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
		public IEnumerator GetAccount(string addressText, Action<Account> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)  
		{	   
			yield return _client.SendRequest(Host, "getAccount", errorHandlingCallback,  (node) => { 
			var result = Account.FromNode(node);
				callback(result);
			} , addressText);		   
		}
		
		
		//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
		public IEnumerator GetBlockTransactionCountByHash(string blockHash, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getBlockTransactionCountByHash", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , blockHash);		   
		}
		
		
		//Returns information about a block by hash.
		public IEnumerator GetBlockByHash(string blockHash, int serialized, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getBlockByHash", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , blockHash, serialized);		   
		}
		
		
		//Returns information about a block by height and chain.
		public IEnumerator GetBlockByHeight(Address chainAddress, uint height, int serialized, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getBlockByHeight", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , chainAddress, height, serialized);		   
		}
		
		
		//Returns the information about a transaction requested by a block hash and transaction index.
		public IEnumerator GetTransactionByBlockHashAndIndex(string blockHash, int index, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTransactionByBlockHashAndIndex", errorHandlingCallback, (node) => { 
			var result = Transaction.FromNode(node);
				callback(result);
			} , blockHash, index);		   
		}
		
		
		//Returns last X transactions of given address.
		public IEnumerator GetAddressTransactions(string addressText, int amountTx, Action<AccountTransactions> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getAddressTransactions", errorHandlingCallback, (node) => { 
			var result = AccountTransactions.FromNode(node);
				callback(result);
			} , addressText, amountTx);		   
		}
		
		
		//TODO document me
		public IEnumerator GetAddressTransactionCount(string addressText, string chainText, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getAddressTransactionCount", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , addressText, chainText);		   
		}
		
		
		//Returns the number of confirmations of given transaction hash and other useful info.
		public IEnumerator GetConfirmations(string hashText, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getConfirmations", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , hashText);		   
		}
		
		
		//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
		public IEnumerator SendRawTransaction(string txData, Action<string> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "sendRawTransaction", errorHandlingCallback, (node) => { 
			var result = node.Value;
				callback(result);
			} , txData);		   
		}
		
		
		//Returns information about a transaction by hash.
		public IEnumerator GetTransaction(string hashText, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTransaction", errorHandlingCallback, (node) => { 
			var result = Transaction.FromNode(node);
				callback(result);
			} , hashText);		   
		}
		
		
		//Returns an array of chains with useful information.
		public IEnumerator GetChains(Action<Chain[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getChains", errorHandlingCallback, (node) => { 
			var result = new Chain[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = Chain.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns an array of tokens deployed in Phantasma.
		public IEnumerator GetTokens(Action<Token[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTokens", errorHandlingCallback, (node) => { 
			var result = new Token[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = Token.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns an array of apps deployed in Phantasma.
		public IEnumerator GetApps(Action<App[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getApps", errorHandlingCallback, (node) => { 
			var result = new App[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = App.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		//Returns information about the root chain.
		public IEnumerator GetRootChain(Action<RootChain> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getRootChain", errorHandlingCallback, (node) => { 
			var result = RootChain.FromNode(node);
				callback(result);
			} );		   
		}
		
		
		//Returns last X transactions of given token.
		public IEnumerator GetTokenTransfers(string tokenSymbol, int amount, Action<Transaction[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTokenTransfers", errorHandlingCallback, (node) => { 
			var result = new Transaction[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = Transaction.FromNode(child);
			}
				callback(result);
			} , tokenSymbol, amount);		   
		}
		
		
		//Returns the number of transaction of a given token.
		public IEnumerator GetTokenTransferCount(string tokenSymbol, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTokenTransferCount", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , tokenSymbol);		   
		}
		
		
		//Returns the balance for a specific token and chain, given an address.
		public IEnumerator GetTokenBalance(string addressText, string tokenSymbol, string chainInput, Action<Balance> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback)
        {	   
			yield return _client.SendRequest(Host, "getTokenBalance", errorHandlingCallback, (node) => { 
			var result = Balance.FromNode(node);
				callback(result);
			} , addressText, tokenSymbol, chainInput);		   
		}
		
		
	}
}