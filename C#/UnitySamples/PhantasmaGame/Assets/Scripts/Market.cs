using System;
using System.Collections;
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

    /// <summary>
    /// Get the market in Phantasma Blockchain with the current assets
    /// </summary>
    public void GetMarket(Action<Auction[]> successCallback = null, Action errorCallback = null)
    {
        StartCoroutine(GetMarketCoroutine(successCallback, errorCallback));
    }

    private IEnumerator GetMarketCoroutine(Action<Auction[]> successCallback = null, Action errorCallback = null)
    {
        CanvasManager.Instance.ShowOperationPopup("Refreshing blockchain asset market...", false);

        uint itemsPerPage = 20;

        //GetAuctions(string symbol, uint page, uint pageSize, Action<Auction[], int, int> callback, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorHandlingCallback = null)

        yield return PhantasmaDemo.Instance.PhantasmaApi.GetAuctions(PhantasmaDemo.TOKEN_SYMBOL, 1, itemsPerPage,
            (auctions, currentPage, totalPages) =>
            {
                ProcessAuctions(auctions, currentPage, totalPages);
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);

                if (errorCallback != null)
                {
                    errorCallback();
                }
            });

        // todo check if received everything

        //CarAuctions.Clear();
        //SellCarAuctions.Clear();
        //BuyCarAuctions.Clear();
        //foreach (var auction in auctions)
        //{
        //    var carAuction = new CarAuction
        //    {
        //        tokenID = auction.tokenId,
        //        auction = auction
        //    };

        //    CarAuctions.Add(auction.tokenId, carAuction);

        //    if (auction.creatorAddress.Equals(PhantasmaDemo.Instance.Key.Address.ToString()))
        //    {
        //        // Auction has my address -> Selling Auction
        //        SellCarAuctions.Add(auction.tokenId, carAuction);
        //    }
        //    else
        //    {
        //        // No my auction -> Buying Auction
        //        BuyCarAuctions.Add(auction.tokenId, carAuction);
        //    }
        //}

        //CanvasManager.Instance.HideOperationPopup();

        //if (successCallback != null)
        //{
        //    successCallback(auctions);
        //}
    }

    //private IEnumerator ProcessAuctions(Auction auctions, uint currentPage, uint totalPages, Action<Auction[], int,int> successCallback = null, Action<EPHANTASMA_SDK_ERROR_TYPE, string> errorCallback = null)
    private IEnumerator ProcessAuctions(Auction[] auctions, int currentPage, int totalPages)
    {
        if (currentPage < totalPages)
        {
            yield return PhantasmaDemo.Instance.PhantasmaApi.GetAuctions(PhantasmaDemo.TOKEN_SYMBOL, (uint) currentPage + 1, (uint) totalPages,
                (a, cPage, tPages) =>
                {
                    ProcessAuctions(a, cPage, tPages);
                },
                (errorType, errorMessage) =>
                {
                    CanvasManager.Instance.HideOperationPopup();
                    CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
                });
        }
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        CanvasManager.Instance.ShowOperationPopup("Purchasing an asset from the blockchain asset market...", false);

        var script = ScriptUtils.BeginScript()
            .AllowGas(car.OwnerAddress, 1, 9999)
            .CallContract("market", "BuyToken", PhantasmaDemo.Instance.Key.Address, PhantasmaDemo.TOKEN_SYMBOL, car.TokenID)
            .SpendGas(car.OwnerAddress)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                StartCoroutine(CheckAssetPurchase(car, result));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Check if the auction purchase was successful
    /// </summary>
    private IEnumerator CheckAssetPurchase(Car car, string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking auction purchase...", true);

        yield return PhantasmaDemo.Instance.CheckOperation(PhantasmaDemo.EBLOCKCHAIN_OPERATION.BUY_ASSET, result,
            (tx) =>
            {
                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        if (eKind == EventKind.AuctionFilled)
                        {
                            //Debug.Log(evt.kind + " - " + marketEventData.ID);

                            //var carData = car.Data;
                            //car.Data    = carData;

                            var carMutableData = car.MutableData;
                            carMutableData.location = CarLocation.None;

                            car.OwnerAddress = PhantasmaDemo.Instance.Key.Address;
                            car.MutableData = carMutableData;

                            CarAuctions.Remove(car.TokenID);
                            BuyCarAuctions.Remove(car.TokenID);

                            PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);

                            CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);

                            CanvasManager.Instance.HideBuyPopup();
                            CanvasManager.Instance.HideOperationPopup();
                            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.SUCCESS, "Asset purchased from the market with success.");

                            GetMarket((auctions) =>
                            {
                                CanvasManager.Instance.marketMenu.SetContent(auctions);

                                if (CarAuctions.Keys.Count > 0)
                                {
                                    CanvasManager.Instance.marketMenu.SelectMarketBuyTab();
                                }
                            });

                            return;
                        }
                    }
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideBuyPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Something failed while purchasing the asset from the market. Please try again.");
            },
            ((errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideBuyPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }));
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    public void SellAsset(Car car, Address from, BigInteger price, Timestamp endDate, AuctionCurrency currency = AuctionCurrency.Game_Coin)
    {
        CanvasManager.Instance.ShowOperationPopup("Putting an asset for sale on the blockchain asset market...", false);

        // Add the possibility to create auctions with other coins than SOUL and pass the other coin as an argument of the SellToken method
        var script = ScriptUtils.BeginScript()
            .AllowGas(from, 1, 9999)
            .CallContract("market", "SellToken", from, PhantasmaDemo.TOKEN_SYMBOL, "SOUL", car.TokenID, price, endDate)
            .SpendGas(from)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                StartCoroutine(CheckAssetSale(car, price, endDate, result));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideSellPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }
        ));
    }

    /// <summary>
    /// Check if the auction sale creation was successful
    /// </summary>
    private IEnumerator CheckAssetSale(Car car, BigInteger price, Timestamp endDate, string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking auction sale creation...", true);

        yield return PhantasmaDemo.Instance.CheckOperation(PhantasmaDemo.EBLOCKCHAIN_OPERATION.SELL_ASSET, result,
            (tx) =>
            {
                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        if (eKind == EventKind.AuctionCreated)
                        {
                            var bytes = Base16.Decode(evt.data);
                            var marketEventData = Serialization.Unserialize<MarketEventData>(bytes);
                            
                            var newAuction = new Auction
                            {
                                creatorAddress  = PhantasmaDemo.Instance.Key.Address.ToString(),
                                endDate         = endDate.Value,
                                baseSymbol      = marketEventData.BaseSymbol,
                                quoteSymbol     = marketEventData.QuoteSymbol,
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
                            CanvasManager.Instance.HideOperationPopup();
                            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.SUCCESS, "Asset put up for sale in the market with success.");
                            return;
                        }
                    }
                }

                CanvasManager.Instance.HideSellPopup();
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Something failed while creating the auction sale on the market. Please try again.");
            },
            ((errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideSellPopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }));
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset(Car car)
    {
        CanvasManager.Instance.ShowOperationPopup("Removing an asset from the blockchain market...", false);

        var script = ScriptUtils.BeginScript()
            .AllowGas(car.OwnerAddress, 1, 9999)
            .CallContract("market", "BuyToken", PhantasmaDemo.Instance.Key.Address, PhantasmaDemo.TOKEN_SYMBOL, car.TokenID)
            .SpendGas(car.OwnerAddress)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                StartCoroutine(CheckAssetRemoval(car, result));
            },
            (errorType, errorMessage) =>
            {
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
                CanvasManager.Instance.HideOperationPopup();
            }
        ));
    }

    /// <summary>
    /// Check if the auction sale creation was successful
    /// </summary>
    private IEnumerator CheckAssetRemoval(Car car, string result)
    {
        CanvasManager.Instance.ShowOperationPopup("Checking auction sale removal...", true);

        yield return PhantasmaDemo.Instance.CheckOperation(PhantasmaDemo.EBLOCKCHAIN_OPERATION.REMOVE_ASSET, result,
            (tx) =>
            {
                foreach (var evt in tx.events)
                {
                    EventKind eKind;
                    if (Enum.TryParse(evt.kind, out eKind))
                    {
                        if (eKind == EventKind.AuctionCancelled)
                        {
                            //Debug.Log(evt.kind + " - " + marketEventData.ID);

                            //var carData = car.Data;
                            //car.Data    = carData;

                            var carMutableData = car.MutableData;
                            carMutableData.location = CarLocation.None;

                            car.OwnerAddress = PhantasmaDemo.Instance.Key.Address;
                            car.MutableData = carMutableData;

                            CarAuctions.Remove(car.TokenID);
                            SellCarAuctions.Remove(car.TokenID);

                            PhantasmaDemo.Instance.MyCars.Add(car.TokenID, car);

                            CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);

                            CanvasManager.Instance.HideRemovePopup();
                            CanvasManager.Instance.HideOperationPopup();

                            GetMarket((auctions) =>
                            {
                                CanvasManager.Instance.marketMenu.SetContent(auctions);

                                if (CarAuctions.Keys.Count > 0)
                                {
                                    CanvasManager.Instance.marketMenu.SelectMarketSellTab();
                                }
                            });

                            CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.SUCCESS, "Asset removed from the market with success.");
                            return;
                        }
                    }
                }

                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideRemovePopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, "Something failed while removing the auction from the market. Please try again.");
            },
            ((errorType, errorMessage) =>
            {
                CanvasManager.Instance.HideOperationPopup();
                CanvasManager.Instance.HideRemovePopup();
                CanvasManager.Instance.ShowResultPopup(EOPERATION_RESULT.FAIL, errorType + " - " + errorMessage);
            }));
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
