using UnityEngine;
using UnityEngine.UI;

public class AdminMenu : MonoBehaviour
{
    public Button createTokenButton;

    // Start is called before the first frame update
    void Start()
    {
        createTokenButton.interactable = !PhantasmaDemo.Instance.IsTokenCreated();
    }

    // Update is called once per frame
    void Update()
    {
        
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
