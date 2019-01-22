using UnityEngine;
using UnityEngine.UI;

public class AdminMenu : MonoBehaviour
{
    public Button       createTokenButton, mintTokenButton;
    public Text         tokenSymbol, tokenName, myWalletTokens, currentSupplyTokens;
    public Text         errorMessage;
    public GameObject   tokenContent, supplyContent;

    private Color   _defaultColor;
    private int     _tokenSupply;

    void Awake()
    {
        _defaultColor = createTokenButton.targetGraphic.color;
    }

    void OnEnable()
    {
        ClearErrorContent();

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

                if (int.TryParse(token.currentSupply, out var suply))
                {
                    _tokenSupply = suply;
                }
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

    public void ShowError(string error)
    {
        // TODO change to use show error popup
        errorMessage.text = error;
        errorMessage.gameObject.SetActive(true);
    }

    private void ClearErrorContent()
    {
        errorMessage.text = string.Empty;
        errorMessage.gameObject.SetActive(false);
    }

    public void CreateTokenClicked()
    {
        PhantasmaDemo.Instance.CreateToken();
    }

    public void MintTokenClicked()
    {
        PhantasmaDemo.Instance.MintToken("Car " + (_tokenSupply + 1));
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAdmin();
    }
}
