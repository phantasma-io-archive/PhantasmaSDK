using System.Collections.Generic;

using UnityEngine;

using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.VM.Utils;

public class Market : MonoBehaviour
{
    //private Dictionary<BigInteger, CarData> _cars;

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
    /// Get the market with the current assets
    /// </summary>
    public void GetMarket()
    {
        // TODO o método GetAuctionIDs não existe no Spook

        //PhantasmaDemo.Instance.PhantasmaApi.GetTokenData("CAR", result);
        //var script = ScriptUtils.BeginScript()
        //    .AllowGas(PhantasmaDemo.Instance.Key.Address, 1, 9999)
        //    .CallContract("token", "GetAuctionsIDs", PhantasmaDemo.Instance.Key.Address, car.AuctionID)
        //    .SpendGas(PhantasmaDemo.Instance.Key.Address)
        //    .EndScript();

        //Storage.FindMapForContract<BigInteger, Auction>(GLOBAL_AUCTIONS_LIST);
    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    public void BuyAsset(Car car)
    {
        var script = ScriptUtils.BeginScript()
            .AllowGas(car.OwnerAddress, 1, 9999)
            .CallContract("token", "BuyToken", car.OwnerAddress, car.AuctionID)
            .SpendGas(car.OwnerAddress)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    var carData         = car.Data;
                    var mutableCarData  = car.MutableData;
                    
                    //_cars.Remove(car.CarID);
                    MarketBuyAssets.Remove(car);

                    PhantasmaDemo.Instance.MyCars.Add(car.CarID, car);

                    var auction = GetAuction(car.AuctionID);

                    // TODO confirmar que foi removido um leilão na blockchain e remover tb da lista local
                    // var auctionIDs = (BigInteger[])simulator.Nexus.RootChain.InvokeContract("market", "GetAuctionIDs");
                    Auctions = new Dictionary<BigInteger, Auction>(); //Storage.FindMapForContract<BigInteger, NachoAuction>(GLOBAL_AUCTIONS_LIST);
                    Auctions.Remove(car.AuctionID);
                    
                    var activeAuctionsList = new List<BigInteger>(); //Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
                    //Expect(activeList.Contains(auctionID), "auction finished");

                    //var car = GetCar(carID);

                    if (car.OwnerAddress != PhantasmaDemo.Instance.Key.Address)
                    {
                        //ProcessAuctionSale(to, auctionID, ref auction);

                        var oldTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, auction.creator); // TODO
                        oldTeam.Remove(car.CarID);

                        var newTeam = new List<BigInteger>(); //Storage.FindCollectionForAddress<BigInteger>(ACCOUNT_CARS, to);
                        newTeam.Add(car.CarID);

                        // update car owner
                        car.OwnerAddress = PhantasmaDemo.Instance.Key.Address;
                    }

                    mutableCarData.location = CarLocation.None;

                    car.Data        = carData;
                    car.AuctionID   = 0;

                    // delete this auction from active list
                    activeAuctionsList.Remove(car.AuctionID);

                    //Notify(to, NachoEvent.Purchase);

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
    public void SellAsset(Car car, Address from, BigInteger startPrice, BigInteger endPrice, AuctionCurrency currency = AuctionCurrency.Game_Coin, uint duration = 30)
    {
        //_cars.Add(car.CarID, car.Data);

        MarketSellAssets.Add(car);

        PhantasmaDemo.Instance.MyCars.Remove(car.CarID);

        var carData         = car.Data;
        var carMutableData  = car.MutableData;

        var script = ScriptUtils.BeginScript()
            .AllowGas(from, 1, 9999)
            .CallContract("token", "SellToken", from, PhantasmaDemo.TOKEN_SYMBOL, car.CarID, startPrice)
            .SpendGas(from)
            .EndScript();

        StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.SignAndSendTransaction(script, "main",
            (result) =>
            {
                Debug.Log("sign result: " + result);

                StartCoroutine(PhantasmaDemo.Instance.PhantasmaApi.GetTransaction(result, (tx) =>
                {
                    // TODO confirmar que foi criado mais um leilão na blockchain e adicionar na lista local
                    // var auctionIDs = (BigInteger[])simulator.Nexus.RootChain.InvokeContract("market", "GetAuctionIDs");
                    Auctions = new Dictionary<BigInteger, Auction>(); //Storage.FindMapForContract<BigInteger, NachoAuction>(GLOBAL_AUCTIONS_LIST);
                    var auctionID = Auctions.Keys.Count + 1;
                    //Auctions.Add(auctionID, auction);

                    var activeList = new List<BigInteger>(); //Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
                    activeList.Add(auctionID);

                    carMutableData.location = CarLocation.Market;

                    car.Data        = carData;
                    car.AuctionID   = auctionID;
                    
                    CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();

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
        //_cars.Remove(car.CarID);
        MarketSellAssets.Remove(car);

        PhantasmaDemo.Instance.MyCars.Add(car.CarID, car);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }

    //public BigInteger[] GetActiveAuctions()
    //{
    //    var activeList = Storage.FindCollectionForContract<BigInteger>(ACTIVE_AUCTIONS_LIST);
    //    return activeList.All();
    //}

    public Auction GetAuction(BigInteger auctionID)
    {
        var auctions = Auctions;
        var auction = auctions[auctionID];

        return auction;
    }
}
