using UnityEngine;

public class MainMenu : MonoBehaviour
{
    public void AccountClicked()
    {
        CanvasManager.Instance.OpenAccount();
    }

    public void LogoutClicked()
    {
        PhantasmaDemo.Instance.LogOut();
    }
}
