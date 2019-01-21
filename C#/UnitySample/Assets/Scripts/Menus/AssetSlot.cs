using UnityEngine;
using UnityEngine.UI;

public enum EASSET_TYPE
{
    MY_ASSET,
    BUY_MARKET_ASSET,
    SELL_MARKET_ASSET
}

public class AssetSlot : MonoBehaviour
{
    public const float SLOT_HEIGHT = 250f;

    public Text     nameText, priceText, powerText, speedText;
    public Button   buyButton, sellButton, removeButton;
    public Image    assetImage;

    private Car _asset;

    public void SetSlot(Car car, EASSET_TYPE type)
    {
        _asset = car;

        nameText.text   = car.MutableData.name;
        powerText.text  = "Power: " + car.MutableData.power;
        speedText.text  = "Speed: " + car.MutableData.speed;

        assetImage.sprite   = car.Image;

        switch (type)
        {
            case EASSET_TYPE.MY_ASSET:
                priceText.gameObject.SetActive(false);

                sellButton.gameObject.SetActive(true);

                buyButton.gameObject.SetActive(false);
                removeButton.gameObject.SetActive(false);
                break;
            case EASSET_TYPE.BUY_MARKET_ASSET:

                priceText.text  = PhantasmaDemo.Instance.market.CarAuctions[car.TokenID].auction.price;
                priceText.gameObject.SetActive(true);

                buyButton.gameObject.SetActive(true);

                sellButton.gameObject.SetActive(false);
                removeButton.gameObject.SetActive(false);
                break;
            case EASSET_TYPE.SELL_MARKET_ASSET:

                if (PhantasmaDemo.Instance.market.CarAuctions.ContainsKey(car.TokenID))
                {
                    priceText.text = PhantasmaDemo.Instance.market.CarAuctions[car.TokenID].auction.price.ToString();
                    priceText.gameObject.SetActive(true);
                }
                else
                {
                    Debug.Log("auctions keys: " + PhantasmaDemo.Instance.market.CarAuctions.Keys.Count + " | id: " + car.TokenID);
                }

                removeButton.gameObject.SetActive(true);

                buyButton.gameObject.SetActive(false);
                sellButton.gameObject.SetActive(false);
                break;
        }

    }

    /// <summary>
    /// Buy this asset from the market
    /// </summary>
    public void BuyAsset()
    {
        CanvasManager.Instance.ShowBuyPopup(_asset);
    }

    /// <summary>
    /// Put this asset for sale on the market
    /// </summary>
    public void SellAsset()
    {
        CanvasManager.Instance.ShowSellPopup(_asset);
    }

    /// <summary>
    /// Remove an asset that is for sale on the market
    /// </summary>
    public void RemoveAsset()
    {
        CanvasManager.Instance.ShowRemovePopup(_asset);
    }
}
