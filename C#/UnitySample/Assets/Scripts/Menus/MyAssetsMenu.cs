using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MyAssetsMenu : MonoBehaviour
{
    public GameObject   galleryContent;
    public Text         errorMessage;
    public AssetSlot    assetSlot;

    private List<AssetSlot> _assetSlots;

    void Awake()
    {
        _assetSlots = new List<AssetSlot>();
    }

    void OnEnable()
    {
        //Debug.Log("my: " + PhantasmaDemo.Instance.MyCars.Count + " | slots: " + _assetSlots.Count);
        //if (_assetSlots.Count != PhantasmaDemo.Instance.MyCars.Keys.Count)
        //{
            UpdateMyAssets();
        //}
    }

    public void UpdateMyAssets()
    {
        // TODO optimized this to use pool slots
        foreach (var slot in _assetSlots)
        {
            DestroyImmediate(slot.gameObject);
        }

        _assetSlots.Clear();

        //Debug.Log("my assets: " + PhantasmaDemo.Instance.MyCars.Keys.Count);
        var tokenIds = new List<string>(PhantasmaDemo.Instance.MyCars.Keys);

        for (var i = 0; i < tokenIds.Count; i++)
        {
            var asset = PhantasmaDemo.Instance.MyCars[tokenIds[i]];

            var newSlot                     = Instantiate(assetSlot, galleryContent.transform, false);
            newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
            newSlot.SetSlot(asset, EASSET_TYPE.MY_ASSET);
            newSlot.gameObject.SetActive(true);

            _assetSlots.Add(newSlot);
        }
    }

    public void ShowError(string error)
    {
        errorMessage.text = error;
        errorMessage.gameObject.SetActive(true);
    }

    private void ClearErrorContent()
    {
        errorMessage.text = string.Empty;
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMyAssets();
    }
}


