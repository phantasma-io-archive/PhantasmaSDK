using UnityEngine;
using UnityEngine.UI;

public class SellPopup : MonoBehaviour
{
    public Text         carName;
    public Text         speedStats, powerStats;
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
        //TODO validate price input field

        var price = 2; // TODO priceInput.text to BigInteger

        PhantasmaDemo.Instance.market.SellAsset(_car, PhantasmaDemo.Instance.Key.Address, price, price);
    }
}
