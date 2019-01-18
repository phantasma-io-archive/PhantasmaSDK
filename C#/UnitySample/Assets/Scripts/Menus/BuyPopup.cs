using UnityEngine;
using UnityEngine.UI;

public class BuyPopup : MonoBehaviour
{
    public Text     carName, cost;
    public Text     speedStats, powerStats;
    public Image    sprite;

    private Car _car;

    public void SetPopup(Car car)
    {
        _car = car;

        carName.text = car.MutableData.name;

        speedStats.text = "Speed: " + car.MutableData.speed;
        powerStats.text = "Power: " + car.MutableData.power;

        var auction = PhantasmaDemo.Instance.market.GetCarAuction(car.TokenID);

        cost.text = "Cost: " + auction.marketAuction.Price;

        sprite.sprite = car.Image;
    }

    public void Cancel()
    {
        CanvasManager.Instance.HideSellPopup();
    }

    public void Buy()
    {
        PhantasmaDemo.Instance.market.BuyAsset(_car);
    }
}
