using Phantasma.Numerics;
using UnityEngine;

public class Car
{
    public BigInteger       CarID       { get; set; }
    public BigInteger       AuctionID   { get; set; }
    public CarData          Data        { get; set; }
    public CarMutableData   MutableData { get; set; }
    public Sprite           Icon        { get; set; } // TODO A imagem do asset não devia estar guardado na blockchain?

    public void SetCar(BigInteger cardID, BigInteger auctionID, CarData data, CarMutableData mutableData, Sprite assetIcon)
    {
        CarID       = cardID;
        AuctionID   = auctionID;
        Data        = data;
        MutableData = mutableData;
        Icon        = assetIcon;
    }
}
