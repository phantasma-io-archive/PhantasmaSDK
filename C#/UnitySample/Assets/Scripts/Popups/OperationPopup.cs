using UnityEngine;
using UnityEngine.UI;

public class OperationPopup : MonoBehaviour
{
    private const float _MIN_VISIBLE_DURATION   = 2.5f;

    public Text message;

    private float   _timeVisible;
    private bool    _isClosing;

    void OnEnable()
    {
        _isClosing      = false;
        _timeVisible    = 0f;
    }

    void Update()
    {
        _timeVisible += Time.unscaledDeltaTime;

        if (_isClosing && _timeVisible >= _MIN_VISIBLE_DURATION)
        {
            gameObject.SetActive(false);
        }
    }

    public void ShowPopup(string msg)
    {
        Debug.Log("show: " + msg);
        _isClosing = false;
        _timeVisible = 0;

        message.text = msg;

        gameObject.SetActive(true);
    }

    public void HidePopup()
    {
        _isClosing = true;
    }
}
