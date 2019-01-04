using System.Globalization;
using UnityEngine;
using UnityEngine.UI;

public class AssetSlot : MonoBehaviour
{
    public Text     nameText, priceText;
    public Button   buyButton;
    public Image    assetImage;

	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}

    public void SetName(string name)
    {
        nameText.text = name;
    }

    public void SetPrice(decimal price)
    {
        priceText.text = price.ToString(CultureInfo.InvariantCulture);
    }

    public void ToggleBuyButton(bool isVisible)
    {
        buyButton.gameObject.SetActive(isVisible);
    }
}
