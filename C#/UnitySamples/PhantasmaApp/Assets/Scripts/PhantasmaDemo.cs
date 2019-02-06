using System;
using System.Collections;
using System.Collections.Generic;
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

    private decimal                 _balance;
    private IEnumerator             _pendingTxCoroutine;
    private string                  _lastTransactionHash;
    private EBLOCKCHAIN_OPERATION   _lastTransactionType;

    public API                          PhantasmaApi        { get; private set; }
    public Dictionary<string, Token>    PhantasmaTokens     { get; private set; }
    public List<Transaction>            LastTransactions    { get; private set; }

    private static PhantasmaDemo _instance;
    public static PhantasmaDemo Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(PhantasmaDemo)) as PhantasmaDemo : _instance; return _instance; }
    }

    private void Awake()
    {
        PhantasmaTokens     = new Dictionary<string, Token>();
        LastTransactions    = new List<Transaction>();
    }

    private void Start ()
    {
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
                        // Pending Transaction
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
        CanvasManager.Instance.ShowOperationPopup("Fetching account data from the blockchain...", false);

        StartCoroutine(PhantasmaApi.GetAccount(address, 
            account =>
            {
                CanvasManager.Instance.accountBalancesMenu.SetBalance("Name: " + account.name);

                foreach (var balance in account.balances)
                {
                    var isFungible = PhantasmaTokens[balance.symbol].flags.Contains("Fungible");

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
                foreach (var token in result)
                {
                    PhantasmaTokens.Add(token.symbol, token);
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
    /// Returns the last transactions associated to the login address
    /// </summary>
    public void GetTransactions(Action<AccountTransactions> successCallback = null, Action errorCallback = null)
    {
        StartCoroutine(GetTransactionsHistoryCoroutine(successCallback, errorCallback));
    }

    private IEnumerator GetTransactionsHistoryCoroutine(Action<AccountTransactions> successCallback = null, Action errorCallback = null)
    {
        CanvasManager.Instance.ShowOperationPopup("Fetching last transactions from the blockchain...", false);

        uint itemsPerPage = 20;

        yield return PhantasmaApi.GetAddressTransactions(Key.Address.Text, 1, itemsPerPage,
            (accountTransactions, currentPage, totalPages) =>
            {
                StartCoroutine(ProcessTransactions(accountTransactions, currentPage, totalPages, successCallback, errorCallback));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);

                if (errorCallback != null)
                {
                    errorCallback();
                }
            });
    }

    private IEnumerator ProcessTransactions(AccountTransactions accountTransactions, int currentPage, int totalPages, Action<AccountTransactions> successCallback = null, Action errorCallback = null)
    {
        if (currentPage < totalPages)
        {
            yield return PhantasmaApi.GetAddressTransactions(Key.Address.Text, (uint)currentPage + 1, (uint)totalPages,
                (accountTxs, cPage, tPages) =>
                {
                    StartCoroutine(ProcessTransactions(accountTxs, cPage, tPages, successCallback, errorCallback));
                },
                (errorType, errorMessage) =>
                {
                    CanvasManager.Instance.HideOperationPopup();
                    CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
                });
        }
        else
        {
            CanvasManager.Instance.HideOperationPopup();

            if (totalPages == 0)
            {
                CanvasManager.Instance.transactionsHistoryMenu.ShowRefreshButton("No transactions associated to your address on the blockchain.");
            }
            else
            {               
                LastTransactions.Clear();
                LastTransactions.AddRange(accountTransactions.txs);

                if (successCallback != null)
                {
                    successCallback(accountTransactions);
                }
            }
        }
    }

    public void TransferTokens(Address from, Address to, string tokenSymbol, BigInteger amount)
    {
        CanvasManager.Instance.ShowOperationPopup("Transferring tokens between addresses...", false);

        var script = ScriptUtils.BeginScript()
            .AllowGas(Key.Address, Address.Null, 1, 9999)
            .CallContract("token", "TransferTokens", from, to, tokenSymbol, amount)
            .SpendGas(Key.Address)
            .EndScript();

        StartCoroutine(PhantasmaApi.SignAndSendTransaction(Key, script, "main",
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
    /// Check if the token transfer was successful
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
                    if (Enum.TryParse(evt.kind, out EventKind eKind))
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
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }));
    }

    #endregion
}
