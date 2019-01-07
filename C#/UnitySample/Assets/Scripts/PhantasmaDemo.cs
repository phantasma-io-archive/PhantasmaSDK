using System;
using UnityEngine;
using UnityEngine.Networking;

using System.Collections;
using System.Globalization;

using Phantasma.Cryptography;
using Phantasma.SDK;

/*
 * Phantasma Spook
 * https://github.com/phantasma-io/PhantasmaSpook/tree/master/Docs
 */

public class PhantasmaDemo : MonoBehaviour
{
    private const string _SERVER_ADDRESS = "http://localhost:7077/rpc";

    private enum EWALLET_STATE
    {
        INIT,
        SYNC,
        UPDATE,
        READY
    }

    private KeyPair keys;

    private EWALLET_STATE state = EWALLET_STATE.INIT;
    private decimal balance;

    //private PhantasmaRpcService _phantasmaApiService;
    //private AccountDto _account;
    //private KeyPair _key;
    //private List<ChainDto> _chains;
    //private TokenList _tokens;

    private static PhantasmaDemo _instance;
    public static PhantasmaDemo Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(PhantasmaDemo)) as PhantasmaDemo : _instance; return _instance; }
    }

    private void Start ()
    {
        //GetAccount("P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr"); //TEST
    }

    private IEnumerator SyncBalance()
    {
        yield return new WaitForSeconds(2);
        //var balances = api.GetAssetBalancesOf(this.keys);
        //balance = balances.ContainsKey(assetSymbol) ? balances[assetSymbol] : 0;
        state = EWALLET_STATE.UPDATE;
    }

    private void Update () {

        switch (state)
        {
            case EWALLET_STATE.INIT:
                {
                    state = EWALLET_STATE.SYNC;
                    StartCoroutine(SyncBalance());
                    break;
                }

            case EWALLET_STATE.UPDATE:
                {
                    state = EWALLET_STATE.READY;
                    CanvasManager.Instance.SetBalance(balance.ToString(CultureInfo.InvariantCulture));
                    break;
                }
        }		
	}

    public void Login(string address)
    {
        GetAccount(address);
    }

    private void LoggedIn(string address)
    {
        //Debug.Log("logged in");

        //var address = "L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25";
        //var addressBytes = Encoding.ASCII.GetBytes(address);
        //keys = KeyPair.FromWIF(address);

        CanvasManager.Instance.SetAddress(address);

        CanvasManager.Instance.ToggleLogin(false, address);
    }

    public void LogOut()
    {
        //Debug.Log("logged out");

        // TODO something here ?

        CanvasManager.Instance.ToggleLogin(true);
    }

    #region Blockchain calls

    public void GetAccount(string address)
    {
        //P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr

        Debug.Log("get account 0: " + address);
        var api = new API("http://localhost:7077/rpc");
        StartCoroutine(api.GetAccount(address, result =>
        {
            CanvasManager.Instance.SetBalance("Name: " + result.Name);

            foreach (var balanceSheetResult in result.Balances)
            {
                var amount = long.Parse(balanceSheetResult.Amount) / Mathf.Pow(10f, 8);
                CanvasManager.Instance.AddBalanceEntry("Chain: " + balanceSheetResult.Chain + " - " + amount + " " + balanceSheetResult.Symbol);

                Debug.Log("balance: " +balanceSheetResult.Chain + " | " + balanceSheetResult.Amount);
            }

            LoggedIn(address);
        }));
    }

    //public void RegisterName()
    //{
    //    StartCoroutine(RegisterNameCoroutine());
    //}

    //private IEnumerator RegisterNameCoroutine()
    //{
    //    var myData = "{ \"jsonrpc\":\"2.0\",\"method\":\"getAccount\",\"params\":[\"P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr\"],\"id\":1}";
    //    var www = UnityWebRequest.Post(_SERVER_ADDRESS, myData);

    //    yield return www.SendWebRequest();

    //    if (www.isNetworkError || www.isHttpError)
    //    {
    //        Debug.Log(www.error);
    //    }
    //    else
    //    {
    //        Debug.Log(www.downloadHandler.text);
    //    }
    //}

    //public void ListTransactions()
    //{
    //    StartCoroutine(ListTransactionsCoroutine());
    //}

    //private IEnumerator ListTransactionsCoroutine()
    //{
    //    var myData = "{ \"jsonrpc\":\"2.0\",\"method\":\"getAccount\",\"params\":[\"P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr\"],\"id\":1}";
    //    var www = UnityWebRequest.Post(_SERVER_ADDRESS, myData);

    //    yield return www.SendWebRequest();

    //    if (www.isNetworkError || www.isHttpError)
    //    {
    //        Debug.Log(www.error);
    //    }
    //    else
    //    {
    //        Debug.Log(www.downloadHandler.text);
    //    }
    //}

    public void GetMarket()
    {
        // TODO get assets from the blockchain

        var assetsCount = 10;

        for (int i = 0; i < assetsCount; i++)
        {
            // Duplicate asset slot template to hold this asset content
            //var newAsset            
        }

    }

    #endregion
}
