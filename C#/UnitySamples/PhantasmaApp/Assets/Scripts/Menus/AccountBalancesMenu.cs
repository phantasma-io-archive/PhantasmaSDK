using UnityEngine;
using UnityEngine.UI;

public class AccountBalancesMenu : MonoBehaviour
{
    public Text balanceLabel;

    public void SetBalance(string balance)
    {
        balanceLabel.text = balance + "\n";
        //Debug.Log("set balance: " + balanceLabel.text);
    }

    public void AddBalanceEntry(string entry)
    {
        balanceLabel.text += entry + "\n";
        //Debug.Log("add entry: " + balanceLabel.text);
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAccountBalancesMenu();
    }
}
