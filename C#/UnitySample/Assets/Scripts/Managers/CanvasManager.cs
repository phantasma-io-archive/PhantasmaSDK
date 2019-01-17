using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

public class CanvasManager : MonoBehaviour {

    public Text                 addressLabel, errorMessage;
    public MyAssetsMenu         myAssetsMenu;
    public LoginMenu            loginMenu;
    public MainMenu             mainMenu;
    public AccountMenu          accountMenu;
    public MarketMenu           marketMenu;
    public AdminMenu            adminMenu;
    public BuyPopup             buyPopup;
    public SellPopup            sellPopup;
    public FetchingDataPopup    fetchingDataPopup;

    private static CanvasManager _instance;
    public static CanvasManager Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(CanvasManager)) as CanvasManager : _instance; return _instance; }
    }

    // Use this for initialization
    void Start () {

        loginMenu.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(false);
        accountMenu.gameObject.SetActive(false);
        myAssetsMenu.gameObject.SetActive(false);
        marketMenu.gameObject.SetActive(false);
        adminMenu.gameObject.SetActive(false);
        
        //OpenLogin();
    }
	
    public void SetAddress(string address)
    {
        addressLabel.text = address;
    }

    public void ClearAddress()
    {
        addressLabel.text = string.Empty;
    }

    public void SetErrorMessage(string error)
    {
        errorMessage.gameObject.SetActive(true);
        errorMessage.text = error;
    }

    #region Login Menu

    public void OpenLogin()
    {
        mainMenu.gameObject.SetActive(false);

        loginMenu.gameObject.SetActive(true);
    }

    public void CloseLogin()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.gameObject.SetActive(true);

        loginMenu.gameObject.SetActive(false);
    }

    #endregion

    #region Account Menu

    public void OpenAccount()
    {
        mainMenu.gameObject.SetActive(false);

        accountMenu.gameObject.SetActive(true);
    }

    public void CloseAccount()
    {
        mainMenu.gameObject.SetActive(true);

        accountMenu.gameObject.SetActive(false);
    }

    #endregion

    #region My Assets Menu

    public void OpenMyAssets()
    {
        addressLabel.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(false);

        myAssetsMenu.gameObject.SetActive(true);
    }

    public void CloseMyAssets()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.gameObject.SetActive(true);

        myAssetsMenu.gameObject.SetActive(false);
    }

    #endregion
    
    #region Market Menu

    public void OpenMarket()
    {
        addressLabel.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(false);

        marketMenu.gameObject.SetActive(true);
    }

    public void CloseMarket()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.gameObject.SetActive(true);

        marketMenu.gameObject.SetActive(false);
    }

    #endregion

    #region Admin Menu

    public void OpenAdmin()
    {
        mainMenu.gameObject.SetActive(false);
        addressLabel.gameObject.SetActive(false);

        adminMenu.gameObject.SetActive(true);
    }

    public void CloseAdmin()
    {
        addressLabel.gameObject.SetActive(true);
        mainMenu.gameObject.SetActive(true);

        adminMenu.gameObject.SetActive(false);
    }

    #endregion

    #region Popups

    public void ShowFetchingDataPopup(string message = null)
    {
        fetchingDataPopup.ShowPopup(message);
    }

    public void HideFetchingDataPopup()
    {
        fetchingDataPopup.HidePopup();
    }

    public void ShowSellPopup(Car car)
    {
        sellPopup.SetPopup(car);
        sellPopup.gameObject.SetActive(true);
    }

    public void HideSellPopup()
    {
        sellPopup.gameObject.SetActive(false);
    }

    public void ShowBuyPopup(Car car)
    {
        buyPopup.SetPopup(car);
        buyPopup.gameObject.SetActive(true);
    }

    public void HideBuyPopup()
    {
        buyPopup.gameObject.SetActive(false);
    }

    #endregion

    //public void RegisterName()
    //{
    //    PhantasmaDemo.Instance.RegisterName();
    //}

    //public void ListTransactions()
    //{
    //    PhantasmaDemo.Instance.ListTransactions();
    //}

}
