using System;
using Phantasma.SDK;
using UnityEngine;
using UnityEngine.UI;

public class TransactionsHistoryMenu : MonoBehaviour
{
    public Text     transactionsLabel;
    public Text     message;
    public Button   refreshButton;

    void OnEnable()
    {
        message.gameObject.SetActive(false);
        refreshButton.gameObject.SetActive(false);

        GetTransactions();
    }

    private void GetTransactions()
    {
        Debug.Log("get transactions");
        PhantasmaDemo.Instance.GetTransactions(SetContent, () =>
        {
            ShowRefreshButton();
        });
    }

    public void SetContent(AccountTransactions accountTransactions)
    {
        Debug.Log("menu set content: " + accountTransactions.txs.Length);
        if (accountTransactions.txs.Length == 0)
        {
            CanvasManager.Instance.HideOperationPopup();
            ShowRefreshButton("There are no transactions associated to your address on the blockchain.");
            return;
        }

        message.text = string.Empty;
        message.gameObject.SetActive(false);
        refreshButton.gameObject.SetActive(false);

        transactionsLabel.text = "\n";

        var newEntry = string.Empty;

        for (var i = 0; i < accountTransactions.txs.Length; i++)
        {
            var transaction = accountTransactions.txs[i];

            newEntry = (i + 1) + " - " + transaction.chainAddress + "\nDate: " + new DateTime(transaction.timestamp) + "\nEvents: ";

            foreach (var transactionEvent in transaction.events)
            {
                var kind = transactionEvent.kind;

                if (kind == "GasEscrow" || kind == "GasPayment") continue;

                newEntry += transactionEvent.kind + " ; ";
            }

            AddHistoryEntry(newEntry);
        }
    }

    public void AddHistoryEntry(string entry)
    {
        transactionsLabel.text += entry + "\n";
        //Debug.Log("add entry: " + transactionsLabel.text);
    }

    public void ShowRefreshButton(string msg = null)
    {
        message.text = msg;
        message.gameObject.SetActive(true);

        refreshButton.gameObject.SetActive(true);
    }

    public void RefreshClicked()
    {
        GetTransactions();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseTransactionsHistoryMenu();
    }
}
