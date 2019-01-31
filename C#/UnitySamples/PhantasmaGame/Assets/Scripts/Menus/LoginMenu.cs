using UnityEngine;
using UnityEngine.UI;

public class LoginMenu : MonoBehaviour
{
    public InputField   addressInputField;
    
    public void Login()
    {
        if (string.IsNullOrEmpty(addressInputField.text))
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Private Key cannot be empty.");
            return;
        }

        if (PhantasmaDemo.Instance != null)
        {
            if (PhantasmaDemo.Instance.PhantasmaApi != null && !PhantasmaDemo.Instance.PhantasmaApi.IsValidPrivateKey(addressInputField.text))
            {
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Error: The entered private key is not valid.\nThe private key must start with an 'L' or a 'K' and have 52 characters.");
                return;
            }

            PhantasmaDemo.Instance.Login(addressInputField.text);
        }
    }

    public void GeneratePrivateKeyClicked()
    {
        PhantasmaDemo.Instance.GenerateNewKey((newKey) => CanvasManager.Instance.ShowNewKeyPopup(newKey));
    }

    public void ClearAddress()
    {
        addressInputField.text = string.Empty;
    }
}
