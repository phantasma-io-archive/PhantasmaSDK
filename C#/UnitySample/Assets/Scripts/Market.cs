using System.Collections.Generic;
using UnityEngine;

public class Market : MonoBehaviour
{
    public List<MyGameAsset> MarketBuyAssets    { get; private set; }
    public List<MyGameAsset> MarketSellAssets   { get; private set; }

    private void Awake()
    {
        MarketBuyAssets     = new List<MyGameAsset>();
        MarketSellAssets    = new List<MyGameAsset>();
    }

    // Start is called before the first frame update
    void Start()
    {
        FillMarket();
    }
    
    public void FillMarket()
    {
        // TODO get assets from the blockchain
        foreach (var carImage in PhantasmaDemo.Instance.carImages)
        {           
            var newCar = new MyGameAsset();
            newCar.SetAsset(carImage.name, (decimal)Random.Range(0f, 10f), carImage);

            MarketBuyAssets.Add(newCar);
        }
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    /// <param name="assetSlot"></param>
    public void BuyAsset(MyGameAsset asset)
    {
        MarketBuyAssets.Remove(asset);

        PhantasmaDemo.Instance.MyAssets.Add(asset);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void SellAsset(MyGameAsset asset)
    {
        MarketSellAssets.Add(asset);

        PhantasmaDemo.Instance.MyAssets.Remove(asset);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void RemoveAsset(MyGameAsset asset)
    {
        MarketSellAssets.Remove(asset);

        PhantasmaDemo.Instance.MyAssets.Add(asset);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }
}
