using System;
using Phantasma.Cryptography;
using Phantasma.Numerics;

public enum CarRarity
{
    Common      = 0,
    Rare        = 1,
    Legendary   = 2
}

public enum CarLocation
{
    None    = 0,
    Race    = 1,
    Market  = 2
}

public enum TransactionType
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

[Flags]
public enum CarFlags
{
    None    = 0,
    Locked  = 1,
}

public enum AuctionCurrency
{
    Game_Coin,
    Soul
}


public struct NachoTransaction
{
    public uint             timestamp;
    public TransactionType  type;
    public BigInteger       amount;
    public byte[]           content;
}

public struct CarInfo
{
    public Address      owner;
    public byte         power;
    public byte         speed;
    public CarLocation  location;
    public BigInteger   carID;
    public BigInteger   auctionID;
    public CarFlags     flags;
}

public struct Auction
{
    public uint         startTime;
    public uint         endTime;
    public BigInteger   contentID;
    public BigInteger   startPrice;
    public BigInteger   endPrice;
    public Address      creator;
    public AuctionCurrency currency;
}

public struct Sale
{
    public uint         time;
    public BigInteger   auctionID;
    public BigInteger   price;
    public Address      buyer;
}

public struct Account
{
    public uint         creationTime;
    public BigInteger   balanceGameCoin;
    public BigInteger   raceID;
    public string       unused;
    public string       address;
    public AccountFlags flags;
    public int          ELO;
    public RaceMode     queueMode;
    public uint         queueJoinTime;
    public uint         queueUpdateTime;
    public Address      queueVersus;
    public BigInteger[] queueIDs;
}