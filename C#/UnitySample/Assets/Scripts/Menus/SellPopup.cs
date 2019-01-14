using System;
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

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
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
        if (priceInput.characterValidation == InputField.CharacterValidation.Decimal)
        {
            //var price = decimal.Parse(priceInput.text);
            if (BigInteger.TryParse(priceInput.text, out BigInteger price))
            {
                PhantasmaDemo.Instance.market.SellAsset(_car, PhantasmaDemo.Instance.Key.Address, price, price);
            }
            else
            {
                inputError.text = "1 - Price must be a decimal positive value.";
            }
        }
        else
        {
            inputError.text = "2 - Price must be a decimal positive value.";
        }


    }
}
