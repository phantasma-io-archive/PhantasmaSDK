using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AdminMenu : MonoBehaviour
{
    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void CreateTokenClicked()
    {
        PhantasmaDemo.Instance.CreateToken();
    }

    public void PopulateMarketClicked()
    {
        PhantasmaDemo.Instance.market.FillMarket();
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAdmin();
    }


}
