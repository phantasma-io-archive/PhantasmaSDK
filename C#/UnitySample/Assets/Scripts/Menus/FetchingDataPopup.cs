using UnityEngine;
using UnityEngine.UI;

public class FetchingDataPopup : MonoBehaviour
{
    private const float     _MIN_VISIBLE_DURATION   = 2.5f;
    private const string    _DEFAULT_MESSAGE        = "Fetching data from the Blockchain...";

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

    public void ShowPopup(string msg = null)
    {
        message.text = msg == null ? _DEFAULT_MESSAGE : msg;

        gameObject.SetActive(true);
    }

    public void HidePopup()
    {
        _isClosing = true;
    }
}
