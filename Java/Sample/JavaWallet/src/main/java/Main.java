import java.util.List;

public class Main {
    public static void main(String[] args) {
        PhantasmaAPI client = new PhantasmaAPI("http://localhost:7077");

        List<PhantasmaAPI.Token> tokens = client.GetTokens();
        List<PhantasmaAPI.Transaction> txs = client.GetTokenTransfers("SOUL", 1, 5);
        PhantasmaAPI.Block block = client.GetBlockByHeight("main", 1);
    }
}