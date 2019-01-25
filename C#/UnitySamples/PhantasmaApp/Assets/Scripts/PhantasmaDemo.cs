using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;

using UnityEngine;

using Phantasma.Cryptography;
using Phantasma.SDK;
using Token = Phantasma.SDK.Token;

public enum EOPERATION_RESULT
{
    FAIL,
    SUCCESS
}

public class PhantasmaDemo : MonoBehaviour
{
    public enum EBLOCKCHAIN_OPERATION
    {

    }

    private readonly Dictionary<EBLOCKCHAIN_OPERATION, string> _BLOCKCHAIN_OPERATION_DESCRIPTION = new Dictionary<EBLOCKCHAIN_OPERATION, string> {};

    public const string TOKEN_SYMBOL    = "CAR";
    public const string TOKEN_NAME      = "Car Demo Token";

    private const string _SERVER_ADDRESS = "http://localhost:7077/rpc";

    private const float _TRANSACTION_CONFIRMATION_DELAY = 10f;

    public KeyPair Key { get; private set; }

    private enum EWALLET_STATE
    {
        INIT,
        SYNC,
        UPDATE,
        READY
    }

    public List<Sprite> carImages;

    private EWALLET_STATE   _state = EWALLET_STATE.INIT;
    private decimal         _balance;

    public API                          PhantasmaApi        { get; private set; }
    public Dictionary<string, Token>    PhantasmaTokens     { get; private set; }

    private IEnumerator             _pendingTxCoroutine;
    private string                  _lastTransactionHash;
    private EBLOCKCHAIN_OPERATION   _lastTransactionType;

    private static PhantasmaDemo _instance;
    public static PhantasmaDemo Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(PhantasmaDemo)) as PhantasmaDemo : _instance; return _instance; }
    }

    private void Awake()
    {
        PhantasmaTokens = new Dictionary<string, Token>();
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
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Not valid Private Key: " + e.Message);   
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
        CanvasManager.Instance.ClearAddress();
        CanvasManager.Instance.OpenLogin();
    }

    #region Blockchain calls

    public IEnumerator CheckOperation(EBLOCKCHAIN_OPERATION operation, string transactionHash, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)
    {
        if (_pendingTxCoroutine != null)
        {
            StopCoroutine(_pendingTxCoroutine);

            _pendingTxCoroutine = null;
        }

        _pendingTxCoroutine = CheckOperationCoroutine(operation, transactionHash, callback, errorHandlingCallback);

        yield return StartCoroutine(_pendingTxCoroutine);
    }

    private IEnumerator CheckOperationCoroutine(EBLOCKCHAIN_OPERATION operation, string transactionHash, Action<Transaction> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)
    {
        _lastTransactionType = operation;
        _lastTransactionHash = transactionHash;

        var isTransactionCompleted = false;

        while (!isTransactionCompleted)
        {
            yield return new WaitForSecondsRealtime(_TRANSACTION_CONFIRMATION_DELAY);

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

    public void CancelTransaction()
    {
        //if (_pendingTxCoroutine != null)
        //{
        //    // set flag pause to true
        //    _pendingTxCoroutine
        //}

        StartCoroutine(CancelTransactionCoroutine(_lastTransactionHash));
    }

    private IEnumerator CancelTransactionCoroutine(string transactionHash)
    {
        yield return PhantasmaApi.CancelTransaction(transactionHash,
            (tx) =>
            {
                CanvasManager.Instance.ShowCancelOperationPopup(EOPERATION_RESULT.SUCCESS, "The operation '" + _BLOCKCHAIN_OPERATION_DESCRIPTION[_lastTransactionType] + "' was canceled with success.");
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.ShowCancelOperationPopup(EOPERATION_RESULT.FAIL, 
                    "The transaction regarding the operation '" + _BLOCKCHAIN_OPERATION_DESCRIPTION[_lastTransactionType] + "' is already being processed by the blockchain and cannot be canceled anymore.");
            });
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

        CanvasManager.Instance.ShowOperationPopup("Fetching account data from the blockchain...", false);

        StartCoroutine(PhantasmaApi.GetAccount(address, 
            account =>
            {
                CanvasManager.Instance.accountMenu.SetBalance("Name: " + account.name);

                foreach (var balance in account.balances)
                {
                    var isFungible = PhantasmaTokens[balance.symbol].Flags.Contains("Fungible");

                    var amount = isFungible ? decimal.Parse(balance.amount) / (decimal) Mathf.Pow(10f, 8) : decimal.Parse(balance.amount);
                    CanvasManager.Instance.accountMenu.AddBalanceEntry("Chain: " + balance.chain + " - " + amount + " " + balance.symbol);
                }

                CanvasManager.Instance.HideOperationPopup();

                LoggedIn(address);

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }
    
    /// <summary>
    /// Check the tokens deployed in Phantasma Blockchain.
    /// </summary>
    /// <param name="callback"></param>
    public void CheckTokens(Action callback = null)
    {
        CanvasManager.Instance.ShowOperationPopup("Fetching Phantasma tokens...", false);
        
        PhantasmaTokens.Clear();

        StartCoroutine(PhantasmaApi.GetTokens(
            (result) =>
            {
                //Debug.Log("sign result tokens: " + result.Length);

                foreach (var token in result)
                {
                    PhantasmaTokens.Add(token.symbol, token);
                    //Debug.Log("ADD token: " + token.symbol);
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
                    CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
                }
                else
                {
                    CanvasManager.Instance.SetErrorMessage(errorType + " - "  + errorMessage);
                }
            }
        ));
    }
    
    #endregion
}
