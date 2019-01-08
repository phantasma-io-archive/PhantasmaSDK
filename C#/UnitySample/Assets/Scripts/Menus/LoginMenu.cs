using UnityEngine;
using UnityEngine.UI;

public class LoginMenu : MonoBehaviour
{
    public InputField   addressInputField;
    public Text         loginError;

    // Start is called before the first frame update
    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    void OnEnable()
    {
        ClearContent();
    }

    public void Login()
    {
        PhantasmaDemo.Instance.Login(addressInputField.text);
    }

    public void SetLoginError(string error)
    {
        loginError.text = error;
    }

    private void ClearContent()
    {
        addressInputField.text  = string.Empty;
        loginError.text         = string.Empty;
    }
}
