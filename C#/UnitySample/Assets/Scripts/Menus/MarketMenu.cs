using UnityEngine;
using UnityEngine.UI;

public class MarketMenu : MonoBehaviour
{
    public AssetSlot    assetSlot;
    public GameObject   buyMarketContent, sellMarketContent;
    public Button       buyButton, sellButton;
    public GameObject   buyTab, sellTab;

    void OnEnable()
    {
        SelectMarketBuyTab();
    }

    public void UpdateMarket()
    {
        for (var i = 0; i < PhantasmaDemo.Instance.market.MarketBuyAssets.Count; i++)
        {
            var marketAsset = PhantasmaDemo.Instance.market.MarketBuyAssets[i];

            var newSlot                     = Instantiate(assetSlot, buyMarketContent.transform, false);
            newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
            newSlot.SetSlot(marketAsset, EASSET_TYPE.BUY_MARKET_ASSET);
            newSlot.gameObject.SetActive(true);
        }
    }

    public void SelectMarketBuyTab()
    {
        buyButton.interactable = false;
        sellButton.interactable = true;

        buyTab.SetActive(true);
        sellTab.SetActive(false);
    }

    public void SelectMarketSellTab()
    {
        sellButton.interactable = false;
        buyButton.interactable = true;

        sellTab.SetActive(true);
        buyTab.SetActive(false);
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

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMarket();
    }
}
