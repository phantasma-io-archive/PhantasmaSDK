using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;

using UnityEngine;

using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Contracts.Native;
using Phantasma.Blockchain.Tokens;
using Phantasma.Cryptography;
using Phantasma.IO;
using Phantasma.Numerics;
using Phantasma.SDK;
using Phantasma.VM.Utils;
using Random = UnityEngine.Random;
using Token = Phantasma.SDK.Token;

/*
 * Phantasma Spook
 * https://github.com/phantasma-io/PhantasmaSpook/tree/master/Docs
 */

public class PhantasmaDemo : MonoBehaviour
{
    public const string TOKEN_SYMBOL    = "CAR";
    public const string TOKEN_NAME      = "Car Demo Token";

    private const string _SERVER_ADDRESS = "http://localhost:7077/rpc";

    public KeyPair Key { get; private set; }

    private enum EWALLET_STATE
    {
        INIT,
        SYNC,
        UPDATE,
        READY
    }

    /*
    public enum EventKind
    {
        ChainCreate,
        TokenCreate,
        TokenSend,
        TokenReceive,
        TokenMint,
        TokenBurn,
        TokenEscrow,
        AddressRegister,
        FriendAdd,
        FriendRemove,
        GasEscrow,
        GasPayment,
    }
     */

    public Market       market;
    public List<Sprite> carImages;

    private EWALLET_STATE   _state = EWALLET_STATE.INIT;
    private decimal         _balance;

    public API                          PhantasmaApi        { get; private set; }
    public Dictionary<string, Token>    PhantasmaTokens     { get; private set; }
    public bool                         IsTokenCreated      { get; private set; }
    public bool                         IsTokenOwner        { get; private set; }
    public decimal                      TokenCurrentSupply  { get; private set; }
    public Dictionary<string, Car>      MyCars              { get; set; }
    
    private static PhantasmaDemo _instance;
    public static PhantasmaDemo Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(PhantasmaDemo)) as PhantasmaDemo : _instance; return _instance; }
    }

    private void Awake()
    {
        PhantasmaTokens = new Dictionary<string, Token>();
        MyCars          = new Dictionary<string, Car>();
    }

    private void Start ()
    {
        //GetAccount("P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr"); //TEST

        PhantasmaApi = new API("http://localhost:7077/rpc");
        
        Invoke("LoadPhantasmaData", 2f);
    }

    public void LoadPhantasmaData()
    {
        CheckTokens(() =>
        {
            CanvasManager.Instance.OpenLogin();
        });
    }

    private IEnumerator SyncBalance()
    {
        yield return new WaitForSeconds(2);
        //var balances = api.GetAssetBalancesOf(this.keys);
        //balance = balances.ContainsKey(assetSymbol) ? balances[assetSymbol] : 0;
        _state = EWALLET_STATE.UPDATE;
    }

    private void Update () {

        switch (_state)
        {
            case EWALLET_STATE.INIT:
                {
                    _state = EWALLET_STATE.SYNC;
                    StartCoroutine(SyncBalance());
                    break;
                }

            case EWALLET_STATE.UPDATE:
                {
                    _state = EWALLET_STATE.READY;
                    CanvasManager.Instance.accountMenu.SetBalance(_balance.ToString(CultureInfo.InvariantCulture));
                    break;
                }
        }		
	}

    public void Login(string privateKey)
    {
        Key = KeyPair.FromWIF(privateKey);

        GetAccount(Key.Address.ToString());
    }

    private void LoggedIn(string address)
    {
        Debug.Log("logged in: " + address);

        //var address = "L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25";
        //var addressBytes = Encoding.ASCII.GetBytes(address);
        
        CanvasManager.Instance.SetAddress(address);
        CanvasManager.Instance.CloseLogin();
    }

    public void LogOut()
    {
        //Debug.Log("logged out");

        // TODO something else here ?

        CanvasManager.Instance.ClearAddress();
        CanvasManager.Instance.OpenLogin();
    }

    #region Blockchain calls

    public void GetAccount(string address)
    {
        // Private key: L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25
        // Public key:  P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr

        Debug.Log("Get account: " + address);

        CanvasManager.Instance.ShowFetchingDataPopup("Fetching account data from the blockchain...");

        StartCoroutine(PhantasmaApi.GetAccount(address, 
            account =>
            {
                CanvasManager.Instance.accountMenu.SetBalance("Name: " + account.name);

                foreach (var balance in account.balances)
                {
                    var isFungible = PhantasmaTokens[balance.symbol].Flags.Contains("Fungible");

                    var amount = isFungible ? decimal.Parse(balance.amount) / (decimal) Mathf.Pow(10f, 8) : decimal.Parse(balance.amount);
                    CanvasManager.Instance.accountMenu.AddBalanceEntry("Chain: " + balance.chain + " - " + amount + " " + balance.symbol);

                    if (balance.symbol.Equals(TOKEN_SYMBOL))
                    {
                        TokenCurrentSupply = amount;

                        MyCars.Clear();

                        foreach (var tokenID in balance.ids)
                        {
                            StartCoroutine(PhantasmaApi.GetTokenData(TOKEN_SYMBOL, tokenID.ToString(), 
                                (tokenData =>
                                {
                                    var ramBytes        = Base16.Decode(tokenData.ram);
                                    var carMutableData  = Serialization.Unserialize<CarMutableData>(ramBytes);
                                    
                                    var romBytes    = Base16.Decode(tokenData.rom);
                                    var carData     = Serialization.Unserialize<CarData>(romBytes);

                                    var newCar = new Car();
                                    newCar.SetCar(Address.FromText(tokenData.ownerAddress), tokenID, carData, carMutableData);

                                    MyCars.Add(newCar.TokenID, newCar);
                                }),
                                (type, s) =>
                                {

                                }));
                        }
                    }
                }

                CanvasManager.Instance.HideFetchingDataPopup();

                LoggedIn(address);

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));
    }

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

    public void CreateToken()
    {
        CheckTokens(() =>
        {
            CanvasManager.Instance.ShowFetchingDataPopup("Creating a new token on the blockchain...");

            var script = ScriptUtils.BeginScript()
                .AllowGas(Key.Address, 1, 9999)
                //.CallContract("nexus", "CreateToken", Key.Address, TOKEN_SYMBOL, TOKEN_NAME, 0, 0, TokenFlags.Transferable)
                .CallContract("nexus", "CreateToken", Key.Address, TOKEN_SYMBOL, TOKEN_NAME, 10000, 0, TokenFlags.Transferable | TokenFlags.Finite)
                .SpendGas(Key.Address)
                .EndScript();

            StartCoroutine(PhantasmaApi.SignAndSendTransaction(script, "main",
                (result) =>
                {
                    Debug.Log("create token result: " + result);

                    StartCoroutine(CheckTokenCreation(result));
                },
                (errorType, errorMessage) =>
                {
                    CanvasManager.Instance.HideFetchingDataPopup();
                    CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
                }
            ));
        });
    }

    public IEnumerator CheckTokenCreation(string result)
    {
        CanvasManager.Instance.ShowFetchingDataPopup("Checking token creation...");

        yield return new WaitForSecondsRealtime(10f);
        
        yield return PhantasmaApi.GetTransaction(result, (tx) =>
        {
            foreach (var evt in tx.events)
            {
                if (Enum.TryParse(evt.kind, out EventKind eKind))
                {
                    if (eKind == EventKind.TokenCreate)
                    {
                        var bytes       = Base16.Decode(evt.data);
                        var tokenSymbol = Serialization.Unserialize<string>(bytes);

                        Debug.Log(evt.kind + " - " + tokenSymbol);

                        if (tokenSymbol.Equals(TOKEN_SYMBOL))
                        {
                            IsTokenCreated  = true;
                            IsTokenOwner    = true;

                            CheckTokens(() =>
                            {
                                CanvasManager.Instance.adminMenu.SetContent();
                                //PhantasmaApi.LogTransaction(Key.Address, 0, TransactionType.Created_Token, "CAR");
                            });
                        }

                        break;
                    }
                    else
                    {
                        CanvasManager.Instance.HideFetchingDataPopup();
                        // TODO aconteceu algum erro...
                    }
                }
                else
                {
                    CanvasManager.Instance.HideFetchingDataPopup();
                    // TODO aconteceu algum erro..
                }
            }
        });
    }

    public void CheckTokens(Action callback = null)
    {
        IsTokenCreated = false;

        CanvasManager.Instance.ShowFetchingDataPopup("Fetching Phantasma tokens...");
        
        PhantasmaTokens.Clear();

        StartCoroutine(PhantasmaApi.GetTokens(
            (result) =>
            {
                //Debug.Log("sign result tokens: " + result.Length);

                foreach (var token in result)
                {
                    PhantasmaTokens.Add(token.symbol, token);
                    //Debug.Log("ADD token: " + token.symbol);

                    if (token.symbol.Equals(TOKEN_SYMBOL))
                    {
                        Debug.Log("CREATED TRUE: " + token.symbol);
                        IsTokenCreated = true;
                        break;
                    }
                }

                CanvasManager.Instance.HideFetchingDataPopup();

                if (callback != null)
                {
                    callback();
                }

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideFetchingDataPopup();

                if (CanvasManager.Instance.loginMenu.gameObject.activeInHierarchy)
                {
                    CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
                }
                else
                {
                    CanvasManager.Instance.SetErrorMessage(errorType + " - "  + errorMessage);
                }
            }
        ));
    }

    // TODO merge this method with the GetTOkens
    public bool OwnsToken(string tokenSymbol, Action callback = null)
    {
        IsTokenOwner = false;

        CanvasManager.Instance.ShowFetchingDataPopup("Fetching tokens from the blockchain...");

        StartCoroutine(PhantasmaApi.GetTokens(
            (result) =>
            {
                foreach (var token in result)
                {
                    //Debug.Log("check token: " + token.symbol + " | owner: " + token.ownerAddress + " | MY: " + Key.Address);

                    if (token.symbol.Equals(TOKEN_SYMBOL) && token.ownerAddress.Equals(Key.Address.ToString()))
                    {
                        Debug.Log("IS TOKEN OWNER: " + token.symbol);
                        IsTokenOwner = true;
                        break;
                    }
                }

                CanvasManager.Instance.HideFetchingDataPopup();

                if (callback != null)
                {
                    callback();
                }
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideFetchingDataPopup();
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));

        return IsTokenOwner;
    }

    public void MintToken()
    {
        var carData = new CarData
        {
            rarity  = CarRarity.Common,
            imageID = Random.Range(0, carImages.Count)
        };

        var carMutableData = new CarMutableData
        {
            name        = "Super Cadillac",
            power       = (byte)Random.Range(1, 10),
            speed       = (byte)Random.Range(1, 10),
            location    = CarLocation.None,
        };

        var txData          = Serialization.Serialize(carData);
        var txMutableData   = Serialization.Serialize(carMutableData);

        var mintData = Base16.Encode(txData);

        Debug.Log("mint data: " + mintData);

        var script = ScriptUtils.BeginScript()
                        .AllowGas(Key.Address, 1, 9999)
                        .CallContract("token", "MintToken", Key.Address, TOKEN_SYMBOL, txData, txMutableData)
                        .SpendGas(Key.Address)
                        .EndScript();


        CanvasManager.Instance.ShowFetchingDataPopup("Minting a new token...");

        StartCoroutine(PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    foreach (var evt in tx.events)
                    {
                        if (Enum.TryParse(evt.kind, out EventKind eKind))
                        {
                            if (eKind == EventKind.TokenMint)
                            {
                                var bytes       = Base16.Decode(evt.data);
                                var tokenData   = Serialization.Unserialize<TokenEventData>(bytes);

                                var tokenID = tokenData.value;

                                Debug.Log("has event: " + evt.kind + " - car token id:" + tokenID);

                                var newCar = new Car();
                                newCar.SetCar(tokenData.chainAddress, tokenID.ToString(), carData, carMutableData);

                                // Add new car to admin assets
                                MyCars.Add(tokenID.ToString(), newCar);

                                //PhantasmaApi.LogTransaction(PhantasmaDemo.Instance.Key.Address, 0, TransactionType.Created_Car, carID);

                                CheckTokens(() =>
                                {
                                    CanvasManager.Instance.adminMenu.SetContent();
                                });

                                //CanvasManager.Instance.adminMenu.SetContent();
                                //CanvasManager.Instance.HideFetchingDataPopup();

                                break;
                            }
                            else
                            {
                                // TODO aconteceu algum erro...
                            }
                        }
                        else
                        {
                            // TODO aconteceu algum erro..
                        }
                    }

                }));

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));
    }

    #endregion
}
