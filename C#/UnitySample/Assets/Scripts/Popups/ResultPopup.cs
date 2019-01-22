using UnityEngine;
using UnityEngine.UI;

public class ResultPopup : MonoBehaviour
{
    public Text message;

    public void ShowPopup(string msg)
    {
        message.text = msg;

        gameObject.SetActive(true);
    }

    public void HidePopup()
    {
        message.text = string.Empty;

        gameObject.SetActive(false);
    }
}