using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MarketMenu : MonoBehaviour
{
    public enum EMARKETPLACE_TYPE
    {
        BUY,
        SELL
    }

    public AssetSlot    assetSlot;
    public GameObject   buyMarketContent, sellMarketContent;
    public Button       buyButton, sellButton;
    public GameObject   buyTab, sellTab;

    private List<AssetSlot> _buySlots, _sellSlots;

    void Awake()
    {
        _buySlots   = new List<AssetSlot>();
        _sellSlots  = new List<AssetSlot>();
    }

    void OnEnable()
    {
        if (_buySlots.Count != PhantasmaDemo.Instance.market.MarketBuyAssets.Count)
        {
            UpdateMarket(EMARKETPLACE_TYPE.BUY);
        }

        SelectMarketBuyTab();
    }

    public void UpdateMarket(EMARKETPLACE_TYPE marketPlace)
    {
        switch (marketPlace)
        {
            case EMARKETPLACE_TYPE.BUY:

                // TODO optimized this to use pool slots
                foreach (var buySlot in _buySlots)
                {
                    DestroyImmediate(buySlot.gameObject);
                }

                _buySlots.Clear();

                for (var i = 0; i < PhantasmaDemo.Instance.market.MarketBuyAssets.Count; i++)
                {
                    var marketAsset = PhantasmaDemo.Instance.market.MarketBuyAssets[i];

                    var newSlot                     = Instantiate(assetSlot, buyMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
                    newSlot.SetSlot(marketAsset, EASSET_TYPE.BUY_MARKET_ASSET);
                    newSlot.gameObject.SetActive(true);

                    _buySlots.Add(newSlot);
                }

                break;

            case EMARKETPLACE_TYPE.SELL:

                // TODO optimized this to use pool slots
                foreach (var sellSlot in _sellSlots)
                {
                    DestroyImmediate(sellSlot.gameObject);
                }

                _sellSlots.Clear();

                for (var i = 0; i < PhantasmaDemo.Instance.market.MarketSellAssets.Count; i++)
                {
                    var marketAsset = PhantasmaDemo.Instance.market.MarketSellAssets[i];

                    var newSlot                     = Instantiate(assetSlot, sellMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
                    newSlot.SetSlot(marketAsset, EASSET_TYPE.BUY_MARKET_ASSET);
                    newSlot.gameObject.SetActive(true);

                    _sellSlots.Add(newSlot);
                }

                break;
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
