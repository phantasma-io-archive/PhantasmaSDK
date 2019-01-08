using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MainMenu : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void AccountClicked()
    {
        CanvasManager.Instance.OpenAccount();
    }

    public void MyAssetsClicked()
    {
        CanvasManager.Instance.OpenMyAssets();
    }

    public void MarketClicked()
    {
        CanvasManager.Instance.OpenMarket();
    }

    public void LogoutClicked()
    {
        PhantasmaDemo.Instance.LogOut();
    }
}
