using System;
using System.Collections.Generic;
using Phantasma.Blockchain.Contracts;
using Phantasma.Blockchain.Contracts.Native;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.VM.Utils;
using Phantasma.IO;
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

    private const int _MARKET_CARS_COUNT = 20;

    private Dictionary<BigInteger, CarData> _cars;

    public Dictionary<BigInteger, Auction>  Auctions            { get; private set; }
    public List<Car>                        MarketBuyAssets     { get; private set; }
    public List<Car>                        MarketSellAssets    { get; private set; }
    
    private void Awake()
    {
        MarketBuyAssets     = new List<Car>();
        MarketSellAssets    = new List<Car>();
    }

    // Start is called before the first frame update
    void Start()
    {

    }
    
    /// <summary>
    /// Fill the market with new assets
    /// </summary>
    public void FillMarket()
    {
        _cars = new Dictionary<BigInteger, CarData>();

        while (_cars.Keys.Count < _MARKET_CARS_COUNT)
        {
            CreateCar(out var car);
            
            MarketBuyAssets.Add(car);
        }
    }

    /// <summary>
    /// Get the market with the current assets
    /// </summary>
    public void GetMarket()
    {
        //PhantasmaDemo.Instance.PhantasmaApi.GetTokenData("CAR", result);

    }

    private void CreateCar(out Car car)
    {
        var cars = _cars; //Storage.FindMapForContract<BigInteger, CarData>(GLOBAL_CARS_LIST); //TODO

        var carData = new CarData
        {
            owner       = PhantasmaDemo.Instance.Key.Address,
            name        = "Super Cadillac", 
            power       = (byte)Random.Range(1, 10),
            speed       = (byte)Random.Range(1, 10),
            location    = CarLocation.Market,
            flags       = CarFlags.Locked
        };
      
        var txData = Serialization.Serialize(carData);
        //var txData = carData.Serialize();

        var script = ScriptUtils.BeginScript()
                        .AllowGas(PhantasmaDemo.Instance.Key.Address, 1, 9999)
                        .CallContract("token", "MintToken", PhantasmaDemo.Instance.Key.Address, "CAR", txData)
                        .SpendGas(PhantasmaDemo.Instance.Key.Address)
                        .EndScript();

        var carID = new BigInteger(-1);

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main", 
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    foreach (var evt in tx.events)
                    {
                        Debug.Log("has event: " + evt.kind + " - " + evt.data);

                        if (Enum.TryParse(evt.kind, out EventKind eKind))
                        {
                            if (eKind == EventKind.TokenMint)
                            {
                                var bytes   = Base16.Decode(evt.data);
                                var data    = Serialization.Unserialize<TokenEventData>(bytes);

                                carID   = data.value;

                                // save car
                                cars.Add(carID, carData);

                                PhantasmaDemo.Instance.PhantasmaApi.LogTransaction(PhantasmaDemo.Instance.Key.Address, 0, TransactionType.Created_Car, carID);
                                
                                break;
                            }
                            else
                            {
                                // TODO aconteceu algum erro...
                            }
                        }
                        else
                        {
                            // TODO aconteceu algum erro..
                        }
                    }

                    if (carID == null)
                    {

                    }
                    else
                    {
                        return;
                    }

                }));

            },
            (errorType, errorMessage) =>
            {
                // TODO
                CanvasManager.Instance.loginMenu.SetLoginError(errorType + " - " + errorMessage);
            }
        ));

        if (carID > 0)
        {
            car = new Car();
            car.SetCar(carID, 0, carData, PhantasmaDemo.Instance.carImages[Random.Range(0, PhantasmaDemo.Instance.carImages.Count)]);
        }
        else
        {
            car = null;
        }
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        _cars.Remove(car.CarID);
        MarketBuyAssets.Remove(car);

        PhantasmaDemo.Instance.MyCars.Add(car);

        var carData = car.Data;

        var auction = GetAuction(car.AuctionID);
        
        var activeAuctionsList = new List<BigInteger>(); //Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
        //Expect(activeList.Contains(auctionID), "auction finished");

        //var car = GetCar(carID);

        if (car.Data.owner != PhantasmaDemo.Instance.Key.Address) //TODO to)
        {
            //ProcessAuctionSale(to, auctionID, ref auction);

            var oldTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, auction.creator); // TODO
            oldTeam.Remove(car.CarID);

            var newTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, to);
            newTeam.Add(car.CarID);

            // update car owner
            carData.owner = PhantasmaDemo.Instance.Key.Address; // TODO to;
        }

        carData.location = CarLocation.None;
        //SetCar(carData.carID, car);

        car.AuctionID = 0;

        // delete this auction from active list
        activeAuctionsList.Remove(car.AuctionID);

        //Notify(to, NachoEvent.Purchase);
        
        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    public void SellAsset(Car car, Address from, BigInteger startPrice, BigInteger endPrice, AuctionCurrency currency = AuctionCurrency.Game_Coin, uint duration = 30)
    {
        _cars.Add(car.CarID, car.Data);

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


        car.AuctionID       = auctionID;
        carData.location    = CarLocation.Market;
        //SetCar(carID, car);

        CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset(Car car)
    {
        _cars.Remove(car.CarID);
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
        var auctions = Auctions; //TODO Storage.FindMapForContract<BigInteger, Auction>(GLOBAL_AUCTIONS_LIST);
        var auction = auctions[auctionID];

        return auction;
    }
}
