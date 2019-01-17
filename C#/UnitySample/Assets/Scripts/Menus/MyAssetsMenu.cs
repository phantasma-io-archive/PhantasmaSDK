using System.Collections.Generic;
using Phantasma.Numerics;
using UnityEngine;

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
        //Debug.Log("my: " + PhantasmaDemo.Instance.MyCars.Count + " | slots: " + _assetSlots.Count);
        if (_assetSlots.Count != PhantasmaDemo.Instance.MyCars.Count)
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

        //Debug.Log("my assets: " + PhantasmaDemo.Instance.MyCars.Keys.Count);
        var carIds = new List<BigInteger>(PhantasmaDemo.Instance.MyCars.Keys);

        for (var i = 0; i < carIds.Count; i++)
        {
            var asset = PhantasmaDemo.Instance.MyCars[carIds[i]];

            var newSlot                     = Instantiate(assetSlot, galleryContent.transform, false);
            newSlot.transform.localPosition += Vector3.down * AssetSlot.SLOT_HEIGHT * i;
            newSlot.SetSlot(asset, EASSET_TYPE.MY_ASSET);
            newSlot.gameObject.SetActive(true);

            _assetSlots.Add(newSlot);
        }
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseMyAssets();
    }
}


