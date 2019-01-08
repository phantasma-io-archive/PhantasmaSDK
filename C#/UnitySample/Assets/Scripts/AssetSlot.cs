using System.Globalization;
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

    public Text     nameText, priceText;
    public Button   buyButton, sellButton, removeButton;
    public Image    assetImage;

    public MyGameAsset asset;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    public void SetSlot(MyGameAsset asset, EASSET_TYPE type)
    {
        nameText.text       = asset.Name;
        priceText.text      = asset.Price.ToString(CultureInfo.InvariantCulture);
        assetImage.sprite   = asset.Icon;

        switch (type)
        {
            case EASSET_TYPE.MY_ASSET:
                sellButton.gameObject.SetActive(true);

                buyButton.gameObject.SetActive(false);
                removeButton.gameObject.SetActive(false);
                break;
            case EASSET_TYPE.BUY_MARKET_ASSET:
                buyButton.gameObject.SetActive(true);

                sellButton.gameObject.SetActive(false);
                removeButton.gameObject.SetActive(false);
                break;
            case EASSET_TYPE.SELL_MARKET_ASSET:
                sellButton.gameObject.SetActive(true);

                buyButton.gameObject.SetActive(false);
                removeButton.gameObject.SetActive(false);
                break;
        }

    }
}
