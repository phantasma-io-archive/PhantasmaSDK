import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.databind.type.CollectionType;
import com.github.arteam.simplejsonrpc.client.JsonRpcClient;
import com.github.arteam.simplejsonrpc.client.Transport;
import com.google.common.base.Charsets;
import org.apache.http.client.methods.CloseableHttpResponse;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.entity.StringEntity;
import org.apache.http.impl.client.CloseableHttpClient;
import org.apache.http.impl.client.HttpClients;
import org.apache.http.util.EntityUtils;
import org.jetbrains.annotations.NotNull;
import java.io.IOException;
import java.util.List;

public class PhantasmaAPI {

	final JsonRpcClient client;
	final ObjectMapper mapper;

	public PhantasmaAPI(final String host)
	{
		mapper = new ObjectMapper();
		client = new JsonRpcClient(new Transport() {

			CloseableHttpClient httpClient = HttpClients.createDefault();

			@NotNull
			@Override
			public String pass(@NotNull String request) throws IOException {
				// Used Apache HttpClient 4.3.1 as an example
				HttpPost post = new HttpPost(host+"/rpc");
				post.setEntity(new StringEntity(request, Charsets.UTF_8));
				//post.setHeader(HttpHeaders.ACCEPT, PageAttributes.MediaType.JSON_UTF_8.toString());
				try (CloseableHttpResponse httpResponse = httpClient.execute(post)) {
					return EntityUtils.toString(httpResponse.getEntity(), Charsets.UTF_8);
				}
			}
		});
	}


	public static class Balance
	{
		public String chain;//
		public String amount;//
		public String symbol;//
		public long decimals;//
		public List<String> ids;//
	}

	public static class Account
	{
		public String address;//
		public String name;//
		public List<Balance> balances;//
	}

	public static class Chain
	{
		public String name;//
		public String address;//
		public String parentAddress;//
		public long height;//
	}

	public static class App
	{
		public String id;//
		public String title;//
		public String url;//
		public String description;//
		public String icon;//
	}

	public static class Event
	{
		public String address;//
		public String kind;//
		public String data;//
	}

	public static class Transaction
	{
		public String hash;//
		public String chainAddress;//
		public long timestamp;//
		public long confirmations;//
		public long blockHeight;//
		public String blockHash;//
		public String script;//
		public List<Event> events;//
		public String result;//
	}

	public static class AccountTransactions
	{
		public String address;//
		public List<Transaction> txs;//
	}

	public static class Paginated
	{
		public long page;//
		public long pageSize;//
		public long total;//
		public long totalPages;//
		public Object result;//
	}

	public static class Block
	{
		public String hash;//
		public String previousHash;//
		public long timestamp;//
		public long height;//
		public String chainAddress;//
		public String payload;//
		public List<Transaction> txs;//
		public String validatorAddress;//
		public String reward;//
	}

	public static class TokenMetadata
	{
		public String key;//
		public String value;//
	}

	public static class Token
	{
		public String symbol;//
		public String name;//
		public long decimals;//
		public String currentSupply;//
		public String maxSupply;//
		public String ownerAddress;//
		public List<TokenMetadata> metadataList;//
		public String flags;//
	}

	public static class TokenData
	{
		public String ID;//
		public String chainAddress;//
		public String ownerAddress;//
		public String ram;//
		public String rom;//
		public boolean forSale;//
	}

	public static class SendRawTx
	{
		public String hash;//
		public String error;//
	}

	public static class Auction
	{
		public String creatorAddress;//
		public long startDate;//
		public long endDate;//
		public String baseSymbol;//
		public String quoteSymbol;//
		public String tokenId;//
		public String price;//
	}

	public static class Script
	{
		public List<Event> events;//
		public String result;//
	}


	//Returns the account name and balance of given address.
	public Account GetAccount(String addressText)
	{
		return client.createRequest()
				.returnAs(Account.class)
				.method("getAccount")
				.params(addressText)
				.execute();
	}

	//Returns the address that owns a given name.
	public String LookUpName(String name)
	{
		return client.createRequest()
				.returnAs(String.class)
				.method("lookUpName")
				.params(name)
				.execute();
	}

	//Returns the height of a chain.
	public long GetBlockHeight(String chainInput)
	{
		return client.createRequest()
				.returnAs(long.class)
				.method("getBlockHeight")
				.params(chainInput)
				.execute();
	}

	//Returns the number of transactions of given block hash or error if given hash is invalid or is not found.
	public long GetBlockTransactionCountByHash(String blockHash)
	{
		return client.createRequest()
				.returnAs(long.class)
				.method("getBlockTransactionCountByHash")
				.params(blockHash)
				.execute();
	}

	//Returns information about a block by hash.
	public Block GetBlockByHash(String blockHash)
	{
		return client.createRequest()
				.returnAs(Block.class)
				.method("getBlockByHash")
				.params(blockHash)
				.execute();
	}

	//Returns a serialized string, containing information about a block by hash.
	public String GetRawBlockByHash(String blockHash)
	{
		return client.createRequest()
				.returnAs(String.class)
				.method("getRawBlockByHash")
				.params(blockHash)
				.execute();
	}

	//Returns information about a block by height and chain.
	public Block GetBlockByHeight(String chainInput, long height)
	{
		return client.createRequest()
				.returnAs(Block.class)
				.method("getBlockByHeight")
				.params(chainInput, height)
				.execute();
	}

	//Returns a serialized string, in hex format, containing information about a block by height and chain.
	public String GetRawBlockByHeight(String chainInput, long height)
	{
		return client.createRequest()
				.returnAs(String.class)
				.method("getRawBlockByHeight")
				.params(chainInput, height)
				.execute();
	}

	//Returns the information about a transaction requested by a block hash and transaction index.
	public Transaction GetTransactionByBlockHashAndIndex(String blockHash, long index)
	{
		return client.createRequest()
				.returnAs(Transaction.class)
				.method("getTransactionByBlockHashAndIndex")
				.params(blockHash, index)
				.execute();
	}

	//Paginated api call: Returns last X transactions of given address.
	public AccountTransactions GetAddressTransactions(String addressText, long page, long pageSize)
	{
		Paginated pages = client.createRequest()
				.returnAs(Paginated.class)
				.method("getAddressTransactions")
				.params(addressText, page, pageSize)
				.execute();

		CollectionType pageType = mapper.getTypeFactory().constructCollectionType(List.class, AccountTransactions.class);
		return mapper.convertValue(pages.result, pageType);
	}

	//Get number of transactions in a specific address and chain
	public long GetAddressTransactionCount(String addressText, String chainInput)
	{
		return client.createRequest()
				.returnAs(long.class)
				.method("getAddressTransactionCount")
				.params(addressText, chainInput)
				.execute();
	}

	//Allows to broadcast a signed operation on the network, but it&apos;s required to build it manually.
	public String SendRawTransaction(String txData)
	{
		return client.createRequest()
				.returnAs(String.class)
				.method("sendRawTransaction")
				.params(txData)
				.execute();
	}

	//Allows to invoke script based on network state, without state changes.
	public Script InvokeRawScript(String chainInput, String scriptData)
	{
		return client.createRequest()
				.returnAs(Script.class)
				.method("invokeRawScript")
				.params(chainInput, scriptData)
				.execute();
	}

	//Returns information about a transaction by hash.
	public Transaction GetTransaction(String hashText)
	{
		return client.createRequest()
				.returnAs(Transaction.class)
				.method("getTransaction")
				.params(hashText)
				.execute();
	}

	//Removes a pending transaction from the mempool.
	public String CancelTransaction(String hashText)
	{
		return client.createRequest()
				.returnAs(String.class)
				.method("cancelTransaction")
				.params(hashText)
				.execute();
	}

	//Returns an array of all chains deployed in Phantasma.
	public List<Chain> GetChains()
	{
		return client.createRequest()
				.returnAsList(Chain.class)
				.method("getChains")

				.execute();
	}

	//Returns an array of tokens deployed in Phantasma.
	public List<Token> GetTokens()
	{
		return client.createRequest()
				.returnAsList(Token.class)
				.method("getTokens")

				.execute();
	}

	//Returns info about a specific token deployed in Phantasma.
	public Token GetToken(String symbol)
	{
		return client.createRequest()
				.returnAs(Token.class)
				.method("getToken")
				.params(symbol)
				.execute();
	}

	//Returns data of a non-fungible token, in hexadecimal format.
	public TokenData GetTokenData(String symbol, String IDtext)
	{
		return client.createRequest()
				.returnAs(TokenData.class)
				.method("getTokenData")
				.params(symbol, IDtext)
				.execute();
	}

	//Returns an array of apps deployed in Phantasma.
	public List<App> GetApps()
	{
		return client.createRequest()
				.returnAsList(App.class)
				.method("getApps")

				.execute();
	}

	//Paginated api call: Returns last X transactions of given token.
	public List<Transaction> GetTokenTransfers(String tokenSymbol, long page, long pageSize)
	{
		Paginated pages = client.createRequest()
				.returnAs(Paginated.class)
				.method("getTokenTransfers")
				.params(tokenSymbol, page, pageSize)
				.execute();

		CollectionType pageType = mapper.getTypeFactory().constructCollectionType(List.class, Transaction.class);
		return mapper.convertValue(pages.result, pageType);
	}

	//Returns the number of transaction of a given token.
	public long GetTokenTransferCount(String tokenSymbol)
	{
		return client.createRequest()
				.returnAs(long.class)
				.method("getTokenTransferCount")
				.params(tokenSymbol)
				.execute();
	}

	//Returns the balance for a specific token and chain, given an address.
	public Balance GetTokenBalance(String addressText, String tokenSymbol, String chainInput)
	{
		return client.createRequest()
				.returnAs(Balance.class)
				.method("getTokenBalance")
				.params(addressText, tokenSymbol, chainInput)
				.execute();
	}

	//Returns the number of active auctions.
	public long GetAuctionsCount(String symbol)
	{
		return client.createRequest()
				.returnAs(long.class)
				.method("getAuctionsCount")
				.params(symbol)
				.execute();
	}

	//Paginated api call: Returns the auctions available in the market.
	public List<Auction> GetAuctions(String symbol, long page, long pageSize)
	{
		Paginated pages = client.createRequest()
				.returnAs(Paginated.class)
				.method("getAuctions")
				.params(symbol, page, pageSize)
				.execute();

		CollectionType pageType = mapper.getTypeFactory().constructCollectionType(List.class, Auction.class);
		return mapper.convertValue(pages.result, pageType);
	}

	//Returns the auction for a specific token.
	public Auction GetAuction(String symbol, String IDtext)
	{
		return client.createRequest()
				.returnAs(Auction.class)
				.method("getAuction")
				.params(symbol, IDtext)
				.execute();
	}

}

