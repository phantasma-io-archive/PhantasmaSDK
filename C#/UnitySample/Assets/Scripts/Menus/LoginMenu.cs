using System.Linq.Expressions;
using UnityEngine;
using UnityEngine.UI;

public class LoginMenu : MonoBehaviour
{
    public InputField   addressInputField;

    // TODO adicionar botão para gerar novas chaves privadas

    public void Login()
    {
        if (string.IsNullOrEmpty(addressInputField.text))
        {
            CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, "Private Key cannot be empty.");
            return;
        }

        if (PhantasmaDemo.Instance != null)
        {
            if (PhantasmaDemo.Instance.PhantasmaApi != null && !PhantasmaDemo.Instance.PhantasmaApi.IsValidPrivateKey(addressInputField.text))
            {
                CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL, "Error: The entered private key is not valid.\nThe private key must start with an 'L' or a 'K' and have 52 characters.");
                return;
            }

            PhantasmaDemo.Instance.Login(addressInputField.text);
        }
    }

    public void GeneratePrivateKeyClicked()
    {
        PhantasmaDemo.Instance.GenerateNewKey((newKey) => CanvasManager.Instance.ShowNewKeyPopup(newKey));
    }
}
