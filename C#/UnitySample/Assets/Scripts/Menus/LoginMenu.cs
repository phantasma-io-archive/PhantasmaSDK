using UnityEngine;
using UnityEngine.UI;

public class LoginMenu : MonoBehaviour
{
    public InputField   addressInputField;
    public Text         loginError;

    void OnEnable()
    {
        ClearContent();
    }

    public void Login()
    {
        if (string.IsNullOrEmpty(addressInputField.text))
        {
            ShowError("Private Key cannot be empty.");
            return;
        }

        if (PhantasmaDemo.Instance != null)
        {
            if (PhantasmaDemo.Instance.PhantasmaApi != null && !PhantasmaDemo.Instance.PhantasmaApi.IsValidPrivateKey(addressInputField.text))
            {
                ShowError("Error: The entered private key is not valid.\nThe private key must start with an 'L' or a 'K' and have 52 characters.");
                return;
            }

            PhantasmaDemo.Instance.Login(addressInputField.text);
        }
    }

    public void ShowError(string error)
    {
        // TODO change to use show error popup

        loginError.gameObject.SetActive(true);
        loginError.text = error;
    }

    private void ClearContent()
    {
        addressInputField.text  = string.Empty;
        loginError.text         = string.Empty;
    }
}
