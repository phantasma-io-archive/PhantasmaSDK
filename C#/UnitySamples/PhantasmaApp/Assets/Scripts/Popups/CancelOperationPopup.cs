using UnityEngine;
using UnityEngine.UI;

public class CancelOperationPopup : MonoBehaviour
{
    public Text message;

    //public void ShowPopup(EOPERATION_RESULT type, string msg)
    //{
    //    message.text = msg;

    //    switch (type)
    //    {
    //        case EOPERATION_RESULT.FAIL:
    //            message.color = Color.red;
    //            break;
    //        case EOPERATION_RESULT.SUCCESS:
    //            message.color = Color.green;
    //            break;
    //    }

    //    gameObject.SetActive(true);
    //}

    public void HidePopup()
    {
        message.text = string.Empty;

        gameObject.SetActive(false);
    }
}
