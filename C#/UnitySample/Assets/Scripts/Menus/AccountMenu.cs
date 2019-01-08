using UnityEngine;
using UnityEngine.UI;

public class AccountMenu : MonoBehaviour
{
    public Text balanceLabel;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void SetBalance(string balance)
    {
        balanceLabel.text = balance + "\n";
        //Debug.Log("set balance: " + balanceLabel.text);
    }

    public void AddBalanceEntry(string entry)
    {
        balanceLabel.text += entry + "\n";
        //Debug.Log("add entry: " + balanceLabel.text);
    }

    public void BackClicked()
    {
        CanvasManager.Instance.CloseAccount();
    }
}
