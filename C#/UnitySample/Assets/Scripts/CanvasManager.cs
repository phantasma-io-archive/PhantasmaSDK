using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

public class CanvasManager : MonoBehaviour {

    public Text         addressLabel, balanceLabel, loginError;
    public GameObject   balanceScroll, login, buttons, market;
    public Button       startButton;
    public InputField   addressInputField;
    public AssetSlot    assetSlot;

    private static CanvasManager _instance;
    public static CanvasManager Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(CanvasManager)) as CanvasManager : _instance; return _instance; }
    }

    // Use this for initialization
    void Start () {

        market.SetActive(false);
        buttons.SetActive(false);

        ToggleLogin(true);
    }
	
	// Update is called once per frame
	void Update () {
		
	}

    public void ToggleLogin(bool isVisible, string address = null)
    {
        //Debug.Log("Toggle Login: " + isVisible);
        addressLabel.text = address == null ? string.Empty : address;
        addressLabel.gameObject.SetActive(!isVisible);

        balanceScroll.gameObject.SetActive(!isVisible);

        loginError.text = string.Empty;
        loginError.gameObject.SetActive(!isVisible);

        addressInputField.text = string.Empty;

        login.SetActive(isVisible);

        buttons.SetActive(!isVisible);
    }

    public void SetAddress(string address)
    {
        addressLabel.text = address;
    }

    public void SetBalance(string balance)
    {
        balanceLabel.text = balance;
    }

    public void SetLoginError(string error)
    {
        loginError.text = error;
        loginError.gameObject.SetActive(true);
    }

    public void EnableStartButton()
    {
        startButton.interactable = true;
    }

    #region Button Handlers

    public void Login()
    {
        PhantasmaDemo.Instance.GetAccount(addressInputField.text);
    }

    public void StartGame()
    {
        addressLabel.gameObject.SetActive(false);
        buttons.SetActive(false);
        
        market.SetActive(true);
    }

    public void BackMarket()
    {
        addressLabel.gameObject.SetActive(true);
        buttons.SetActive(true);

        market.SetActive(false);
    }

    public void RegisterName()
    {
        PhantasmaDemo.Instance.RegisterName();
    }

    public void CrossChainTransfer()
    {
        PhantasmaDemo.Instance.CrossChainTransfer();
    }

    public void ListTransactions()
    {
        PhantasmaDemo.Instance.ListTransactions();
    }

    public void Logout()
    {
        PhantasmaDemo.Instance.LogOut();
    }

    #endregion
}
