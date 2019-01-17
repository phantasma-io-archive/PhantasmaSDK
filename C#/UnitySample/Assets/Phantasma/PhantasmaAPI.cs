using System;
using System.Collections;
using System.Globalization;

using UnityEngine;
using UnityEngine.Networking;

using LunarLabs.Parser;
using LunarLabs.Parser.JSON;

using Phantasma.Numerics;

namespace Phantasma.SDK
{
    public enum EPHANTASMA_SDK_ERROR_TYPE
    {
        API_ERROR,
        WEB_REQUEST_ERROR,
        FAILED_PARSING_JSON,
        MALFORMED_RESPONSE
    }

	internal static class APIUtils
    {
        internal static long GetInt64(this DataNode node, string name)
        {
            return node.GetLong(name);
        }

        internal static bool GetBoolean(this DataNode node, string name)
        {
            return node.GetBool(name);
        }
    }

    internal class JSONRPC_Client
    {
        internal IEnumerator SendRequest(string url, string method, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback, 
                                            Action<DataNode> callback, params object[] parameters)
        {
            string contents;

            var paramData = DataNode.CreateArray("params");
            
            if (parameters!=null && parameters.Length > 0)
            {
                foreach (var obj in parameters)
                {
                    paramData.AddField(null, obj);
                }
            }

            var jsonRpcData = DataNode.CreateObject(null);
            jsonRpcData.AddField("jsonrpc", "2.0");
            jsonRpcData.AddField("method", method);
            jsonRpcData.AddField("id", "1");
            jsonRpcData.AddNode(paramData);
            
            UnityWebRequest www;
            string json;

            try
            {
				json = JSONWriter.WriteToString(jsonRpcData);
            }
            catch (Exception e)
            {
                throw e;
            }
            
            //Debug.Log("SEND www request json: " + json);

            www = UnityWebRequest.Post(url, json);
            yield return www.SendWebRequest();
            
            if (www.isNetworkError || www.isHttpError)
            {
                Debug.Log("Error: " + www.error);
                if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.WEB_REQUEST_ERROR, www.error);			
            }
            else
            {
                //Debug.Log("RECEIVED json: " + www.downloadHandler.text);

                var root = JSONReader.ReadFromString(www.downloadHandler.text);
				
				if (root == null)
				{
					if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.FAILED_PARSING_JSON, "failed to parse JSON");
				}
				else 
				if (root.HasNode("error")) {
					var errorDesc = root.GetString("error");
					if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.API_ERROR, errorDesc);
				}
				else
				if (root.HasNode("result"))
				{
					var result = root["result"];
					callback(result);
				}
				else {					
					if (errorHandlingCallback != null) errorHandlingCallback(EPHANTASMA_SDK_ERROR_TYPE.MALFORMED_RESPONSE, "malformed response");
				}				
            }

			yield break;
        }		
   }
   
   
	public struct Balance 
	{
		public string chain;
		public string amount;
		public string symbol;
		public string[] ids;
	   
		public static Balance FromNode(DataNode node) 
		{
			Balance result;
						
			result.chain = node.GetString("chain");						
			result.amount = node.GetString("amount");						
			result.symbol = node.GetString("symbol");			
			var ids_array = node.GetNode("ids");
			if (ids_array != null) {
				result.ids = new string[ids_array.ChildCount];
				for (int i=0; i < ids_array.ChildCount; i++) {
											
					result.ids[i] = ids_array.GetNodeByIndex(i).AsString();
				}
			}
			else {
				result.ids = new string[0];
			}
			

			return result;			
		}
	}
	
	public struct Account 
	{
		public string address;
		public string name;
		public Balance[] balances;
	   
		public static Account FromNode(DataNode node) 
		{
			Account result;
						
			result.address = node.GetString("address");						
			result.name = node.GetString("name");			
			var balances_array = node.GetNode("balances");
			if (balances_array != null) {
				result.balances = new Balance[balances_array.ChildCount];
				for (int i=0; i < balances_array.ChildCount; i++) {
					
					result.balances[i] = Balance.FromNode(balances_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.balances = new Balance[0];
			}
			

			return result;			
		}
	}
	
	public struct Chain 
	{
		public string name;
		public string address;
		public string parentAddress;
		public uint height;
	   
		public static Chain FromNode(DataNode node) 
		{
			Chain result;
						
			result.name = node.GetString("name");						
			result.address = node.GetString("address");						
			result.parentAddress = node.GetString("parentAddress");						
			result.height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct App 
	{
		public string id;
		public string title;
		public string url;
		public string description;
		public string icon;
	   
		public static App FromNode(DataNode node) 
		{
			App result;
						
			result.id = node.GetString("id");						
			result.title = node.GetString("title");						
			result.url = node.GetString("url");						
			result.description = node.GetString("description");						
			result.icon = node.GetString("icon");

			return result;			
		}
	}
	
	public struct Event 
	{
		public string address;
		public string kind;
		public string data;
	   
		public static Event FromNode(DataNode node) 
		{
			Event result;
						
			result.address = node.GetString("address");						
			result.kind = node.GetString("kind");						
			result.data = node.GetString("data");

			return result;			
		}
	}
	
	public struct Transaction 
	{
		public string hash;
		public string chainAddress;
		public uint timestamp;
		public uint blockHeight;
		public string script;
		public Event[] events;
		public string result;
	   
		public static Transaction FromNode(DataNode node) 
		{
			Transaction result;
						
			result.hash = node.GetString("hash");						
			result.chainAddress = node.GetString("chainAddress");						
			result.timestamp = node.GetUInt32("timestamp");						
			result.blockHeight = node.GetUInt32("blockHeight");						
			result.script = node.GetString("script");
            var events_array = node.GetNode("events");
			if (events_array != null) {
				result.events = new Event[events_array.ChildCount];
				for (int i=0; i < events_array.ChildCount; i++) {
					
					result.events[i] = Event.FromNode(events_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.events = new Event[0];
			}
									
			result.result = node.GetString("result");

			return result;			
		}
	}
	
	public struct AccountTransactions 
	{
		public string address;
		public uint amount;
		public Transaction[] txs;
	   
		public static AccountTransactions FromNode(DataNode node) 
		{
			AccountTransactions result;
						
			result.address = node.GetString("address");						
			result.amount = node.GetUInt32("amount");			
			var txs_array = node.GetNode("txs");
			if (txs_array != null) {
				result.txs = new Transaction[txs_array.ChildCount];
				for (int i=0; i < txs_array.ChildCount; i++) {
					
					result.txs[i] = Transaction.FromNode(txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.txs = new Transaction[0];
			}
			

			return result;			
		}
	}
	
	public struct Block 
	{
		public string hash;
		public string previousHash;
		public uint timestamp;
		public uint height;
		public string chainAddress;
		public string payload;
		public Transaction[] txs;
		public string validatorAddress;
		public string reward;
	   
		public static Block FromNode(DataNode node) 
		{
			Block result;
						
			result.hash = node.GetString("hash");						
			result.previousHash = node.GetString("previousHash");						
			result.timestamp = node.GetUInt32("timestamp");						
			result.height = node.GetUInt32("height");						
			result.chainAddress = node.GetString("chainAddress");						
			result.payload = node.GetString("payload");			
			var txs_array = node.GetNode("txs");
			if (txs_array != null) {
				result.txs = new Transaction[txs_array.ChildCount];
				for (int i=0; i < txs_array.ChildCount; i++) {
					
					result.txs[i] = Transaction.FromNode(txs_array.GetNodeByIndex(i));
					
				}
			}
			else {
				result.txs = new Transaction[0];
			}
									
			result.validatorAddress = node.GetString("validatorAddress");						
			result.reward = node.GetString("reward");

			return result;			
		}
	}
	
	public struct RootChain 
	{
		public string name;
		public string address;
		public uint height;
	   
		public static RootChain FromNode(DataNode node) 
		{
			RootChain result;
						
			result.name = node.GetString("name");						
			result.address = node.GetString("address");						
			result.height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct Token 
	{
		public string symbol;
		public string name;
		public int decimals;
		public string currentSupply;
		public string maxSupply;
		public string ownerAddress;
		public string Flags;
	   
		public static Token FromNode(DataNode node) 
		{
			Token result;
						
			result.symbol = node.GetString("symbol");						
			result.name = node.GetString("name");						
			result.decimals = node.GetInt32("decimals");						
			result.currentSupply = node.GetString("currentSupply");						
			result.maxSupply = node.GetString("maxSupply");						
			result.ownerAddress = node.GetString("ownerAddress");						
			result.Flags = node.GetString("flags");

			return result;			
		}
	}
	
	public struct TokenData 
	{
		public string ID;
		public string chainAddress;
		public string ownerAddress;
		public string ram;
		public string rom;
	   
		public static TokenData FromNode(DataNode node) 
		{
			TokenData result;
						
			result.ID = node.GetString("iD");						
			result.chainAddress = node.GetString("chainAddress");						
			result.ownerAddress = node.GetString("ownerAddress");						
			result.ram = node.GetString("ram");						
			result.rom = node.GetString("rom");

			return result;			
		}
	}
	
	public struct TxConfirmation 
	{
		public string hash;
		public string chainAddress;
		public int confirmations;
		public uint height;
	   
		public static TxConfirmation FromNode(DataNode node) 
		{
			TxConfirmation result;
						
			result.hash = node.GetString("hash");						
			result.chainAddress = node.GetString("chainAddress");						
			result.confirmations = node.GetInt32("confirmations");						
			result.height = node.GetUInt32("height");

			return result;			
		}
	}
	
	public struct SendRawTx 
	{
		public string hash;
		public string error;
	   
		public static SendRawTx FromNode(DataNode node) 
		{
			SendRawTx result;
						
			result.hash = node.GetString("hash");						
			result.error = node.GetString("error");

			return result;			
		}
	}
	
	public struct Auction 
	{
		public string creatorAddress;
		public uint startDate;
		public uint endDate;
		public string Symbol;
		public string TokenID;
		public string Price;
	   
		public static Auction FromNode(DataNode node) 
		{
			Auction result;
						
			result.creatorAddress = node.GetString("creatorAddress");						
			result.startDate = node.GetUInt32("startDate");						
			result.endDate = node.GetUInt32("endDate");						
			result.Symbol = node.GetString("symbol");						
			result.TokenID = node.GetString("tokenID");						
			result.Price = node.GetString("price");

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
		public IEnumerator GetAccount(string addressText, Action<Account> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getAccount", errorHandlingCallback, (node) => { 
			var result = Account.FromNode(node);
				callback(result);
			} , addressText);		   
		}
		
		
		//Returns the height of a chain.
		public IEnumerator GetBlockHeight(string chainInput, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getBlockHeight", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , chainInput);		   
		}
		
		
		//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
		public IEnumerator GetBlockTransactionCountByHash(string blockHash, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getBlockTransactionCountByHash", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , blockHash);		   
		}
		
		
		//Returns information about a block by hash.
		public IEnumerator GetBlockByHash(string blockHash, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getBlockByHash", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , blockHash);		   
		}
		
		
		//Returns information about a block (encoded) by hash.
		public IEnumerator GetRawBlockByHash(string blockHash, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getRawBlockByHash", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , blockHash);		   
		}
		
		
		//Returns information about a block by height and chain.
		public IEnumerator GetBlockByHeight(string chainInput, uint height, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getBlockByHeight", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , chainInput, height);		   
		}
		
		
		//Returns information about a block by height and chain.
		public IEnumerator GetRawBlockByHeight(string chainInput, uint height, Action<Block> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getRawBlockByHeight", errorHandlingCallback, (node) => { 
			var result = Block.FromNode(node);
				callback(result);
			} , chainInput, height);		   
		}
		
		
		//Returns the information about a transaction requested by a block hash and transaction index.
		public IEnumerator GetTransactionByBlockHashAndIndex(string blockHash, int index, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getTransactionByBlockHashAndIndex", errorHandlingCallback, (node) => { 
			var result = Transaction.FromNode(node);
				callback(result);
			} , blockHash, index);		   
		}
		
		
		//Returns last X transactions of given address.
		public IEnumerator GetAddressTransactions(string addressText, int amountTx, Action<AccountTransactions> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getAddressTransactions", errorHandlingCallback, (node) => { 
			var result = AccountTransactions.FromNode(node);
				callback(result);
			} , addressText, amountTx);		   
		}
		
		
		//Get number of transactions in a specific address and chain
		public IEnumerator GetAddressTransactionCount(string addressText, string chainInput, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getAddressTransactionCount", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , addressText, chainInput);		   
		}
		
		
		//Returns the number of confirmations of given transaction hash and other useful info.
		public IEnumerator GetConfirmations(string hashText, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getConfirmations", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , hashText);		   
		}
		
		
		//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
		public IEnumerator SendRawTransaction(string txData, Action<string> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "sendRawTransaction", errorHandlingCallback, (node) => { 
			var result = node.Value;
				callback(result);
			} , txData);		   
		}
		
		
		//Returns information about a transaction by hash.
		public IEnumerator GetTransaction(string hashText, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getTransaction", errorHandlingCallback, (node) => { 
			var result = Transaction.FromNode(node);
				callback(result);
			} , hashText);		   
		}
		
		
		//Returns an array of all chains deployed in Phantasma.
		public IEnumerator GetChains(Action<Chain[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
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
		public IEnumerator GetTokens(Action<Token[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
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
		
		
		//Returns info about a specific token deployed in Phantasma.
		public IEnumerator GetToken(string symbol, Action<Token> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getToken", errorHandlingCallback, (node) => { 
			var result = Token.FromNode(node);
				callback(result);
			} , symbol);		   
		}
		
		
		//Returns data of a non-fungible token, in hexadecimal format.
		public IEnumerator GetTokenData(string symbol, string IDtext, Action<TokenData> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getTokenData", errorHandlingCallback, (node) => { 
			var result = TokenData.FromNode(node);
				callback(result);
			} , symbol, IDtext);		   
		}
		
		
		//Returns an array of apps deployed in Phantasma.
		public IEnumerator GetApps(Action<App[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
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
		public IEnumerator GetRootChain(Action<RootChain> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getRootChain", errorHandlingCallback, (node) => { 
			var result = RootChain.FromNode(node);
				callback(result);
			} );		   
		}
		
		
		//Returns last X transactions of given token.
		public IEnumerator GetTokenTransfers(string tokenSymbol, int amount, Action<Transaction[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
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
		public IEnumerator GetTokenTransferCount(string tokenSymbol, Action<int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getTokenTransferCount", errorHandlingCallback, (node) => { 
			var result = int.Parse(node.Value);
				callback(result);
			} , tokenSymbol);		   
		}
		
		
		//Returns the balance for a specific token and chain, given an address.
		public IEnumerator GetTokenBalance(string addressText, string tokenSymbol, string chainInput, Action<Balance> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getTokenBalance", errorHandlingCallback, (node) => { 
			var result = Balance.FromNode(node);
				callback(result);
			} , addressText, tokenSymbol, chainInput);		   
		}
		
		
		//Returns the auctions available in the market.
		public IEnumerator GetAuctions(Action<Auction[]> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)  
		{	   
			yield return _client.SendRequest(Host, "getAuctions", errorHandlingCallback, (node) => { 
			var result = new Auction[node.ChildCount];
			for (int i=0; i<result.Length; i++) { 
				var child = node.GetNodeByIndex(i);
				result[i] = Auction.FromNode(child);
			}
				callback(result);
			} );		   
		}
		
		
		
       public IEnumerator SignAndSendTransaction(byte[] script, string chain, Action<string> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)
       {
           Debug.Log("Sending transaction...");

           //var tx = new Blockchain.Transaction("nexus", chain, script,  DateTime.UtcNow + TimeSpan.FromHours(1));
           var tx = new Blockchain.Transaction("simnet", chain, script,  DateTime.UtcNow + TimeSpan.FromHours(1));
            tx.Sign(PhantasmaDemo.Instance.Key);

           yield return SendRawTransaction(Base16.Encode(tx.ToByteArray(true)), callback, errorHandlingCallback);
       }
		
       public bool IsValidPrivateKey(string address)
       {
           return (address.StartsWith("L", false, CultureInfo.InvariantCulture) || 
                   address.StartsWith("K", false, CultureInfo.InvariantCulture)) && address.Length == 52;
       }
	}
}