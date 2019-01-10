using Phantasma.Numerics;
using UnityEngine;

public class MyGameAsset
{
    public BigInteger   CardID  { get; private set; }
    public string       Name    { get; private set; }
    public decimal      Price   { get; private set; }
    public Sprite       Icon    { get; private set; }

    public void SetAsset(BigInteger cardID, string assetName, decimal assetPrice, Sprite assetIcon)
    {
        CardID  = cardID;
        Name    = assetName;
        Price   = assetPrice;
        Icon    = assetIcon;
    }
}
