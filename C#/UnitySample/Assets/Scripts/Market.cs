using System;
using System.Collections.Generic;
using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Contracts.Native;
using Phantasma.Core.Types;
using UnityEngine;

using Phantasma.Cryptography;
using Phantasma.IO;
using Phantasma.Numerics;
using Phantasma.SDK;
using Phantasma.VM.Utils;

public class Market : MonoBehaviour
{
    public Dictionary<string, CarAuction>  CarAuctions      { get; private set; }
    public Dictionary<string, CarAuction>  BuyCarAuctions   { get; private set; }
    public Dictionary<string, CarAuction>  SellCarAuctions     { get; private set; }
    
    private void Awake()
    {
        CarAuctions     = new Dictionary<string, CarAuction>();
        BuyCarAuctions  = new Dictionary<string, CarAuction>();
        SellCarAuctions = new Dictionary<string, CarAuction>();
    }

    // Start is called before the first frame update
    void Start()
    {

    }

    /// <summary>
    /// Get the market in Phantasma Blockchain with the current assets
    /// </summary>
    public void GetMarket(Action successCallback = null, Action errorCallback = null)
    {
        CanvasManager.Instance.ShowFetchingDataPopup("Refreshing blockchain asset market...");

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetAuctions(
            (auctions) =>
            {
                if (auctions.Length == 0)
                {
                    CanvasManager.Instance.HideFetchingDataPopup();
                    CanvasManager.Instance.marketMenu.ShowError("There are no auctions on the blockchain market.");
                    return;
                }

                foreach (var auction in auctions)
                {
                    var carAuction = new CarAuction
                    {
                        tokenID = auction.tokenId,
                        auction = auction,

                    };

                    CarAuctions.Add(auction.tokenId, carAuction);

                    if (auction.creatorAddress.Equals(PhantasmaDemo.Instance.Key.Address.ToString()))
                    {
                        // Auction has my address -> Selling Auction
                        SellCarAuctions.Add(auction.tokenId, carAuction);
                    }
                    else
                    {
                        // No my auction -> Buying Auction
                        BuyCarAuctions.Add(auction.tokenId, carAuction);
                    }
                }

                CanvasManager.Instance.HideFetchingDataPopup();
            },
            (errorType, errorMessage) =>
            {
                // TODO se este método for chamado de outro menu, então o erro deve ir para esse menu
                CanvasManager.Instance.marketMenu.ShowError(errorType + " - " + errorMessage, true);
                CanvasManager.Instance.HideFetchingDataPopup();
            }));
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        CanvasManager.Instance.ShowFetchingDataPopup("Purchasing an asset from the blockchain asset market...");

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
                    var succeed = false;

                    foreach (var evt in tx.events)
                    {
                        EventKind eKind;
                        if (Enum.TryParse(evt.kind, out eKind))
                        {
                            if (eKind == EventKind.AuctionFilled)
                            {
                                var bytes           = Base16.Decode(evt.data);
                                var marketEventData = Serialization.Unserialize<MarketEventData>(bytes);

                                Debug.Log(evt.kind + " - " + marketEventData.ID);

                                ////////////////

                                //var carData = car.Data;
                                //car.Data    = carData;

                                var carMutableData      = car.MutableData;
                                carMutableData.location = CarLocation.None;

                                car.OwnerAddress = PhantasmaDemo.Instance.Key.Address;
                                car.MutableData = carMutableData;
                                
                                CarAuctions.Remove(car.TokenID);
                                BuyCarAuctions.Remove(car.TokenID);

                                PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);
                                //CanvasManager.Instance.myAssetsMenu.UpdateMyAssets(); // Talvez não seja necessário isto

                                CanvasManager.Instance.HideBuyPopup();

                                CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);

                                succeed = true;
                                break;
                            }
                            else
                            {
                                // TODO remove but check call hide sell popup
                                CanvasManager.Instance.HideFetchingDataPopup();
                                CanvasManager.Instance.HideSellPopup();
                                CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (02). Please try again.");
                            }

                        }
                        else
                        {
                            // TODO remove but check call hide sell popup
                            CanvasManager.Instance.HideFetchingDataPopup();
                            CanvasManager.Instance.HideSellPopup();
                            CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (01). Please try again.");
                        }

                    }

                    if (!succeed)
                    {
                        CanvasManager.Instance.HideFetchingDataPopup();
                        CanvasManager.Instance.HideBuyPopup();
                        CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (1). Please try again.");
                    }
                    
                }));

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.marketMenu.ShowError(errorType + " - " + errorMessage);
                CanvasManager.Instance.HideFetchingDataPopup();
            }
        ));
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    public void SellAsset(Car car, Address from, BigInteger price, Timestamp endDate, AuctionCurrency currency = AuctionCurrency.Game_Coin)
    {
        CanvasManager.Instance.ShowFetchingDataPopup("Puting an asset for sale on the blockchain asset market...");

        var script = ScriptUtils.BeginScript()
            .AllowGas(from, 1, 9999)
            .CallContract("market", "SellToken", from, PhantasmaDemo.TOKEN_SYMBOL, car.TokenID, price, endDate)
            .SpendGas(from)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    var succeed = false;

                    foreach (var evt in tx.events)
                    {
                        EventKind eKind;
                        if (Enum.TryParse(evt.kind, out eKind))
                        {
                            if (eKind == EventKind.AuctionCreated)
                            {
                                var bytes           = Base16.Decode(evt.data);
                                var marketEventData = Serialization.Unserialize<MarketEventData>(bytes);

                                Debug.Log(evt.kind + " - " + marketEventData.ID);

                                var newAuction = new Auction
                                {
                                    creatorAddress  = PhantasmaDemo.Instance.Key.Address.ToString(),
                                    endDate         = endDate.Value,
                                    symbol          = marketEventData.Symbol,
                                    tokenId         = marketEventData.ID.ToString(),
                                    price           = price.ToString()
                                };

                                var newCarAuction = new CarAuction
                                {
                                    tokenID = car.TokenID,
                                    auction = newAuction
                                };

                                CarAuctions.Add(car.TokenID, newCarAuction);

                                //var carData = car.Data;
                                //car.Data = carData;

                                var carMutableData      = car.MutableData;
                                carMutableData.location = CarLocation.Market;
                                car.MutableData         = carMutableData;

                                PhantasmaDemo.Instance.MyCars.Remove(car.TokenID);
                                CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();

                                CanvasManager.Instance.HideSellPopup();

                                succeed = true;
                                break;
                            }
                            else
                            {
                                // TODO remove but check call hide sell popup
                                CanvasManager.Instance.HideFetchingDataPopup();
                                CanvasManager.Instance.HideSellPopup();
                                CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (02). Please try again.");
                            }

                        }
                        else
                        {
                            // TODO remove but check call hide sell popup
                            CanvasManager.Instance.HideFetchingDataPopup();
                            CanvasManager.Instance.HideSellPopup();
                            CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (01). Please try again.");
                        }

                    }

                    if (!succeed)
                    {
                        CanvasManager.Instance.HideFetchingDataPopup();
                        CanvasManager.Instance.HideSellPopup();
                        CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (1). Please try again.");
                    }

                }));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideFetchingDataPopup();
                CanvasManager.Instance.HideSellPopup();
                CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (2). Please try again.");
            }
        ));
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset(Car car)
    {
        CanvasManager.Instance.ShowFetchingDataPopup("Removing an asset from the blockchain market...");

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
                    var succeed = false;

                    foreach (var evt in tx.events)
                    {
                        EventKind eKind;
                        if (Enum.TryParse(evt.kind, out eKind))
                        {
                            if (eKind == EventKind.AuctionCancelled)
                            {
                                var bytes           = Base16.Decode(evt.data);
                                var marketEventData = Serialization.Unserialize<MarketEventData>(bytes);

                                Debug.Log(evt.kind + " - " + marketEventData.ID);

                                ////////////////

                                //var carData = car.Data;
                                //car.Data    = carData;

                                var carMutableData = car.MutableData;
                                carMutableData.location = CarLocation.None;

                                car.OwnerAddress    = PhantasmaDemo.Instance.Key.Address;
                                car.MutableData     = carMutableData;

                                CarAuctions.Remove(car.TokenID);
                                SellCarAuctions.Remove(car.TokenID);

                                PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);
                                //CanvasManager.Instance.myAssetsMenu.UpdateMyAssets(); // Talvez não seja necessário isto

                                CanvasManager.Instance.HideRemovePopup();

                                CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);

                                succeed = true;
                                break;
                            }
                            else
                            {
                                // TODO remove but check call hide sell popup
                                CanvasManager.Instance.HideFetchingDataPopup();
                                CanvasManager.Instance.HideSellPopup();
                                CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (02). Please try again.");
                            }

                        }
                        else
                        {
                            // TODO remove but check call hide sell popup
                            CanvasManager.Instance.HideFetchingDataPopup();
                            CanvasManager.Instance.HideSellPopup();
                            CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (01). Please try again.");
                        }

                    }

                    if (!succeed)
                    {
                        CanvasManager.Instance.HideFetchingDataPopup();
                        CanvasManager.Instance.HideRemovePopup();
                        CanvasManager.Instance.myAssetsMenu.ShowError("Something failed on the connection to the blockchain (1). Please try again.");
                    }

                }));

            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.marketMenu.ShowError(errorType + " - " + errorMessage);
                CanvasManager.Instance.HideFetchingDataPopup();
            }
        ));
    }

    /// <summary>
    /// Get the auction from Phantasma Blockchain relative to a specific token
    /// </summary>
    /// <param name="tokenID">ID of the token</param>
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
