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
    public Button       buyButton, sellButton, refreshButton;
    public GameObject   buyTab, sellTab;
    public Text         message, buyTabMessage, sellTabMessage;

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
        PhantasmaDemo.Instance.market.GetMarket((auctions) =>
        {
            if (auctions.Length == 0)
            {
                CanvasManager.Instance.HideOperationPopup();
                ShowRefreshButton("There are no auctions on the blockchain market.");
                return;
            }

            Debug.Log("buy slots: " + _buySlots.Count + " | market buy: " + PhantasmaDemo.Instance.market.BuyCarAuctions.Keys.Count);
            
            if (_buySlots.Count != PhantasmaDemo.Instance.market.BuyCarAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.BUY);
            }

            buyTabMessage.text = _buySlots.Count == 0 ? "There are no assets to buy on the market." : string.Empty;
            buyTabMessage.gameObject.SetActive(_buySlots.Count == 0);

            Debug.Log("sell slots: " + _sellSlots.Count + " | market sell: " + PhantasmaDemo.Instance.market.SellCarAuctions.Keys.Count);
            if (_sellSlots.Count != PhantasmaDemo.Instance.market.SellCarAuctions.Keys.Count)
            {
                UpdateMarket(EMARKETPLACE_TYPE.SELL);
            }

            sellTabMessage.text = _buySlots.Count == 0 ? "You don't have any assets for sale on the market." : string.Empty;
            sellTabMessage.gameObject.SetActive(_sellSlots.Count == 0);

            buyButton.gameObject.SetActive(true);
            sellButton.gameObject.SetActive(true);

            SelectMarketBuyTab();

        }, () =>
        {
            //CanvasManager.Instance.ShowResultPopup(ERESULT_TYPE.FAIL,"Could not fetch blockchain assets market.");
            ShowRefreshButton();
        });
    }

    public void UpdateMarket(EMARKETPLACE_TYPE marketPlace)
    {
        switch (marketPlace)
        {
            case EMARKETPLACE_TYPE.BUY:

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

    public void ShowRefreshButton(string msg = null)
    {
        message.text = msg;
        message.gameObject.SetActive(true);

        refreshButton.gameObject.SetActive(true);
    }

    public void RefreshClicked()
    {
        GetMarket();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMarket();
    }
}
