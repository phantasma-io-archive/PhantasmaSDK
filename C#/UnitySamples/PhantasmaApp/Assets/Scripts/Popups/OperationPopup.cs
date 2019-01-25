using System.Net.Configuration;
using UnityEngine;
using UnityEngine.UI;

public class OperationPopup : MonoBehaviour
{
    private const float _MIN_VISIBLE_DURATION   = 2.5f;

    public Text             message;
    public Button           cancelButton;
    public RectTransform    loadingBarRectComponent;
    public float            rotateSpeed = 2.5f;

    private bool    _isClosing;
    private float   _timeVisible;
    private float   _loadingBarRotation;

    void OnEnable()
    {
        _isClosing      = false;
        _timeVisible    = 0f;
    }

    void Update()
    {
        _loadingBarRotation                         = _loadingBarRotation + (Time.deltaTime * rotateSpeed);
        loadingBarRectComponent.transform.rotation  = Quaternion.Euler(0f, 0f, -72f * (int)_loadingBarRotation);

        _timeVisible += Time.unscaledDeltaTime;

        if (_isClosing && _timeVisible >= _MIN_VISIBLE_DURATION)
        {
            gameObject.SetActive(false);
        }
    }

    public void ShowPopup(string msg, bool showCancelButton)
    {
        Debug.Log("------> " + msg);
        _isClosing = false;
        _timeVisible = 0;

        message.text = msg;

        cancelButton.gameObject.SetActive(showCancelButton);

        gameObject.SetActive(true);
    }

    public void HidePopup()
    {
        _isClosing = true;
    }

    public void CancelClicked()
    {
        //PhantasmaDemo.Instance.CancelTransaction();
    }
}
