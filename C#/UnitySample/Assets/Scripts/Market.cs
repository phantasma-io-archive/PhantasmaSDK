using System;
using System.Collections.Generic;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.RpcClient.DTOs;
using Phantasma.VM.Utils;
using Phantasma.IO;
using Phantasma.SDK;
using UnityEngine;
using Random = UnityEngine.Random;

public class Market : MonoBehaviour
{
    // TODO move this constants to another file related to the API
    public static readonly string ACCOUNT_CARS          = "teamCars";
    public static readonly string ACCOUNT_HISTORY       = "history";

    public static readonly string GLOBAL_CARS_LIST      = "allCars";
    public static readonly string ACTIVE_AUCTIONS_LIST  = "activeAuctions";
    public static readonly string GLOBAL_AUCTIONS_LIST  = "allAuctions";
    public static readonly string GLOBAL_SALES_LIST     = "sales";

    private Dictionary<BigInteger, CarData> _cars;

    public Dictionary<BigInteger, Auction>  Auctions            { get; private set; }
    public List<Car>                        MarketBuyAssets    { get; private set; }
    public List<Car>                        MarketSellAssets   { get; private set; }
    
    private void Awake()
    {
        MarketBuyAssets     = new List<Car>();
        MarketSellAssets    = new List<Car>();
    }

    // Start is called before the first frame update
    void Start()
    {
        FillMarket();
    }
    
    public void FillMarket()
    {
        foreach (var carImage in PhantasmaDemo.Instance.carImages)
        {
            var newCarID = CreateCar(carImage.name);

            var newCar = new Car();
            newCar.SetCar(_cars[newCarID], carImage);

            MarketBuyAssets.Add(newCar);
        }
    }

    private BigInteger CreateCar(string name)
    {
        var cars = _cars; //Storage.FindMapForContract<BigInteger, CarData>(GLOBAL_CARS_LIST); //TODO

        BigInteger carID = _cars.Keys.Count + Constants.BASE_CAR_ID;

        var car = new CarData()
        {
            owner       = PhantasmaDemo.Instance.Key.Address,
            power       = (byte)Random.Range(1,10),
            speed       = (byte)Random.Range(1, 10),
            location    = CarLocation.Market,
            carID       = carID,
            auctionID   = 0,
            flags       = CarFlags.Locked
        };

        // save car
        cars.Add(carID, car);
        
        var txData = car.Serialize();

        var script = ScriptUtils.BeginScript()
                        .AllowGas(PhantasmaDemo.Instance.Key.Address, 1, 9999)
                        .CallContract("nexus", "MintToken", PhantasmaDemo.Instance.Key.Address, "CAR", txData)
                        .SpendGas(PhantasmaDemo.Instance.Key.Address)
                        .EndScript();
                    
        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "nexus", 
            (result) =>
            {
                //_cars.Add((BigInteger)result, car); TODO o que vem do MintToken() é um BigInteger com o id do novo token

                PhantasmaDemo.Instance.PhantasmaApi.LogTransaction(PhantasmaDemo.Instance.Key.Address, 0, TransactionType.Created_Car, carID);

                //return result); TODO

            },
            (errorType, errorMessage) =>
            {
                // TODO
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));

        return 0; //todo remove
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        _cars.Remove(car.Data.carID);
        MarketBuyAssets.Remove(car);

        PhantasmaDemo.Instance.MyCars.Add(car);

        var carData = car.Data;

        var auction = GetAuction(car.Data.auctionID);
        
        var activeList = new List<BigInteger>(); //Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
        //Expect(activeList.Contains(auctionID), "auction finished");

        //var carID = auction.contentID;
        //var car = GetCar(wrestlerID);

        if (car.Data.owner != PhantasmaDemo.Instance.Key.Address) //TODO to)
        {
            //ProcessAuctionSale(to, auctionID, ref auction);

            var oldTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, auction.creator); // TODO
            oldTeam.Remove(car.Data.carID);

            var newTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, to);
            newTeam.Add(car.Data.carID);

            // update wrestler owner
            carData.owner = PhantasmaDemo.Instance.Key.Address; // TODO to;
        }

        carData.location = CarLocation.None;
        carData.auctionID = 0;
        //SetCar(carData.carID, car);

        // delete this auction from active list
        activeList.Remove(carData.auctionID);

        //Notify(to, NachoEvent.Purchase);
        
        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    public void SellAsset(Car car) // TODO Address from, BigInteger wrestlerID, BigInteger startPrice, BigInteger endPrice, AuctionCurrency currency, uint duration)
    {
        _cars.Add(car.Data.carID, car.Data);

        MarketSellAssets.Add(car);

        PhantasmaDemo.Instance.MyCars.Remove(car);

        var carData = car.Data;

        // store info for the auction
        var currentTime = DateTime.UtcNow; //GetCurrentTime();
        var auction = new Auction()
        {
            //startTime   = currentTime,
            //endTime     = currentTime + duration,
            //startPrice = startPrice,
            //endPrice = endPrice,
            //currency = currency,
            //contentID = wrestlerID,
            //creator = from,
        };

        Auctions = new Dictionary<BigInteger, Auction>(); //Storage.FindMapForContract<BigInteger, NachoAuction>(GLOBAL_AUCTIONS_LIST);
        var auctionID = Auctions.Keys.Count + 1;
        Auctions.Add(auctionID, auction);

        var activeList = new List<BigInteger>(); //Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
        activeList.Add(auctionID);


        carData.auctionID = auctionID;
        carData.location = CarLocation.Market;
        //SetCar(carID, car);

        CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset(Car car)
    {
        _cars.Remove(car.Data.carID);
        MarketSellAssets.Remove(car);

        PhantasmaDemo.Instance.MyCars.Add(car);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }

    //public BigInteger[] GetActiveAuctions()
    //{
    //    var activeList = Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
    //    return activeList.All();
    //}

    public Auction GetAuction(BigInteger auctionID)
    {
        var auctions = Auctions; //Storage.FindMapForContract<BigInteger, Auction>(GLOBAL_AUCTIONS_LIST);
        var auction = auctions[auctionID];

        return auction;
    }
}
