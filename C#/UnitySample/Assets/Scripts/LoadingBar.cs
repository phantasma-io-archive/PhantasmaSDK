using UnityEngine;
using UnityEngine.UI;

public class LoadingBar : MonoBehaviour {

    public float rotateSpeed = 200f;

    private RectTransform   _rectComponent;
    private Image           _imageComp;
    private float           _currentvalue;

    // Use this for initialization
    void Awake()
    {
        _rectComponent  = GetComponent<RectTransform>();
        _imageComp      = _rectComponent.GetComponent<Image>();
    }

    // Update is called once per frame
    void Update()
    {
        _currentvalue = _currentvalue + (Time.deltaTime * rotateSpeed);

        _rectComponent.transform.rotation = Quaternion.Euler(0f, 0f, -72f * (int)_currentvalue);
    }
}