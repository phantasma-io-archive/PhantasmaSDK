public static class Constants
{
    public static readonly string ACCOUNT_CARS = "team";
    
    public const string ANONYMOUS_NAME = "Anonymous";
    public const string ACADEMY_NAME    = "Academy";

    public const uint SOUL_TICKER = 2827;

    public const decimal DOLLAR_GAMECOIN_RATE = 100; // 1 USD = 100 GAMECOINS

    public const string PHANTASMA_DEV_ADDRESS = "PGUHKgY6o72fTQCBHstFcBNwqfaFKMFAEGDr2pfxWg5bV";

    public const uint CAR_MIN_LEVEL = 1;
    public const uint CAR_MAX_LEVEL = 10;

    public const uint BASE_CAR_ID = 100;

    //public static readonly BigInteger MINIMUM_AUCTION_PRICE = TokenUtils.ToBigInteger(0.01m);
    //public static readonly BigInteger MAXIMUM_AUCTION_PRICE = TokenUtils.ToBigInteger(100000.0m);

    public static readonly int MINIMUM_AUCTION_DURATION = 86400; // in seconds 
    public static readonly int MAXIMUM_AUCTION_DAYS_DURATION = 30;
    public static readonly int MAXIMUM_AUCTION_SECONDS_DURATION = 86400 * MAXIMUM_AUCTION_DAYS_DURATION; // in seconds 

    // default ELO rating of a player
    public const int DEFAULT_SCORE = 1000;

    public static readonly uint SECONDS_PER_DAY = 86400;
    public static readonly uint SECONDS_PER_HOUR = 3600;

    public const int MATCHMAKER_UPDATE_SECONDS = 5;
    public const int MATCHMAKER_REMOVAL_SECONDS = 600;

    public const int DEFAULT_ELO = 1200;
}
