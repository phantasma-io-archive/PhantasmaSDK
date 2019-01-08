using UnityEngine;

public class MyGameAsset
{
    public string   Name    { get; private set; }
    public decimal  Price   { get; private set; }
    public Sprite   Icon    { get; private set; }

    public void SetAsset(string assetName, decimal assetPrice, Sprite assetIcon)
    {
        Name    = assetName;
        Price   = assetPrice;
        Icon    = assetIcon;
    }
}
