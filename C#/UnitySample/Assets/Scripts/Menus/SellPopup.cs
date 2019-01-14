using Phantasma.Numerics;
using UnityEngine;
using UnityEngine.UI;

public class SellPopup : MonoBehaviour
{
    public Text         carName;
    public Text         speedStats, powerStats;
    public Text         inputError;
    public InputField   priceInput;
    public Image        sprite;

    private Car _car;

    void OnEnable()
    {
        ClearContent();
    }

    public void SetPopup(Car car)
    {
        _car = car;

        carName.text    = car.Data.name;
        speedStats.text = "Speed: " + car.Data.speed;
        powerStats.text = "Power: " + car.Data.power;

        sprite.sprite = car.Icon;
    }

    public void Cancel()
    {
        CanvasManager.Instance.HideSellPopup();
    }

    public void Sell()
    {
        if (string.IsNullOrEmpty(priceInput.text))
        {
            SetSellError("Price field cannot be empty.");
            return;
        }

        if (priceInput.characterValidation == InputField.CharacterValidation.Decimal)
        {
            //var price = decimal.Parse(priceInput.text);
            if (BigInteger.TryParse(priceInput.text, out BigInteger price))
            {
                PhantasmaDemo.Instance.market.SellAsset(_car, PhantasmaDemo.Instance.Key.Address, price, price);
            }
            else
            {
                SetSellError("Error: Price must be a decimal positive value.");
            }
        }
        else
        {
            SetSellError("Error: Price must be a decimal positive value.");
        }
    }

    public void SetSellError(string error)
    {
        inputError.gameObject.SetActive(true);
        inputError.text = error;
    }

    private void ClearContent()
    {
        priceInput.text = string.Empty;
        inputError.text = string.Empty;
    }
}
