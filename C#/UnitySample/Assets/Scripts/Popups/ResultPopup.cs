using UnityEngine;
using UnityEngine.UI;

public enum ERESULT_TYPE
{
    FAIL,
    SUCCESS
}

public class ResultPopup : MonoBehaviour
{  
    public Text message;

    public void ShowPopup(ERESULT_TYPE type, string msg)
    {
        message.text = msg;

        switch (type)
        {
            case ERESULT_TYPE.FAIL:
                message.color = Color.red;
                break;
            case ERESULT_TYPE.SUCCESS:
                message.color = Color.green;
                break;
        }

        gameObject.SetActive(true);
    }

    public void HidePopup()
    {
        message.text = string.Empty;

        gameObject.SetActive(false);
    }
}