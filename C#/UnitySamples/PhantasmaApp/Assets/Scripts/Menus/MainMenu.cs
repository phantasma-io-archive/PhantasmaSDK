using UnityEngine;

public class MainMenu : MonoBehaviour
{
    public void AccountBalancesClicked()
    {
        CanvasManager.Instance.OpenAccountBalancesMenu();
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
