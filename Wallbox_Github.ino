#include "WiFi.h"
#include "HTTPClient.h"
#include "base64.h"
#include <ArduinoJson.h>
 
char ssid[] = "yourWiFi";         // your network SSID (name)
char password[] = "your Password"; // your network password
 
String authUsername = "wallbox user name";
String authPassword = "wallbox password";
StaticJsonDocument <3500> doc;
//DynamicJsonDocument doc (3500);
 
void setup() {
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");

  if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status
    HTTPClient http;
    http.begin("https://user-api.wall-box.com/users/signin");
    String auth = base64::encode(authUsername + ":" + authPassword);
    http.addHeader("Authorization", "Basic " + auth);
    http.addHeader("Partner","wallbox");
    int httpCode = http.GET(); 
        String payload = http.getString();
        Serial.print("Get token request HTTP Response code: ");
        Serial.println(httpCode);
        //Serial.println(payload);
        int str_len = payload.length() + 1; 
        char payloadArray[str_len];
        payload.toCharArray(payloadArray, str_len);
        DeserializationError error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);
        //Serial.println();
        JsonObject data = doc["data"];
        //const char* data_type = data["type"]; // "user_signin"
        //const char* data_id = data["id"];

        JsonObject data_attributes = data["attributes"];
        //const char* data_attributes_refresh_token = data_attributes["refresh_token"];
        //long long data_attributes_refresh_token_ttl = data_attributes["refresh_token_ttl"];
        const char* data_attributes_token = data_attributes["token"];
        //long long data_attributes_ttl = data_attributes["ttl"]; 
        //Serial.println(data_attributes_ttl);
        //const char* data_attributes_user_id = data_attributes["user_id"];
//********************************************************************************************************************************
//      Charger List 
//      use token to get access
//********************************************************************************************************************************
        String recv_token = String(data_attributes_token);
        recv_token = "Bearer " + recv_token;  // Adding "Bearer " before token
        http.begin("https://api.wall-box.com/v3/chargers/groups");
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        httpCode = http.GET();
        Serial.print("Get chargerList HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        //Serial.println(payload);
        str_len = payload.length() + 1; 
        payloadArray[str_len];
        payload.toCharArray(payloadArray, str_len);
        error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);Serial.println();   
        JsonObject result_groups_0 = doc["result"]["groups"][0];
        JsonObject result_groups_0_chargers_0 = result_groups_0["chargers"][0];
        const char* result_groups_0_chargers_0_name = result_groups_0_chargers_0["name"]; // "PulsarPlus SN ...
        Serial.println(result_groups_0_chargers_0_name);
        int result_groups_0_chargers_0_maxChargingCurrent = result_groups_0_chargers_0["maxChargingCurrent"];
        Serial.print("Max charging current = ");
        Serial.println(result_groups_0_chargers_0_maxChargingCurrent);
        long result_groups_0_chargers_0_id = result_groups_0_chargers_0["id"];
        Serial.print("Charger id = ");
        Serial.println(result_groups_0_chargers_0_id);
//********************************************************************************************************************************
//      Charger Status
//      use token to get access
//********************************************************************************************************************************
        // recv_token already received
        String chargerID = String (result_groups_0_chargers_0_id);
        http.begin("https://api.wall-box.com/chargers/status/" + chargerID );
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        httpCode = http.GET();
        Serial.print("Get chargerStatus HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        //Serial.println(payload);
        str_len = payload.length() + 1; 
        char payloadArray2[str_len];
        payload.toCharArray(payloadArray2, str_len);
        error = deserializeJson(doc, payloadArray2);
        //Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);
        JsonObject config_data = doc["config_data"];
        JsonObject config_data_software = config_data["software"];
        bool config_data_software_updateAvailable = config_data_software["updateAvailable"]; // false
        Serial.print("Software update available = ");
        Serial.println(config_data_software_updateAvailable ? "TRUE" : "FALSE");
        const char* config_data_software_currentVersion = config_data_software["currentVersion"]; // "5.13.17"
        Serial.print("Software version = ");
        Serial.println(config_data_software_currentVersion);
        int config_data_locked = config_data["locked"]; // 0
        Serial.print("Locked = ");
        Serial.println(config_data_locked ? "TRUE" : "FALSE");
        int charging_power = doc["charging_power"]; // 0
        Serial.print("Charging Power = ");
        Serial.println(charging_power);
//********************************************************************************************************************************
//      ChargerLock
//      use token to get access
//********************************************************************************************************************************
/*
        http.begin("https://api.wall-box.com/v2/charger/" + chargerID);
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        http.addHeader("Content-Type", "application/json");
        String httpRequestData = "{\"locked\":1}";
        httpCode = http.PUT(httpRequestData);
        Serial.print("Get chargerUnLock HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        //Serial.println(payload);
        str_len = payload.length() + 1; 
        payloadArray[str_len];
        payload.toCharArray(payloadArray, str_len);
        error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);Serial.println();
        JsonObject data_chargerData = doc["data"]["chargerData"];
        int data_chargerData_locked = data_chargerData["locked"];
        Serial.print("Locked status = ");
        Serial.println(data_chargerData_locked ? "TRUE" : "FALSE");   
        delay(2500);     
*/        
//********************************************************************************************************************************
//      ChargerUnLock
//      use token to get access
//********************************************************************************************************************************
        http.begin("https://api.wall-box.com/v2/charger/" + chargerID);
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        http.addHeader("Content-Type", "application/json");
        String httpRequestData = "{\"locked\":0}";
        httpCode = http.PUT(httpRequestData);
        Serial.print("Get chargerUnLock HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        //Serial.println(payload);
        str_len = payload.length() + 1; 
        payloadArray[str_len];
        payload.toCharArray(payloadArray, str_len);
        error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);Serial.println();
        JsonObject data_chargerData = doc["data"]["chargerData"];
        int data_chargerData_locked = data_chargerData["locked"];
        Serial.print("Locked status = ");
        Serial.println(data_chargerData_locked ? "TRUE" : "FALSE");
//********************************************************************************************************************************
//      maxChargingCurrent
//      use token to get access
//********************************************************************************************************************************
        http.begin("https://api.wall-box.com/v2/charger/" + chargerID);
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        http.addHeader("Content-Type", "application/json");
        httpRequestData = "{\"maxChargingCurrent\":10}";
        httpCode = http.PUT(httpRequestData);
        Serial.print("Get maxChargingCurrent HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        //Serial.println(payload);
        str_len = payload.length() + 1; 
        payloadArray[str_len];
        payload.toCharArray(payloadArray, str_len);
        error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        if (error) {
           Serial.print(F("deserializeJson() failed: "));
           Serial.println(error.f_str());
           return;
        }
        //serializeJsonPretty(doc, Serial);Serial.println();
        data_chargerData = doc["data"]["chargerData"];
        int data_chargerData_maxChargingCurrent = data_chargerData["maxChargingCurrent"];
        Serial.print("MaxChargingCurrent = ");
        Serial.println(data_chargerData_maxChargingCurrent);  
//********************************************************************************************************************************
//      getSessionList
//      use token to get access
//********************************************************************************************************************************
        http.begin("https://api.wall-box.com/v4/sessions/stats");
        http.addHeader("Authorization", recv_token); // Adding Bearer token as HTTP header
        httpCode = http.GET();
        Serial.print("Get getSessionsList HTTP Response code: ");
        Serial.println(httpCode);
        payload = http.getString();
        Serial.println(payload);
        //str_len = payload.length() + 1; 
        //payloadArray[str_len];
        //payload.toCharArray(payloadArray, str_len);
        //error = deserializeJson(doc, payloadArray);
        // Test if parsing succeeds.
        //if (error) {
         //  Serial.print(F("deserializeJson() failed: "));
        //   Serial.println(error.f_str());
        //   return;
        //}
        //serializeJsonPretty(doc, Serial);Serial.println();
        //data_chargerData = doc["data"]["chargerData"];
        //int data_chargerData_maxChargingCurrent = data_chargerData["maxChargingCurrent"];
        //Serial.print("MaxChargingCurrent = ");
        //Serial.println(data_chargerData_maxChargingCurrent);              
        http.end();
//**********************************************************************************************************************************
//********************************************************************************************************************************
//      pauseCharging Session
//      resumeCharging Session
//********************************************************************************************************************************
  }
}
 
void loop() {
  delay(10000);
}
