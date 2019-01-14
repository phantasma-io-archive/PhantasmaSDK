using UnityEngine;
using UnityEngine.UI;

public class AdminMenu : MonoBehaviour
{
    public Button createTokenButton;

    // Start is called before the first frame update
    void Start()
    {
        PhantasmaDemo.Instance.CheckTokenCreation(() =>
        {
            CanvasManager.Instance.adminMenu.SetTokenButton();
        });
    }
    
    public void SetTokenButton()
    {
        Debug.Log("set token: " + PhantasmaDemo.Instance.IsTokenCreated);
        createTokenButton.interactable = !PhantasmaDemo.Instance.IsTokenCreated;

        if (!createTokenButton.IsInteractable())
        {
            Debug.Log("a");
            var color = createTokenButton.targetGraphic.color;
            createTokenButton.targetGraphic.color = new Color(color.r, color.g, color.b, 0.5f);
        }
        else
        {
            Debug.Log("b");
        }


    }
    public void CreateTokenClicked()
    {
        PhantasmaDemo.Instance.CreateToken();
    }

    public void PopulateMarketClicked()
    {
        PhantasmaDemo.Instance.market.FillMarket();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAdmin();
    }
}
