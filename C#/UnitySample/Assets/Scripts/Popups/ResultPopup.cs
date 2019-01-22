using UnityEngine;
using UnityEngine.UI;

public class ResultPopup : MonoBehaviour
{
    // TODO use this to show both errors and successful operations (change between red and green colors)

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