using System;
using Phantasma.Core.Types;
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

        carName.text    = car.MutableData.name;
        speedStats.text = "Speed: " + car.MutableData.speed;
        powerStats.text = "Power: " + car.MutableData.power;

        sprite.sprite = car.Image;
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

        Debug.Log("Sell Price:" + priceInput.text);

        //var price = decimal.Parse(priceInput.text);
        BigInteger price;
        if (BigInteger.TryParse(priceInput.text, out price))
        {
            if (price < 0)
            {
                SetSellError("Error: Price must be a decimal positive value.");
                return;
            }

            PhantasmaDemo.Instance.market.SellAsset(_car, PhantasmaDemo.Instance.Key.Address, price, Timestamp.Now + TimeSpan.FromDays(30));
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
