using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

public class CanvasManager : MonoBehaviour {

    public Text         addressLabel, balanceLabel, loginError;
    public GameObject   loginMenu, mainMenu, accountMenu , myAssetsMenu, marketMenu;
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

        loginMenu.SetActive(false);
        mainMenu.SetActive(false);
        accountMenu.SetActive(false);
        myAssetsMenu.SetActive(false);
        marketMenu.SetActive(false);
        
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
        
        loginError.text = string.Empty;
        loginError.gameObject.SetActive(!isVisible);

        addressInputField.text = string.Empty;

        loginMenu.SetActive(isVisible);

        mainMenu.SetActive(!isVisible);
    }

    public void SetAddress(string address)
    {
        addressLabel.text = address;
    }

    public void SetBalance(string balance)
    {
        balanceLabel.text = balance + "\n";
        //Debug.Log("set balance: " + balanceLabel.text);
    }

    public void AddBalanceEntry(string entry)
    {
        balanceLabel.text += entry + "\n";
        //Debug.Log("add entry: " + balanceLabel.text);
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
        PhantasmaDemo.Instance.Login(addressInputField.text);
    }

    public void OpenAccount()
    {
        addressLabel.gameObject.SetActive(false);
        mainMenu.SetActive(false);

        accountMenu.SetActive(true);
    }

    public void CloseAccount()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.SetActive(true);

        accountMenu.SetActive(false);
    }

    public void OpenMyAsset()
    {
        addressLabel.gameObject.SetActive(false);
        mainMenu.SetActive(false);

        myAssetsMenu.SetActive(true);
    }

    public void CloseMyAssets()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.SetActive(true);

        myAssetsMenu.SetActive(false);
    }

    public void OpenMarket()
    {
        addressLabel.gameObject.SetActive(false);
        mainMenu.SetActive(false);

        marketMenu.SetActive(true);
    }

    public void CloseMarket()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.SetActive(true);

        marketMenu.SetActive(false);
    }

    //public void RegisterName()
    //{
    //    PhantasmaDemo.Instance.RegisterName();
    //}


    //public void ListTransactions()
    //{
    //    PhantasmaDemo.Instance.ListTransactions();
    //}

    public void Logout()
    {
        PhantasmaDemo.Instance.LogOut();
    }

    #endregion
}
