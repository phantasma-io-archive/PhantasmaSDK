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
        PhantasmaDemo.Instance.market.GetMarket(() =>
        {
            if (_buySlots.Count != PhantasmaDemo.Instance.market.BuyAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.BUY);
            }

            if (_sellSlots.Count != PhantasmaDemo.Instance.market.SellAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.SELL);
            }

            SelectMarketBuyTab();
        }, () =>
        {
            // TODO erro ao fazer get market => mostrar erro e não abrir as tabs
        });


        
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

                for (var i = 0; i < PhantasmaDemo.Instance.market.BuyAuctions.Keys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.BuyAuctions[i];

                    var newSlot                     = Instantiate(assetSlot, buyMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
                    //newSlot.SetSlot(marketAsset, EASSET_TYPE.BUY_MARKET_ASSET);
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

                for (var i = 0; i < PhantasmaDemo.Instance.market.SellAuctions.Keys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.SellAuctions[i];

                    var newSlot                     = Instantiate(assetSlot, sellMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
                    //newSlot.SetSlot(marketAsset, EASSET_TYPE.SELL_MARKET_ASSET);
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

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMarket();
    }
}
