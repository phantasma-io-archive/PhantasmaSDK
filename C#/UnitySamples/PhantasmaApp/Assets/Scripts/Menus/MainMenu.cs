using UnityEngine;

public class MainMenu : MonoBehaviour
{
    public void AccountBalancesClicked()
    {
        CanvasManager.Instance.OpenAccount();
    }

    public void TranferTokensClicked()
    {
        CanvasManager.Instance.OpenTransferTokensMenu();
    }

    public void TransactionsHistoryClicked()
    {
        CanvasManager.Instance.OpenTransactionsHistoryMenu();
    }

    public void LogoutClicked()
    {
        PhantasmaDemo.Instance.LogOut();
    }
}
