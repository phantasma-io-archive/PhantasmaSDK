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

public class PhantasmaAPI2 {
    public static class Chain
    {
        public String name;
        public String address;
        public String parentAddress;
        public long height;
    }

    public static class Event
    {
        public String address;
        public String kind;
        public String data;
    }

    public static class Transaction
    {
        public String hash;
        public String chainAddress;
        public long timestamp;
        public long blockHeight;
        public String blockHash;
        public String script;
        public List<Event> events;
        public String result;
        public long confirmations;
    }

    public static class AccountTransactions
    {
        public String address;
        public List<Transaction> txs;
    }

    public static class Paginated
    {
        public long page;
        public long pageSize;
        public long total;
        public long totalPages;
        public Object result;
    }

    public static class Block
    {
        public String hash;
        public String previousHash;
        public long timestamp;
        public long height;
        public String chainAddress;
        public String payload;
        public List<Transaction> txs;
        public String validatorAddress;
        public String reward;
        public Object stuff;
    }

    public static class TokenMetadata
    {
        public String key;
        public String value;
    }

    public static class Token
    {
        public String symbol;
        public String name;
        public long decimals;
        public String currentSupply;
        public String maxSupply;
        public String ownerAddress;
        public List<TokenMetadata> metadataList;
        public String flags;
    }

    final JsonRpcClient client;
    final ObjectMapper mapper;

    public PhantasmaAPI2(final String host)
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

    public Block GetBlockByHeight(String chain, int height)
    {
        return client.createRequest()
                .method("getBlockByHeight")
                .params(chain, height)
                .returnAs(Block.class)
                .execute();
    }

    public List<Token> GetTokens()
    {
        return client.createRequest()
                .method("getTokens")
                .returnAsList(PhantasmaAPI2.Token.class)
                .execute();
    }

    public List<Transaction> GetTokenTransfers(String tokenSymbol, long page, long pageSize)
    {
        Paginated pages = client.createRequest()
                .method("getTokenTransfers")
                .params(tokenSymbol, page, pageSize)
                .returnAs(Paginated.class)
                .execute();

        CollectionType pageType = mapper.getTypeFactory().constructCollectionType(List.class, Transaction.class);
        return mapper.convertValue(pages.result, pageType);
    }
}
