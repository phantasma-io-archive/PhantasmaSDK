using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

public class CanvasManager : MonoBehaviour {

    public Text                 addressLabel, errorMessage;
    public Button               retryConnectionButton;

    // Menus
    public LoginMenu            loginMenu;
    public MainMenu             mainMenu;
    public AccountMenu          accountMenu;

    // Popups
    public NewKeyPopup          newKeyPopup;
    public OperationPopup       operationPopup;
    public ResultPopup          resultPopup;
    public CancelOperationPopup cancelOperationPopup;
    
    private static CanvasManager _instance;
    public static CanvasManager Instance
    {
        get { _instance = _instance == null ? FindObjectOfType(typeof(CanvasManager)) as CanvasManager : _instance; return _instance; }
    }

    // Use this for initialization
    void Start () {

        errorMessage.gameObject.SetActive(false);
        retryConnectionButton.gameObject.SetActive(false);

        loginMenu.gameObject.SetActive(false);
        mainMenu.gameObject.SetActive(false);
        accountMenu.gameObject.SetActive(false);
        
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
        errorMessage.text = error;
        errorMessage.gameObject.SetActive(true);

        retryConnectionButton.gameObject.SetActive(true);
    }

    public void RetryConnectionClicked()
    {
        errorMessage.gameObject.SetActive(false);
        retryConnectionButton.gameObject.SetActive(false);

        //PhantasmaDemo.Instance.LoadPhantasmaData();
    }

    #region Login Menu

    public void OpenLogin()
    {
        mainMenu.gameObject.SetActive(false);

        loginMenu.ClearAddress();
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

    #region Popups

    public void ShowNewKeyPopup(string newKey)
    {
        newKeyPopup.ShowPopup(newKey);
    }

    public void HideNewKeyPopup()
    {
        newKeyPopup.HidePopup();
    }

    public void ShowOperationPopup(string message, bool showCancelButton)
    {
        operationPopup.ShowPopup(message, showCancelButton);
    }

    public void HideOperationPopup()
    {
        operationPopup.HidePopup();
    }

    //public void ShowResultPopup(EOPERATION_RESULT type, string message)
    //{
    //    resultPopup.ShowPopup(type, message);
    //}

    public void HideResultPopup()
    {
        resultPopup.HidePopup();
    }

    //public void ShowCancelOperationPopup(EOPERATION_RESULT type, string message)
    //{
    //    cancelOperationPopup.ShowPopup(type, message);
    //}

    public void HideCancelOperationPopup()
    {
        cancelOperationPopup.HidePopup();
    }
    
    #endregion

}
