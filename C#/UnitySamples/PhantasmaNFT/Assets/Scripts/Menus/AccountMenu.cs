using UnityEngine;
using UnityEngine.UI;

public class AccountMenu : MonoBehaviour
{
    public Text balanceLabel;

    public void SetBalance(string balance)
    {
        balanceLabel.text = balance + "\n";
    }

    public void AddBalanceEntry(string entry)
    {
        balanceLabel.text += entry + "\n";
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAccount();
    }
}
