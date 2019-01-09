using System;
using System.Collections.Generic;
using Phantasma.Cryptography;
using Phantasma.Numerics;
using Phantasma.RpcClient.DTOs;
using Phantasma.VM.Utils;
using UnityEngine;
using Random = UnityEngine.Random;

public class Market : MonoBehaviour
{
    public static readonly string GLOBAL_CARS_LIST = "cars";

    private Dictionary<BigInteger, CarData> _cars;

    public bool debugMode = false; // TODO move this to API.cs

    public List<MyGameAsset> MarketBuyAssets    { get; private set; }
    public List<MyGameAsset> MarketSellAssets   { get; private set; }

    private void Awake()
    {
        MarketBuyAssets     = new List<MyGameAsset>();
        MarketSellAssets    = new List<MyGameAsset>();
    }

    // Start is called before the first frame update
    void Start()
    {
        FillMarket();
    }
    
    public void FillMarket()
    {
        // TODO get assets from the blockchain


        foreach (var carImage in PhantasmaDemo.Instance.carImages)
        {           
            var newCar = new MyGameAsset();
            newCar.SetAsset(carImage.name, (decimal)Random.Range(0f, 10f), carImage);

            MarketBuyAssets.Add(newCar);
        }
    }

    private void CreateCar()
    {
        //Create tx that calls 'MintToken' from 'Token' contract, passing as arguments (address, symbol, data)
        //  address = any address, the new minted token will appear in this address
        //  symbol = "CAR"
        //  data = byte array containing a serialized Car struct eg : var data = Phantasma.Serialization.Serializate(car)
        //the tx returns an ID that identifies the newly minted token

        var cars = _cars; //Storage.FindMapForContract<BigInteger, CarData>(GLOBAL_CARS_LIST);

        BigInteger carID = _cars.Keys.Count + Constants.BASE_CAR_ID;

        var car = new CarData()
        {
            owner       = PhantasmaDemo.Instance.Key.Address,
            power       = 1,
            speed       = 1,
            location    = CarLocation.Market,
            carID       = carID,
            auctionID   = 0,
            flags       = CarFlags.Locked
        };

        // save car
        cars.Add(carID, car);
        
        LogTransaction(PhantasmaDemo.Instance.Key.Address, 0, TransactionType.Created_Car, carID);

        //var script = ScriptUtils.BeginScript()
        //    .AllowGas(PhantasmaDemo.Instance.Key.Address, 1, 9999)
        //    .CallContract("nexus", "MintToken", PhantasmaDemo.Instance.Key.Address, "CAR", Phantasma.Serialization.Serializate(car))
        //    .SpendGas(PhantasmaDemo.Instance.Key.Address)
        //    .EndScript();

    }

    /// <summary>
    /// Buy an asset from the market and add it to my assets
    /// </summary>
    /// <param name="assetSlot"></param>
    public void BuyAsset(MyGameAsset asset)
    {
        MarketBuyAssets.Remove(asset);

        PhantasmaDemo.Instance.MyAssets.Add(asset);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.BUY);
    }

    /// <summary>
    /// Put an asset for sale on the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void SellAsset(MyGameAsset asset)
    {
        MarketSellAssets.Add(asset);

        PhantasmaDemo.Instance.MyAssets.Remove(asset);

        CanvasManager.Instance.myAssetsMenu.UpdateMyAssets();
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    /// <param name="assetSlot"></param>
    public void RemoveAsset(MyGameAsset asset)
    {
        MarketSellAssets.Remove(asset);

        PhantasmaDemo.Instance.MyAssets.Add(asset);

        CanvasManager.Instance.marketMenu.UpdateMarket(MarketMenu.EMARKETPLACE_TYPE.SELL);
    }

    // TODO add this method to the Phantasma API?
    private void LogTransaction<T>(Address address, BigInteger amount, TransactionType type, T content)
    {
        ImportantLog("-------------- LOG TRANSATION: " + type + " | " + amount);

        //var bytes = Serialization.Serialize(content);
        //LogTransaction(address, amount, kind, bytes);
    }

    public void ImportantLog(string s)
    {
        if (debugMode)
        {
            System.Diagnostics.Debug.WriteLine(s);
        }
        else
        {
            Console.ForegroundColor = ConsoleColor.Magenta;
            Console.WriteLine(s);
            Console.ForegroundColor = ConsoleColor.Gray;
        }
    }
}
