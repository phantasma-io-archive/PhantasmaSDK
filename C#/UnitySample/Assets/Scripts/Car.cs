using UnityEngine;

public class Car
{
    public CarData      Data    { get; set; }
    public Sprite       Icon    { get; set; }

    public void SetCar(CarData data, Sprite assetIcon)
    {
        Data    = data;
        Icon    = assetIcon;
    }
}
