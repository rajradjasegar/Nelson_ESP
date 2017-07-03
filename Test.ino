#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;

#include <Servo.h>

// création de l'objet servo issu du moule Servo
Servo myservo;

// déclaration d'une variable payload dont on va se servir pour
// l'envoi et la réception de nos messages
String payload;

// initialisation des constantes spécifiques à votre installation 
const String nelsonName = "Shigaraki";
const char* ssid = "SFR_A5D8";
const char* password = "llankmatiannalb8alei";

// initialise deux variables entières
// pos contient la valeur actuelle de notre servo
int pos = 0;
// lastPos contient la valeur de notre servo lors du dernier tour de boucle
int lastPos = 0;

// tolerance permet d'avoir une approximation du feeback entre deux tours
// de boucle
int tolerance = 2;

void setup() {
  // on souhaite communiquer avec l'ordinateur
  Serial.begin(9600);
  //Serial.setDebugOutput(true);

  Serial.println("Nelson PATCH starting...");

  for(uint8_t t = 4; t > 0; t--) {
      Serial.printf("[SETUP] WAIT %d...\n", t);
      Serial.flush();
      delay(1000);
  }

  // on se connecte au Wifi
  WiFiMulti.addAP(ssid, password);

}

void loop() {
  // on regarde si on est bien connecté au point d'accès wifi
  if((WiFiMulti.run() == WL_CONNECTED)) {
    // création d'un objet appelé http réutilisable et sorti du moule HTTPClient
    HTTPClient http;
    
    Serial.println("[HTTP] begin...");
    String uri = "/nelsons/" + nelsonName + "/";
    http.begin("192.168.1.65", 80, uri);

    Serial.println("[HTTP] GET...");    
    // démarrer la connexion et envoyer les entêtes HTTP
    http.addHeader("Content-type", "application/json");
    int httpCode = http.GET();
    
    // httpCode sera négatif si on rencontre une erreur
    if(httpCode > 0) {
      // les entêtes HTTP ont été envoyés et
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      
      // si le serveur NodeJS répond par OK
      if(httpCode == HTTP_CODE_OK) {
        delay(1000);
        // alors on récupère la réponse sous forme de chaîne de caractères
        String payload = http.getString();
        Serial.println("[HTTP] GET... payload:");
        Serial.println(payload);

        int start = payload.indexOf("\"position\":");
        int end = payload.indexOf('}', start + 1 );
        String positionString = "";
        for (int i= start + 12; i < end; i++)
          positionString += payload.charAt(i);
        
        // récupère la valeur de tom et la stocke dans pos 
        pos = positionString.toInt();

        Serial.println(pos);
        //Serial.println(positionString);
        // nous écrivons ensuite cette valeur sur le servo
        myservo.attach(D1);
        myservo.write(pos);
        delay(1000);
        myservo.detach();
      }
    }
    else
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());

    http.end();

    /*// et nous n'oublions pas de relâcher le servo afin de pouvoir le manipuler
    int pos = map(analogRead(A0), 0, 1023, 0, 179);

    // on attend trois secondes avant la prochaine requête
    delay(3000);
    
    // on regarde à nouveau si le servo n'a pas bougé
    int lastPos = map(analogRead(A0), 0, 1023, 0, 179);
    
    Serial.print("pos:");
    Serial.println(pos);
    Serial.print("last:");
    Serial.println(lastPos);

    if(abs(pos - lastPos) > tolerance) {
      Serial.println("[HTTP] PATCH...");
      
      String payload = "{\"position\":";
      payload += String(lastPos);
      payload += "}";
  
      Serial.print("[HTTP] PATCH... payload:");
      Serial.println(payload);
      
      // démarrer la connexion et envoyer les entêtes HTTP
      http.addHeader("Content-type", "application/json");
      httpCode = http.sendRequest("PATCH", (uint8_t *) payload.c_str(), payload.length());
      
      // httpCode sera négatif si on rencontre une erreur
      if(httpCode > 0) {
        // les entêtes HTTP ont été envoyés et
        Serial.printf("[HTTP] PATCH... code: %d\n", httpCode);
        
        // si le serveur TOM répond par OK
        if(httpCode == HTTP_CODE_OK)
          Serial.println("[HTTP] PATCH... OK!");
      }
      else
        Serial.printf("[HTTP] PATCH... failed, error: %s\n", http.errorToString(httpCode).c_str());
      
      http.end();*/
    /*}*/
  }
}
