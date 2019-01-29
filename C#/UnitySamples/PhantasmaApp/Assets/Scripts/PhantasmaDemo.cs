using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using Phantasma.Blockchain;
using Phantasma.Blockchain.Contracts;
using UnityEngine;

using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.SDK;
using Phantasma.VM.Utils;
using Token = Phantasma.SDK.Token;
using Transaction = Phantasma.SDK.Transaction;

public enum EOPERATION_RESULT
{
    FAIL,
    SUCCESS
}

public class PhantasmaDemo : MonoBehaviour
{
    public enum EBLOCKCHAIN_OPERATION
    {
        TRANSFER_TOKENS
    }

    private readonly Dictionary<EBLOCKCHAIN_OPERATION, string> _BLOCKCHAIN_OPERATION_DESCRIPTION = new Dictionary<EBLOCKCHAIN_OPERATION, string>
    {
        { EBLOCKCHAIN_OPERATION.TRANSFER_TOKENS, "Transfer Tokens" }
    };
    
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
                    CanvasManager.Instance.accountBalancesMenu.SetBalance(_balance.ToString(CultureInfo.InvariantCulture));
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

            yield return new WaitForSecondsRealtime(_TRANSACTION_CONFIRMATION_DELAY);
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
                if (_pendingTxCoroutine != null)
                {
                    StopCoroutine(_pendingTxCoroutine);

                    _pendingTxCoroutine = null;
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideResultPopup();
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

        //_keyField.Content = "Kws5PuviJskdo2V7hGnr37T1Bt4ymEpnLWYyStySvxXwXAiANNuH"; // P54Dd8E4khN4LeVTBXZtS2FSk4666mspuM6aUGtVQS2yX
        //_keyField.Content = "L17u4Eu5eXpdguGxVFV1a5sr5mmGf389nodq9gJYPVMJS9PVrbnm"; // PDSMhv7EeXoAnedYSsHZhavougRJ9rt99kQYzztJDzWkd

        Debug.Log("Get account: " + address);

        CanvasManager.Instance.ShowOperationPopup("Fetching account data from the blockchain...", false);

        StartCoroutine(PhantasmaApi.GetAccount(address, 
            account =>
            {
                CanvasManager.Instance.accountBalancesMenu.SetBalance("Name: " + account.name);

                foreach (var balance in account.balances)
                {
                    var isFungible = PhantasmaTokens[balance.symbol].Flags.Contains("Fungible");

                    var amount = isFungible ? decimal.Parse(balance.amount) / (decimal) Mathf.Pow(10f, 8) : decimal.Parse(balance.amount);
                    CanvasManager.Instance.accountBalancesMenu.AddBalanceEntry("Chain: " + balance.chain + " - " + amount + " " + balance.symbol);
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

    /// <summary>
    /// Returns the account name and balance of given address.
    /// </summary>
    /// <param name="address">String, base58 encoded - address to check for balance and name.</param>
    public void GetTransactions(string address)
    {
        Debug.Log("Get Transactions: " + address);

        CanvasManager.Instance.ShowOperationPopup("Fetching last transactions from the blockchain...", false);

        //StartCoroutine(PhantasmaApi.GetTransactions(address,
        //    account =>
        //    {
        //        CanvasManager.Instance.accountBalancesMenu.SetBalance("Name: " + account.name);

        //        foreach (var balance in account.balances)
        //        {
        //            var isFungible = PhantasmaTokens[balance.symbol].Flags.Contains("Fungible");

        //            var amount = isFungible ? decimal.Parse(balance.amount) / (decimal)Mathf.Pow(10f, 8) : decimal.Parse(balance.amount);
        //            CanvasManager.Instance.transactionsHistoryMenu.AddHistoryEntry("Chain: " + balance.chain + " - " + amount + " " + balance.symbol);
        //        }

        //        CanvasManager.Instance.HideOperationPopup();

        //        LoggedIn(address);

        //    },
        //    (errorType, errorMessage) =>
        //    {
        //        CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
        //    }
        //));
    }

    public void TransferTokens(Address from, Address to, string tokenSymbol, BigInteger amount)
    {
        CanvasManager.Instance.ShowOperationPopup("Transfering tokens between addresses...", false);

        var script = ScriptUtils.BeginScript()
            .AllowGas(Key.Address, 1, 9999)
            .CallContract("token", "TransferTokens", from, to, tokenSymbol, amount)
            .SpendGas(Key.Address)
            .EndScript();

        StartCoroutine(PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                StartCoroutine(CheckTokensTransfer(result));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Check if the auction purchase was successful
    /// </summary>
    private IEnumerator CheckTokensTransfer(string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking auction purchase...", true);

        yield return CheckOperation(EBLOCKCHAIN_OPERATION.TRANSFER_TOKENS, result,
            (tx) =>
            {
                var sentTokens      = false;
                var receivedTokens  = false;

                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        switch (eKind)
                        {
                            case EventKind.TokenSend:
                                sentTokens = true;
                                break;
                            case EventKind.TokenReceive:
                                receivedTokens = true;
                                break;
                        }
                    }
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ClearTransferTokensMenu();

                if (sentTokens && receivedTokens)
                {
                    CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.SUCCESS, "Tokens where transferred with success.");
                }
                else
                {
                    CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Something failed while transferring tokens. Please try again.");
                }
            },
            ((errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                //CanvasManager.Instance.ClearTransferTokensMenu();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }));
    }

    #endregion
}
