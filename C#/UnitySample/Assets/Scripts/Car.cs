using Phantasma.Cryptography;
using UnityEngine;

public class Car
{
    public Address          OwnerAddress    { get; set; }
    public string           TokenID         { get; set; }
    public CarData          Data            { get; set; }
    public CarMutableData   MutableData     { get; set; }
    public Sprite           Image           { get; set; }

    public void SetCar(Address ownerAddress, string tokenID, CarData data, CarMutableData mutableData)
    {
        OwnerAddress    = ownerAddress;
        TokenID         = tokenID;
        Data            = data;
        MutableData     = mutableData;
        Image           = PhantasmaDemo.Instance.carImages[data.imageID];
    }
}
