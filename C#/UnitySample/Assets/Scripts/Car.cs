using Phantasma.Cryptography;
using Phantasma.Numerics;
using UnityEngine;

public class Car
{
    public Address          OwnerAddress    { get; set; }
    public BigInteger       CarID           { get; set; }
    public BigInteger       AuctionID       { get; set; }
    public CarData          Data            { get; set; }
    public CarMutableData   MutableData     { get; set; }
    public Sprite           Image           { get; set; }

    public void SetCar(BigInteger cardID, BigInteger auctionID, CarData data, CarMutableData mutableData)
    {
        CarID       = cardID;
        AuctionID   = auctionID;
        Data        = data;
        MutableData = mutableData;
        Image       = PhantasmaDemo.Instance.carImages[data.imageID];
    }
}
