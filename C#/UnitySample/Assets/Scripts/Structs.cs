using System;
using Phantasma.SDK;

public enum CarRarity
{
    Common      = 0,
    Rare        = 1,
    Legendary   = 2
}

public enum CarLocation
{
    None        = 0,
    Race        = 1,
    Market      = 2,
    Repair_Shop = 3,
}

public enum TransactionKind
{
    Other               = 0,
    Deposit             = 1,
    Withdraw            = 2,
    Auction_Purchase    = 3,
    Auction_Sale        = 4,
    Refund              = 5,
    Rename              = 6,
    Match_Prize         = 7,
    Sent_Car            = 8,
    Received_Car        = 9,
    Transfer            = 10,
    Trophy_Received     = 11,
    Created_Car         = 12
}

public enum RaceMode
{
    None        = 0,
    Pratice     = 1,
    Unranked    = 2,
    Ranked      = 3,
    Versus      = 4
}

[Flags]
public enum AccountFlags
{
    None    = 0,
    Admin   = 1,
    Premium = 2,
    Banned  = 4,
}

public enum AuctionCurrency
{
    Game_Coin,
    Soul
}

public struct CarData
{
    public CarRarity    rarity;
    public int          imageID;
}

public struct CarMutableData
{
    public string       name;
    public byte         power;
    public byte         speed;
    public CarLocation  location;
}

public struct CarAuction
{
    public string       tokenID;
    public Auction      marketAuction;
}

//public struct Sale
//{
//    public uint         time;
//    public BigInteger   auctionID;
//    public BigInteger   price;
//    public Address      buyer;
//}