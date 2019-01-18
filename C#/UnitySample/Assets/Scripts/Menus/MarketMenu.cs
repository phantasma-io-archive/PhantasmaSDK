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
    public Button       buyButton, sellButton, retryRefreshMarket;
    public GameObject   buyTab, sellTab;
    public Text         errorMessage;

    private List<AssetSlot> _buySlots, _sellSlots;

    void Awake()
    {
        _buySlots   = new List<AssetSlot>();
        _sellSlots  = new List<AssetSlot>();
    }

    void OnEnable()
    {
        buyButton.gameObject.SetActive(false);
        sellButton.gameObject.SetActive(false);

        GetMarket();
    }

    public void GetMarket()
    {
        PhantasmaDemo.Instance.market.GetMarket(() =>
        {
            if (_buySlots.Count != PhantasmaDemo.Instance.market.BuyCarAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.BUY);
            }

            if (_sellSlots.Count != PhantasmaDemo.Instance.market.SellCarAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.SELL);
            }

            buyButton.gameObject.SetActive(true);
            sellButton.gameObject.SetActive(true);

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

                var buyAuctionsKeys = new List<string>(PhantasmaDemo.Instance.market.BuyCarAuctions.Keys);
                for (var i = 0; i < buyAuctionsKeys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.BuyCarAuctions[buyAuctionsKeys[i]];

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

                var sellAuctionsKeys = new List<string>(PhantasmaDemo.Instance.market.SellCarAuctions.Keys);
                for (var i = 0; i < sellAuctionsKeys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.SellCarAuctions[sellAuctionsKeys[i]];

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

    public void SetErrorMessage(string error)
    {
        errorMessage.text = error;
        errorMessage.gameObject.SetActive(true);

        retryRefreshMarket.gameObject.SetActive(true);
    }

    public void RetryRefreshClicked()
    {
        GetMarket();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMarket();
    }
}
