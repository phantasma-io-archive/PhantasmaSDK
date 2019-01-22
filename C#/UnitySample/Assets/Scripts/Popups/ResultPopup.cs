using UnityEngine;
using UnityEngine.UI;

public enum ERESULT_TYPE
{
    FAIL,
    SUCCESS
}

public class ResultPopup : MonoBehaviour
{
    private readonly Color _FAIL_COLOR      = new Color(241 / 255f, 137 / 255f, 137 / 255f);
    private readonly Color _SUCCESS_COLOR   = new Color(167 / 255f, 255 / 255f, 131 / 255f);
    
    public Text     message;
    public Image    background;

    public void ShowPopup(ERESULT_TYPE type, string msg)
    {
        message.text = msg;

        switch (type)
        {
            case ERESULT_TYPE.FAIL:
                background.color = _FAIL_COLOR;
                break;
            case ERESULT_TYPE.SUCCESS:
                background.color = _SUCCESS_COLOR;
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