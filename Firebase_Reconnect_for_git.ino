#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "*your ssid*"
#define WIFI_PASSWORD "*your password*"

/* 2. Define the Firebase project host name (required) */
#define FIREBASE_HOST "*your firebase host*"

/** 3. Define the API key
 * 
 * The API key can be obtained since you created the project and set up 
 * the Authentication in Firebase conssole.
 * 
 * You may need to enable the Identity provider at https://console.cloud.google.com/customer-identity/providers 
 * Select your project, click at ENABLE IDENTITY PLATFORM button.
 * The API key also available by click at the link APPLICATION SETUP DETAILS.
 * 
*/
#define API_KEY "*your API Key*" 


/* 4. Define the Firebase Data object */
FirebaseData fbdo;

/* 5. Define the FirebaseAuth data for authentication data */
FirebaseAuth auth;

/* 6. Define the FirebaseConfig data for config data */
FirebaseConfig config;

/* The function to print the operating results */
void printResult(FirebaseData &data);

/* The helper function to get the token status string */
String getTokenStatus(struct token_info_t info);

/* The helper function to get the token type string */
String getTokenType(struct token_info_t info);

/* The helper function to get the token error string */
String getTokenError(struct token_info_t info);

String path = "";
unsigned long dataMillis = 0;
int temp;
int c;
bool signupOK = false;


void setup()
{

    Serial.begin(115200);
    // initialize the button pin as a input:
    pinMode(buttonPin, INPUT);
    // initialize the LED as an output:
    pinMode(ledPin, OUTPUT);
  
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    /* Assign the project host and API key (required) */
    config.host = FIREBASE_HOST;
    config.api_key = API_KEY;

    Firebase.reconnectWiFi(true);

    Serial.println("------------------------------------");
    Serial.println("Sign up new user...");

    /** To sign in as anonymous user, just sign up as anonymous user
     * with blank email and password.
     * 
     * The Anonymous provider must be enabled.
     * 
     * To enable Anonymous provider,
     * from Firebase console, select Authentication, select Sign-in method tab, 
     * under the Sign-in providers list, enable Anonymous provider.
     * 
     * Warning: this will create anonymous user everytime you call this function 
     */
    if (Firebase.signUp(&config, &auth, "", ""))
    {
        Serial.println("Success");
        signupOK = true;
        c=1;

        /** if the database rules were set as in the example "Email_Password.ino"
         * This new user can be access the following location.
         * 
         * The new user UID can be taken from auth.token.uid
        */
        path = "*your path*";
        //path += auth.token.uid.c_str();
    }
    else
    {
        Serial.printf("Failed, %s\n", config.signer.signupError.message.c_str());
    }

    /** The id token (C++ string) will be available from config.signer.tokens.id_token
     * if the sig-up was successful. 
     * 
     * Now you can initialize the library using the internal created credentials.
     * 
     * If the sign-up was failed, the following function will initialize because 
     * the internal authentication credentials are not created.
    */
    Firebase.begin(&config, &auth);
    digitalWrite(ledPin, HIGH);
     Serial.println(WiFi.localIP());
  //The ESP8266 tries to reconnect automatically when the connection is lost
  //WiFi.setAutoReconnect(true);
  //WiFi.persistent(true);

}
 //modified
  //bollean

void loop()
{
  
    if (millis() - dataMillis > 500 && signupOK)
    {
        dataMillis = millis();
        
        // read the pushbutton input pin:
        buttonState = digitalRead(buttonPin);
      
        detectin();
        
        lastButtonState = buttonState;
    }
    //modified
  /*   if (WiFiReturns()) { 
        WiFi.isConnected();
        digitalWrite(WIFILED, HIGH);
        firebasereconnect();
    } else { 
        digitalWrite(WIFILED, LOW);
    }*/
    if (WiFiReturns() ) { 
    //Serial.println("Wifi Connection Reestablished");
    //Serial.println("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("firebase connect status ");
    Serial.println(c);
    //Serial.println();
    //if(!(Firebase.signUp(&config, &auth, "", "")))
    if(c!=1)
    { // replace this with your actual logic
        digitalWrite(ledPin, HIGH);
        Serial.println("Firebase Disconnected");
        Serial.println("Reestablish Firebase Connection");
        c=0;
        firebasereconnect();
    }
    else {
      c=1;
      }
   
}
 else { 
  Serial.println(WiFi.localIP());
       // digitalWrite(ledPin, LOW);
  Serial.println("Wifi or Firebase not connected");
    }
    //modified
}




/* The helper function to get the token type string */
String getTokenType(struct token_info_t info)
{
    switch (info.type)
    {
    case token_type_undefined:
        return "undefined";

    case token_type_legacy_token:
        return "legacy token";

    case token_type_id_token:
        return "id token";

    case token_type_custom_token:
        return "custom token";

    case token_type_oauth2_access_token:
        return "OAuth2.0 access token";

    default:
        break;
    }
    return "undefined";
}

/* The helper function to get the token status string */
String getTokenStatus(struct token_info_t info)
{
    switch (info.status)
    {
    case token_status_uninitialized:
        return "uninitialized";

    case token_status_on_signing:
        return "on signing";

    case token_status_on_request:
        return "on request";

    case token_status_on_refresh:
        return "on refreshing";

    case token_status_ready:
        return "ready";

    case token_status_error:
        return "error";

    default:
        break;
    }
    return "uninitialized";
}

/* The helper function to get the token error string */
String getTokenError(struct token_info_t info)
{
    String s = "code: ";
    s += String(info.error.code);
    s += ", message: ";
    s += info.error.message.c_str();
    return s;
}
bool WiFiReturns() 
 {
    if (WiFi.localIP() == IPAddress(0, 0, 0, 0))
        
        return 0;
    

    switch (WiFi.status()) {
        case WL_NO_SHIELD: return 0;
        case WL_IDLE_STATUS: return 0;
        case WL_NO_SSID_AVAIL: return 0;
        case WL_SCAN_COMPLETED: return 1;
        case WL_CONNECTED: return 1;
        case WL_CONNECT_FAILED: return 0;
        case WL_CONNECTION_LOST: return 0;
        case WL_DISCONNECTED: return 0;
        default: return 0;
    
    }
    
}
//modified
void firebasereconnect() {
    Serial.println("Trying to reconnect");
   Firebase.begin(FIREBASE_HOST, API_KEY);

}
//modified


/*
 *  I'm modified mobizt version for firebase
 *  by adding modified version
 */
