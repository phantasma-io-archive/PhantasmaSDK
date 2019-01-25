using UnityEngine;
using UnityEngine.UI;

public class MainMenu : MonoBehaviour
{
    public Button adminButton;

    // Start is called before the first frame update
    void Start()
    {
        //PhantasmaDemo.Instance.OwnsToken(PhantasmaDemo.TOKEN_SYMBOL, () =>
        //{
        //    CanvasManager.Instance.mainMenu.SetAdminButton();
        //});
    }

    public void SetAdminButton()
    {
        //adminButton.gameObject.SetActive(!PhantasmaDemo.Instance.IsTokenCreated || PhantasmaDemo.Instance.IsTokenCreated && PhantasmaDemo.Instance.IsTokenOwner);
    }

    public void AccountClicked()
    {
        CanvasManager.Instance.OpenAccount();
    }

    public void LogoutClicked()
    {
        //PhantasmaDemo.Instance.LogOut();
    }

}
