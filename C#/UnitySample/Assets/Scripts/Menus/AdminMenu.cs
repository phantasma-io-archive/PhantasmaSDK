using UnityEngine;
using UnityEngine.UI;

public class AdminMenu : MonoBehaviour
{
    public Button       createTokenButton, mintTokenButton;
    public Text         tokenSymbol, tokenName, myWalletTokens, currentSupplyTokens;
    public GameObject   tokenContent, supplyContent;

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
            CanvasManager.Instance.adminMenu.SetContent();
        });
    }

    public void SetContent()
    {
        //Debug.Log("set token: " + PhantasmaDemo.Instance.IsTokenCreated);

        if (PhantasmaDemo.Instance.IsTokenCreated)
        {
            createTokenButton.interactable          = false;
            createTokenButton.targetGraphic.color   = Color.gray;

            tokenContent.SetActive(true);

            mintTokenButton.interactable        = true;
            mintTokenButton.targetGraphic.color = _defaultColor;

            supplyContent.SetActive(true);

            if (PhantasmaDemo.Instance.PhantasmaTokens.ContainsKey(PhantasmaDemo.TOKEN_SYMBOL))
            {
                var token = PhantasmaDemo.Instance.PhantasmaTokens[PhantasmaDemo.TOKEN_SYMBOL];

                tokenSymbol.text    = token.symbol;
                tokenName.text      = token.name;

                myWalletTokens.text         = PhantasmaDemo.Instance.MyCars.Count.ToString();
                currentSupplyTokens.text    = token.currentSupply;
            }
        }
        else
        {
            createTokenButton.interactable          = true;
            createTokenButton.targetGraphic.color   = _defaultColor;

            tokenContent.SetActive(false);

            mintTokenButton.interactable        = false;
            mintTokenButton.targetGraphic.color = Color.gray;

            supplyContent.SetActive(false);

            tokenSymbol.text    = string.Empty;
            tokenName.text      = string.Empty;

            myWalletTokens.text         = "0";
            currentSupplyTokens.text    = "0";
        }
    }

    public void CreateTokenClicked()
    {
        PhantasmaDemo.Instance.CreateToken();
    }

    public void MintTokenClicked()
    {
        PhantasmaDemo.Instance.MintToken();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAdmin();
    }
}
