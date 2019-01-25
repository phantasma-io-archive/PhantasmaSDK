using UnityEngine;
using UnityEngine.UI;

public class TransactionsHistoryMenu : MonoBehaviour
{
    public Text transactionsLabel;

    //public void SetBalance(string balance)
    //{
    //    balanceLabel.text = balance + "\n";
    //    //Debug.Log("set balance: " + balanceLabel.text);
    //}

    public void AddHistoryEntry(string entry)
    {
        transactionsLabel.text += entry + "\n";
        //Debug.Log("add entry: " + transactionsLabel.text);
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseTransactionsHistoryMenu();
    }
}
