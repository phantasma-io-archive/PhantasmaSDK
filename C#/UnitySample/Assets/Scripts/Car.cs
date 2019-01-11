using Phantasma.Numerics;
using UnityEngine;

public class Car
{
    public BigInteger   CarID       { get; set; }
    public BigInteger   AuctionID   { get; set; }
    public CarData      Data        { get; set; }
    public Sprite       Icon        { get; set; }

    public void SetCar(BigInteger cardID, BigInteger auctionID, CarData data, Sprite assetIcon)
    {
        CarID       = cardID;
        AuctionID   = auctionID;
        Data        = data;
        Icon        = assetIcon;
    }
}
