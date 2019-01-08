using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class MyAssetsMenu : MonoBehaviour
{
    public AssetSlot assetSlot;
    public GameObject galleryContent;

    private List<AssetSlot> _assetSlots;

    void Awake()
    {
        _assetSlots = new List<AssetSlot>();
    }

    void OnEnable()
    {
        Debug.Log("my: " + PhantasmaDemo.Instance.MyAssets.Count + " | slots: " + _assetSlots.Count);
        if (_assetSlots.Count != PhantasmaDemo.Instance.MyAssets.Count)
        {
            UpdateMyAssets();
        }
    }

    public void UpdateMyAssets()
    {
        // TODO optimized this to use pool slots
        foreach (var slot in _assetSlots)
        {
            DestroyImmediate(slot.gameObject);
        }

        _assetSlots.Clear();

        for (var i = 0; i < PhantasmaDemo.Instance.MyAssets.Count; i++)
        {
            var marketAsset = PhantasmaDemo.Instance.MyAssets[i];

            var newSlot                     = Instantiate(assetSlot, galleryContent.transform, false);
            newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
            newSlot.SetSlot(marketAsset, EASSET_TYPE.MY_ASSET);
            newSlot.gameObject.SetActive(true);

            _assetSlots.Add(newSlot);
        }
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMyAssets();
    }
}


