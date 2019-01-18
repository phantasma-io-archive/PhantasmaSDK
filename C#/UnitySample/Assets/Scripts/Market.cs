using System;
using System.Collections.Generic;
using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Contracts.Native;
using UnityEngine;

using Phantasma.Cryptography;
using Phantasma.IO;
using Phantasma.Numerics;
using Phantasma.SDK;
using Phantasma.VM.Utils;

public class Market : MonoBehaviour
{
    //private Dictionary<BigInteger, CarData> _cars;

    public Dictionary<string, CarAuction>  CarAuctions      { get; private set; }
    public Dictionary<string, CarAuction>  BuyCarAuctions   { get; private set; }
    public Dictionary<string, CarAuction>  SellCarAuctions     { get; private set; }

    //public List<Car> MarketBuyAssets     { get; private set; }
    //public List<Car> MarketSellAssets    { get; private set; }
    
    private void Awake()
    {
        CarAuctions     = new Dictionary<string, CarAuction>();
        BuyCarAuctions  = new Dictionary<string, CarAuction>();
        SellCarAuctions = new Dictionary<string, CarAuction>();

        //MarketBuyAssets     = new List<Car>();
        //MarketSellAssets    = new List<Car>();
    }

    // Start is called before the first frame update
    void Start()
    {

    }

    /// <summary>
    /// Get the market with the current assets
    /// </summary>
    public void GetMarket(Action successCallback = null, Action errorCallback = null)
    {
        //var script = ScriptUtils.BeginScript()
        //    .AllowGas(PhantasmaDemo.Instance.Key.Address, 1, 9999)
        //    .CallContract("token", "GetAuctionsIDs", PhantasmaDemo.Instance.Key.Address, car.AuctionID)
        //    .SpendGas(PhantasmaDemo.Instance.Key.Address)
        //    .EndScript();

        PhantasmaDemo.Instance.PhantasmaApi.GetAuctions(
            (auctions) =>
            {
                foreach (var auction in auctions)
                {
                    var carAuction = new CarAuction
                    {
                        tokenID         = auction.TokenID,
                        marketAuction   = auction,

                    };

                    CarAuctions.Add(auction.TokenID, carAuction);

                    if (auction.creatorAddress.Equals(PhantasmaDemo.Instance.Key.Address.ToString()))
                    {
                        // Auction has my address -> Selling Auction
                        SellCarAuctions.Add(auction.TokenID, carAuction);
                    }
                    else
                    {
                        // No my auction -> Buying Auction
                        BuyCarAuctions.Add(auction.TokenID, carAuction);
                    }
                }
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            });

    }



    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        var script = ScriptUtils.BeginScript()
            .AllowGas(car.OwnerAddress, 1, 9999)
            .CallContract("market", "BuyToken", car.OwnerAddress, car.TokenID)
            .SpendGas(car.OwnerAddress)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    //var carData = car.Data;
                    //car.Data    = carData;

                    var carMutableData      = car.MutableData;
                    carMutableData.location = CarLocation.None;

                    car.OwnerAddress    = PhantasmaDemo.Instance.Key.Address;
                    car.MutableData     = carMutableData;
                  
                    //var auction = GetAuction(car.AuctionID);

                    CarAuctions.Remove(car.TokenID);
                    BuyCarAuctions.Remove(car.TokenID);

                    PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);
                    //CanvasManager.Instance.myAssetsMenu.UpdateMyAssets(); // Talvez não seja necessário isto

                    CanvasManager.Instance.HideSellPopup();

                    CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);
                }));

            },
            (errorType, errorMessage) =>
            {
                // TODO
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    public void SellAsset(Car car, Address from, BigInteger price, AuctionCurrency currency = AuctionCurrency.Game_Coin, uint duration = 30)
    {
        var script = ScriptUtils.BeginScript()
            .AllowGas(from, 1, 9999)
            .CallContract("market", "SellToken", from, PhantasmaDemo.TOKEN_SYMBOL, car.TokenID, price)
            .SpendGas(from)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    foreach (var evt in tx.events)
                    {
                        if (Enum.TryParse(evt.kind, out EventKind eKind))
                        {
                            if (eKind == EventKind.AuctionCreated)
                            {
                                var bytes           = Base16.Decode(evt.data);
                                var marketEventData = Serialization.Unserialize<MarketEventData>(bytes);

                                Debug.Log(evt.kind + " - " + marketEventData.ID);

                                //StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetAuctions()Transaction(result, (tx) => { }, () => { }));

                                //var newAuction = new Auction
                                //{
                                //    auctionID       = marketEventData.ID,
                                //    marketAuction   = marketEventData..SellAuctions.Add()
                                //};

                                //Auctions.Add(auctionID, auction);

                                //var carData = car.Data;
                                //car.Data    = carData;

                                var carMutableData      = car.MutableData;
                                carMutableData.location = CarLocation.Market;
                                car.MutableData         = carMutableData;
                                
                                PhantasmaDemo.Instance.MyCars.Remove(car.TokenID);
                                CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();

                                CanvasManager.Instance.HideSellPopup();
                                break;
                            }
                            else
                            {
                                CanvasManager.Instance.HideSellPopup();
                                CanvasManager.Instance.HideFetchingDataPopup();
                                
                                // TODO aconteceu algum erro... mostrar erro
                            }
                        }
                        else
                        {
                            CanvasManager.Instance.HideSellPopup();
                            CanvasManager.Instance.HideFetchingDataPopup();
                            // TODO aconteceu algum erro..
                        }
                    }
                }));

            },
            (errorType, errorMessage) =>
            {
                // TODO
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset(Car car)
    {
        // TODO é um buy na prática (address do buyer == address do owner => free sale on blockchain? )

        //_cars.Remove(car.CarID);
        //MarketSellAssets.Remove(car);

        PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }

    public CarAuction GetCarAuction(string tokenID)
    {
        if (CarAuctions.ContainsKey(tokenID))
        {
            return CarAuctions[tokenID];
        }

        var nullAuction = new CarAuction
        { 
            tokenID = string.Empty
        };

        return nullAuction;
    }
}
