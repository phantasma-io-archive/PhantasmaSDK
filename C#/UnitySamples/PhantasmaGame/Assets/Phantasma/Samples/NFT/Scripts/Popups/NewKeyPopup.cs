using UnityEngine;
using UnityEngine.UI;

public class NewKeyPopup : MonoBehaviour
{
    public Text privateKey, copiedKeyMessage;
    
    public void ShowPopup(string key)
    {
        privateKey.text = key;
    
        copiedKeyMessage.gameObject.SetActive(false);
        
        gameObject.SetActive(true);
    }

    public void CopyKeyClicked()
    {
        copiedKeyMessage.gameObject.SetActive(true);

        GUIUtility.systemCopyBuffer = privateKey.text;
    }

    public void HidePopup()
    {
        privateKey.text = string.Empty;

        gameObject.SetActive(false);
    }
}
