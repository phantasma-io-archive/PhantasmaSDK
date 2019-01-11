using UnityEngine;
using UnityEngine.UI;

public class BuyPopup : MonoBehaviour
{
    public Text carName;
    public Text speedStats, powerStats;
    public Text cost;

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

        carName.text = car.Data.name;

        speedStats.text = "Speed: " + car.Data.speed;
        powerStats.text = "Power: " + car.Data.power;

        var auction = PhantasmaDemo.Instance.market.GetAuction(car.Data.auctionID);

        cost.text = "Cost: " + auction.startPrice;
    }

    public void Cancel()
    {
        CanvasManager.Instance.HideSellPopup();
    }

    public void Buy()
    {
        PhantasmaDemo.Instance.market.BuyAsset(_car, PhantasmaDemo.Instance.Key.Address, price, price);
    }
}
