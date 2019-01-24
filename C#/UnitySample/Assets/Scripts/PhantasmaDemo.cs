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
    public const float TRANSACTION_CONFIRMATION_DELAY = 10f;

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

        PhantasmaApi = new API(_SERVER_ADDRESS);
        
        Invoke("LoadPhantasmaData", 2f);
    }

    /// <summary>
    /// Load the tokens deployed on Phantasma Blockchain
    /// </summary>
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

    /// <summary>
    /// Generate a new WIF
    /// </summary>
    /// <param name="callback"></param>
    public void GenerateNewKey(Action<string> callback = null)
    {
        var newKey = KeyPair.Generate().ToWIF();

        if (callback != null)
        {
            callback(newKey);
        }
    }

    /// <summary>
    /// Log in to Phantasma Blockchain
    /// </summary>
    /// <param name="privateKey">User private key</param>
    public void Login(string privateKey)
    {
        try
        {
            Key = KeyPair.FromWIF(privateKey);

            GetAccount(Key.Address.ToString());
        }
        catch (Exception e)
        {
            CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, "Not valid Private Key: " + e.Message);   
        }
    }

    private void LoggedIn(string address)
    {
        Debug.Log("logged in: " + address);
       
        CanvasManager.Instance.SetAddress(address);
        CanvasManager.Instance.CloseLogin();
    }

    public void LogOut()
    {
        IsTokenCreated  = false;
        IsTokenOwner    = false;
        
        TokenCurrentSupply = 0;

        MyCars.Clear();

        CanvasManager.Instance.mainMenu.SetAdminButton();

        CanvasManager.Instance.myAssetsMenu.ClearContent();
        CanvasManager.Instance.marketMenu.ClearContent();

        CanvasManager.Instance.ClearAddress();
        CanvasManager.Instance.OpenLogin();
    }

    #region Blockchain calls

    public IEnumerator CheckOperation(string transactionHash, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)
    {
        var isTransactionCompleted = false;

        while (!isTransactionCompleted)
        {
            yield return new WaitForSecondsRealtime(TRANSACTION_CONFIRMATION_DELAY);

            yield return PhantasmaApi.GetTransaction(transactionHash,
                (tx) =>
                {
                    isTransactionCompleted = true;

                    if (callback != null)
                    {
                        callback(tx);
                    }
                },
                (errorType, errorMessage) =>
                {
                    if (errorType == EPHANTASMA_SDK_ERROR_TYPE.API_ERROR && errorMessage.Equals("pending"))
                    {
                        Debug.Log("PENDING TRANSACTION");
                        // recursive test
                        //StartCoroutine(CheckOperation(transactionHash, callback, errorHandlingCallback));
                        //return;
                    }
                    else
                    {
                        isTransactionCompleted = true;

                        if (errorHandlingCallback != null)
                        {
                            errorHandlingCallback(errorType, errorMessage);
                        }
                    }
                });
        }
    }

    /// <summary>
    /// Returns the account name and balance of given address.
    /// </summary>
    /// <param name="address">String, base58 encoded - address to check for balance and name.</param>
    public void GetAccount(string address)
    {
        // Second test account: KyHrxZyrGPorgJKLv4Cg6dm5xCjEb6k8USRoSysVXAQK6eEb5taU

        // Private key: L2LGgkZAdupN2ee8Rs6hpkc65zaGcLbxhbSDGq8oh6umUxxzeW25
        // Public key:  P2f7ZFuj6NfZ76ymNMnG3xRBT5hAMicDrQRHE4S7SoxEr

        Debug.Log("Get account: " + address);

        CanvasManager.Instance.ShowOperationPopup("Fetching account data from the blockchain...");

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

                CanvasManager.Instance.HideOperationPopup();

                LoggedIn(address);

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Create a new token on Phantasma Blockchain
    /// </summary>
    public void CreateToken()
    {
        CheckTokens(() =>
        {
            CanvasManager.Instance.ShowOperationPopup("Creating a new token on the blockchain...");

            var script = ScriptUtils.BeginScript()
                .AllowGas(Key.Address, 1, 9999)
                //.CallContract("nexus", "CreateToken", Key.Address, TOKEN_SYMBOL, TOKEN_NAME, 0, 0, TokenFlags.Transferable)
                .CallContract("nexus", "CreateToken", Key.Address, TOKEN_SYMBOL, TOKEN_NAME, 10000, 0, TokenFlags.Transferable | TokenFlags.Finite)
                .SpendGas(Key.Address)
                .EndScript();

            StartCoroutine(PhantasmaApi.SignAndSendTransaction(script, "main",
                (result) =>
                {
                    StartCoroutine(CheckTokenCreation(result));
                },
                (errorType, errorMessage) =>
                {
                    CanvasManager.Instance.HideOperationPopup();
                    CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
                }
            ));
        });
    }

    /// <summary>
    /// Check if the creation of a new token on Phantasma Blockchain was successful
    /// </summary>
    public IEnumerator CheckTokenCreation(string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking token creation...");

        yield return CheckOperation(result, 
            (tx) =>
            {
                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        if (eKind == EventKind.TokenCreate)
                        {
                            var bytes = Base16.Decode(evt.data);
                            var tokenSymbol = Serialization.Unserialize<string>(bytes);

                            Debug.Log(evt.kind + " - " + tokenSymbol);

                            if (tokenSymbol.Equals(TOKEN_SYMBOL))
                            {
                                IsTokenCreated = true;
                                IsTokenOwner = true;

                                CheckTokens(() =>
                                {
                                    CanvasManager.Instance.adminMenu.SetContent();
                                    //PhantasmaApi.LogTransaction(Key.Address, 0, TransactionType.Created_Token, "CAR");
                                });

                                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.SUCCESS, "New token created with success.");
                            }

                            return;
                        }
                    }
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, "Something failed on the connection to the blockchain. Please try again.");
            },
            ((errorType, errorMessage) =>
            {
                Debug.Log("FAIL create token");

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
            }));
    }

    /// <summary>
    /// Check the tokens deployed in Phantasma Blockchain.
    /// </summary>
    /// <param name="callback"></param>
    public void CheckTokens(Action callback = null)
    {
        IsTokenCreated = false;

        CanvasManager.Instance.ShowOperationPopup("Fetching Phantasma tokens...");
        
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
                        //Debug.Log("CREATED TRUE: " + token.symbol);
                        IsTokenCreated = true;
                        break;
                    }
                }

                CanvasManager.Instance.HideOperationPopup();

                if (callback != null)
                {
                    callback();
                }

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();

                if (CanvasManager.Instance.loginMenu.gameObject.activeInHierarchy)
                {
                    CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
                }
                else if (CanvasManager.Instance.adminMenu.gameObject.activeInHierarchy)
                {
                    CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
                }
                else
                {
                    CanvasManager.Instance.SetErrorMessage(errorType + " - "  + errorMessage);
                }
            }
        ));
    }

    /// <summary>
    /// Check if the logged in address is the owner of a token
    /// </summary>
    /// <param name="tokenSymbol">Symbol of the token</param>
    public bool OwnsToken(string tokenSymbol, Action callback = null)
    {
        IsTokenOwner = false;

        CanvasManager.Instance.ShowOperationPopup("Fetching tokens from the blockchain...");

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

                CanvasManager.Instance.HideOperationPopup();

                if (callback != null)
                {
                    callback();
                }
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
            }
        ));

        return IsTokenOwner;
    }

    /// <summary>
    /// Mint a new token and increase the supply of the created token
    /// </summary>
    public void MintToken(string tokenName)
    {
        var carData = new CarData
        {
            rarity  = CarRarity.Common,
            imageID = Random.Range(0, carImages.Count)
        };

        var carMutableData = new CarMutableData
        {
            name        = tokenName,
            power       = (byte)Random.Range(1, 10),
            speed       = (byte)Random.Range(1, 10),
            location    = CarLocation.None,
        };

        var txData          = Serialization.Serialize(carData);
        var txMutableData   = Serialization.Serialize(carMutableData);

        var script = ScriptUtils.BeginScript()
                        .AllowGas(Key.Address, 1, 9999)
                        .CallContract("token", "MintToken", Key.Address, TOKEN_SYMBOL, txData, txMutableData)
                        .SpendGas(Key.Address)
                        .EndScript();
        
        CanvasManager.Instance.ShowOperationPopup("Minting a new token...");

        StartCoroutine(PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                StartCoroutine(CheckTokenMint(carData, carMutableData, result));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Check if the auction purchase was successful
    /// </summary>
    private IEnumerator CheckTokenMint(CarData carData, CarMutableData carMutableData, string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking token mint...");

        yield return CheckOperation(result,
            (tx) =>
            {
                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        if (eKind == EventKind.TokenMint)
                        {
                            var bytes = Base16.Decode(evt.data);
                            var tokenData = Serialization.Unserialize<TokenEventData>(bytes);

                            var tokenID = tokenData.value;

                            Debug.Log("has event: " + evt.kind + " - car token id:" + tokenID);

                            var newCar = new Car();
                            newCar.SetCar(tokenData.chainAddress, tokenID.ToString(), carData, carMutableData);

                            // Add new car to admin assets
                            MyCars.Add(tokenID.ToString(), newCar);

                            //PhantasmaApi.LogTransaction(PhantasmaDemo.Instance.Key.Address, 0, TransactionType.Created_Car, carID);

                            CheckTokens(() => { CanvasManager.Instance.adminMenu.SetContent(); });

                            CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.SUCCESS, "Token mint with success.");

                            return;
                        }
                    }
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, "Something failed while executing a new token mint. Please try again.");
            },
            ((errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, errorType + " - " + errorMessage);
            }));
    }

    #endregion
}
