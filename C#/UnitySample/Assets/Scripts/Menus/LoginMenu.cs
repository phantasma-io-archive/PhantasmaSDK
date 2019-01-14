using Phantasma.Cryptography;
using UnityEngine;
using UnityEngine.UI;

public class LoginMenu : MonoBehaviour
{
    public InputField   addressInputField;
    public Text         loginError;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnEnable()
    {
        ClearContent();
    }

    public void Login()
    {
        if (string.IsNullOrEmpty(addressInputField.text))
        {
            SetLoginError("Address cannot be empty.");
            return;
        }

        if (!PhantasmaDemo.Instance.PhantasmaApi.IsValidPrivateKey(addressInputField.text))
        {
            SetLoginError("The typed address is not a valid address.\nThe private key must start with an 'L' or a 'K' and have 52 characaters.");
            return;
        }

        PhantasmaDemo.Instance.Login(addressInputField.text);
    }

    public void SetLoginError(string error)
    {
        loginError.gameObject.SetActive(true);
        loginError.text = error;
    }

    private void ClearContent()
    {
        addressInputField.text  = string.Empty;
        loginError.text         = string.Empty;
    }
}
