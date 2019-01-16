using UnityEngine;
using UnityEngine.UI;

public class AdminMenu : MonoBehaviour
{
    public Button createTokenButton, populateMarketButton;

    private Color _defaultColor;

    void Awake()
    {
        _defaultColor = createTokenButton.targetGraphic.color;
    }

    // Start is called before the first frame update
    void Start()
    {

    }

    void OnEnable()
    {
        PhantasmaDemo.Instance.CheckTokens(() =>
        {
            CanvasManager.Instance.adminMenu.SetButtons();
        });
    }

    public void SetButtons()
    {
        Debug.Log("set token: " + PhantasmaDemo.Instance.IsTokenCreated);

        if (PhantasmaDemo.Instance.IsTokenCreated)
        {
            createTokenButton.interactable          = false;
            createTokenButton.targetGraphic.color   = Color.gray;

            if (PhantasmaDemo.Instance.TokenCurrentSupply == Market.MARKET_CARS_COUNT)
            {
                populateMarketButton.interactable           = false;
                populateMarketButton.targetGraphic.color    = Color.gray;
            }
            else
            {
                populateMarketButton.interactable = true;
                populateMarketButton.targetGraphic.color = _defaultColor;
            }
        }
        else
        {
            createTokenButton.interactable          = true;
            createTokenButton.targetGraphic.color   = _defaultColor;

            populateMarketButton.interactable           = true;
            populateMarketButton.targetGraphic.color    = _defaultColor;
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
