using System.Collections.Generic;
using UnityEngine;

public class MarketMenu : MonoBehaviour
{
    public AssetSlot assetSlot;
    public GameObject buyMarketContent, sellMarketContent;
    
    private List<MyGameAsset> _marketBuyAssets, _marketSellAssets;

    private void Awake()
    {
        _marketBuyAssets    = new List<MyGameAsset>();
        _marketSellAssets   = new List<MyGameAsset>();
    }

    // Start is called before the first frame update
    void Start()
    {
        FillMarket();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void FillMarket()
    {
        // TODO get assets from the blockchain

        _marketBuyAssets = new List<MyGameAsset>();

        for (var i = 0; i < PhantasmaDemo.Instance.carImages.Count; i++)
        {
            var carImage = PhantasmaDemo.Instance.carImages[i];

            var newCar = new MyGameAsset();
            newCar.SetAsset(carImage.name, (decimal)Random.Range(0f, 10f), carImage);

            var newSlot = Instantiate(assetSlot, buyMarketContent.transform, false);
            newSlot.SetSlot(newCar, EASSET_TYPE.BUY_MARKET_ASSET);
            newSlot.gameObject.SetActive(true);

            newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;

            _marketBuyAssets.Add(newCar);
        }
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    /// <param name="assetSlot"></param>
    public void BuyAsset(AssetSlot assetSlot)
    {

    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    /// <param name="asset"></param>
    public void AssetForSale(MyGameAsset asset)
    {

    }

    /// <summary>
    /// Sell an asset from the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void SellAsset(AssetSlot assetSlot)
    {

    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void RemoveAsset(AssetSlot assetSlot)
    {

    }
}
