using UnityEngine;
using UnityEngine.UI;

public class RemovePopup : MonoBehaviour
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

        var carAuction = PhantasmaDemo.Instance.market.GetCarAuction(car.TokenID);

        cost.text = "Cost: " + carAuction.auction.price;

        sprite.sprite = car.Image;
    }

    public void Cancel()
    {
        CanvasManager.Instance.HideRemovePopup();
    }

    public void Remove()
    {
        PhantasmaDemo.Instance.market.RemoveAsset(_car);
    }
}
