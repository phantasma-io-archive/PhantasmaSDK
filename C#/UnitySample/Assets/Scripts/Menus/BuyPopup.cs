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

        carName.text = car.Data.name;

        speedStats.text = "Speed: " + car.Data.speed;
        powerStats.text = "Power: " + car.Data.power;

        var auction = PhantasmaDemo.Instance.market.GetAuction(car.AuctionID);

        cost.text = "Cost: " + auction.startPrice;

        sprite.sprite = car.Icon;
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
