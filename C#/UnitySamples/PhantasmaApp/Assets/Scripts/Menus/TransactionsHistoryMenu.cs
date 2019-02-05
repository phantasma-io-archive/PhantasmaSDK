using Phantasma.SDK;
using UnityEngine;
using UnityEngine.UI;

public class TransactionsHistoryMenu : MonoBehaviour
{
    public Text     transactionsLabel;
    public Text     message;
    public Button   refreshButton;

    //public void SetBalance(string balance)
    //{
    //    balanceLabel.text = balance + "\n";
    //    //Debug.Log("set balance: " + balanceLabel.text);
    //}

    void OnEnable()
    {

    }

    private void GetTransactions()
    {
        PhantasmaDemo.Instance.GetTransactions(SetContent, () =>
        {
            ShowRefreshButton();
        });
    }

    public void SetContent(AccountTransactions accountTransactions)
    {
        //Debug.Log("menu set content: " + auctions.Length);
        if (accountTransactions.txs.Length == 0)
        {
            CanvasManager.Instance.HideOperationPopup();
            ShowRefreshButton("There are no transactions associated to your address on the blockchain.");
            return;
        }

        message.gameObject.SetActive(false);
        refreshButton.gameObject.SetActive(false);

        //Debug.Log("buy slots: " + _buySlots.Count + " | market buy: " + PhantasmaDemo.Instance.market.BuyCarAuctions.Keys.Count);

        foreach (var transaction in accountTransactions.txs)
        {
            AddHistoryEntry(transaction.chainAddress + " | " + transaction.timestamp);
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
