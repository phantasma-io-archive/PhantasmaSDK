using System.Collections.Generic;
using Phantasma.Cryptography;
using Phantasma.IO;
using Phantasma.Numerics;
using Phantasma.SDK;
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
        message.gameObject.SetActive(false);
        buyTabMessage.gameObject.SetActive(false);
        sellTabMessage.gameObject.SetActive(false);

        buyButton.gameObject.SetActive(false);
        sellButton.gameObject.SetActive(false);

        GetMarket();
    }

    public void GetMarket()
    {
        PhantasmaDemo.Instance.market.GetMarket(SetContent, () =>
        {
            ShowRefreshButton();
        });
    }

    public void SetContent(Auction[] auctions)
    {
        if (auctions.Length == 0)
        {
            CanvasManager.Instance.HideOperationPopup();
            ShowRefreshButton("There are no auctions on the blockchain market.");
            return;
        }


        message.gameObject.SetActive(false);
        refreshButton.gameObject.SetActive(false);

        if (_buySlots.Count != PhantasmaDemo.Instance.market.BuyCarAuctions.Keys.Count)
        {
            UpdateMarket(EMARKETPLACE_TYPE.BUY);
        }

        buyTabMessage.text = _buySlots.Count == 0 ? "There are no assets to buy on the market." : string.Empty;
        buyTabMessage.gameObject.SetActive(_buySlots.Count == 0);

        if (_sellSlots.Count != PhantasmaDemo.Instance.market.SellCarAuctions.Keys.Count)
        {
            UpdateMarket(EMARKETPLACE_TYPE.SELL);
        }

        sellTabMessage.text = _sellSlots.Count == 0 ? "You don't have any assets for sale on the market." : string.Empty;
        sellTabMessage.gameObject.SetActive(_sellSlots.Count == 0);

        buyButton.gameObject.SetActive(true);
        sellButton.gameObject.SetActive(true);

        SelectMarketBuyTab();
    }

    public void UpdateMarket(EMARKETPLACE_TYPE marketPlace)
    {
        switch (marketPlace)
        {
            case EMARKETPLACE_TYPE.BUY:

                ClearBuyMarket();

                var buyAuctionsKeys = new List<string>(PhantasmaDemo.Instance.market.BuyCarAuctions.Keys);
                for (var i = 0; i < buyAuctionsKeys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.BuyCarAuctions[buyAuctionsKeys[i]];

                    var newSlot                     = Instantiate(assetSlot, buyMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
                    
                    StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTokenData(PhantasmaDemo.TOKEN_SYMBOL, marketAuction.tokenID,
                        (tokenData =>
                        {
                            var ramBytes        = Base16.Decode(tokenData.ram);
                            var carMutableData  = Serialization.Unserialize<CarMutableData>(ramBytes);

                            var romBytes    = Base16.Decode(tokenData.rom);
                            var carData     = Serialization.Unserialize<CarData>(romBytes);

                            var newCar = new Car();
                            newCar.SetCar(Address.FromText(tokenData.ownerAddress), marketAuction.tokenID, carData, carMutableData);

                            newSlot.SetSlot(EASSET_TYPE.BUY_MARKET_ASSET, newCar);
                        }),
                        (type, s) =>
                        {
                            newSlot.DataNotAvailable();
                        }));

                    newSlot.gameObject.SetActive(true);

                    _buySlots.Add(newSlot);
                }

                break;

            case EMARKETPLACE_TYPE.SELL:

                ClearSellMarket();

                var sellAuctionsKeys = new List<string>(PhantasmaDemo.Instance.market.SellCarAuctions.Keys);
                for (var i = 0; i < sellAuctionsKeys.Count; i++)
                {
                    var marketAuction = PhantasmaDemo.Instance.market.SellCarAuctions[sellAuctionsKeys[i]];

                    var newSlot                     = Instantiate(assetSlot, sellMarketContent.transform, false);
                    newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;

                    StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTokenData(PhantasmaDemo.TOKEN_SYMBOL, marketAuction.tokenID,
                        (tokenData =>
                        {
                            var ramBytes        = Base16.Decode(tokenData.ram);
                            var carMutableData  = Serialization.Unserialize<CarMutableData>(ramBytes);

                            var romBytes    = Base16.Decode(tokenData.rom);
                            var carData     = Serialization.Unserialize<CarData>(romBytes);

                            var newCar = new Car();
                            newCar.SetCar(Address.FromText(tokenData.ownerAddress), marketAuction.tokenID, carData, carMutableData);

                            newSlot.SetSlot(EASSET_TYPE.SELL_MARKET_ASSET, newCar);
                        }),
                        (type, s) =>
                        {
                            newSlot.DataNotAvailable();
                        }));

                    newSlot.gameObject.SetActive(true);

                    _sellSlots.Add(newSlot);
                }

                break;
        }
    }

    public void ClearContent()
    {
        ClearBuyMarket();
        ClearSellMarket();
    }

    private void ClearBuyMarket()
    {
        foreach (var buySlot in _buySlots)
        {
            DestroyImmediate(buySlot.gameObject);
        }

        _buySlots.Clear();
    }

    private void ClearSellMarket()
    {
        foreach (var sellSlot in _sellSlots)
        {
            DestroyImmediate(sellSlot.gameObject);
        }

        _sellSlots.Clear();
    }

    public void SelectMarketBuyTab()
    {
        buyButton.interactable  = false;
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

        buyButton.gameObject.SetActive(false);
        sellButton.gameObject.SetActive(false);
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
