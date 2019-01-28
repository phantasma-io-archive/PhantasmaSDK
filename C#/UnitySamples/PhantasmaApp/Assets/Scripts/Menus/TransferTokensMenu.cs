using System.Collections.Generic;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using UnityEngine;
using UnityEngine.UI;

public class TransferTokensMenu : MonoBehaviour
{
    private const string _SELECT_COIN = "SELECT COIN";

    public InputField   addressInput, amountInput;
    public Dropdown     tokenDropdown;

    // Start is called before the first frame update
    void Start()
    {

    }

    void OnEnable()
    {
        PhantasmaDemo.Instance.CheckTokens(() =>
        {
            CanvasManager.Instance.transferTokensMenu.SetContent();
        });
    }

    public void SetContent()
    {
        Clear();

        var tokensList = new List<string>{ _SELECT_COIN };
        tokensList.AddRange(PhantasmaDemo.Instance.PhantasmaTokens.Keys);

        tokenDropdown.ClearOptions();
        tokenDropdown.AddOptions(tokensList);
    }

    public void Clear()
    {
        addressInput.text   = string.Empty;
        amountInput.text    = string.Empty;

        tokenDropdown.value = 0;
    }

    public void SendClicked()
    {
        if (PhantasmaDemo.Instance == null || PhantasmaDemo.Instance.PhantasmaApi == null)
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "It was not possible to process the operation. Please try again.");
            return;
        }

        // Validate private key
            if (string.IsNullOrEmpty(addressInput.text))
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Public address cannot be empty.");
            return;
        }

        Address address;
        // TODO validar if the address is the same

        //if (PhantasmaDemo.Instance != null && PhantasmaDemo.Instance.PhantasmaApi != null && !PhantasmaDemo.Instance.PhantasmaApi.IsValidPrivateKey(addressInput.text))
        if(!PhantasmaDemo.Instance.PhantasmaApi.IsValidAddress(addressInput.text))
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "The entered address is not valid.\nThe address must start with a 'P' and have 45 characters.");
            return;
        }
        else
        {

            address = Address.FromText(addressInput.text);
            // TODO OU?
            //address = KeyPair.FromWIF(addressInput.text).Address; Chave privada

            if (PhantasmaDemo.Instance.Key.Address.Equals(address))
            {
                Clear();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Cannot send tokens to your own address. Destination address must be different from the origin address.");
            }
        }

        // Validate amount
        if (string.IsNullOrEmpty(amountInput.text))
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Amount Key cannot be empty.");
            return;
        }

        //var price = decimal.Parse(priceInput.text);
        BigInteger amount;
        if (BigInteger.TryParse(amountInput.text, out amount))
        {
            if (amount < 0)
            {
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Amount must be a decimal positive value.");
                return;
            }
        }
        else
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Amount must be a decimal positive value.");
        }

        if (tokenDropdown.options[tokenDropdown.value].text.Equals(_SELECT_COIN))
        {
            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "You must select a coin from the dropdown menu.");
            return;
        }

        Debug.Log("Transfer:" + addressInput.text + " | " + amountInput.text);

        PhantasmaDemo.Instance.TransferTokens(PhantasmaDemo.Instance.Key.Address, address, "SOUL", amount);
    }

    public void ClearClicked()
    {
        Clear();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseTransferTokensMenu();
    }

}
