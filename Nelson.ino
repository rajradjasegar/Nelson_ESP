#include <Servo.h>
#include <ESP8266WiFi.h>

// initialisation des constantes spécifiques à votre installation 
const String nelsonName = "Shigaraki";
const char* ssid = "SFR_A5D8";
const char* password = "llankmatiannalb8alei";
// valeurs pour le serveur Web
const char* host     = "http://192.168.1.65";
//const char* nelson     = "Shigaraki"; 

String keyword = String("\"position\":"); //chaîne que l'on recherche dans le JSON
 
Servo myservo;  // création d'un objet myservo issu de la librairie Servo

// déclaration d'une variable qui stockera la position (= l'angle) du servo
int pos = 0 ;

// drapeau indiquant pendant l'analyse de la réponse du serveur
// si on est dans l'en-tête HTTP (false) ou dans le contenu de 
// la ressource.
bool inBody = false;

void setup() 
{
  // initialisation du port série
  Serial.begin(115200);
  
  // on affiche le wifi sur lequel on veut se connecter
  Serial.print("Connexion au WiFi ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);  // on se connecte
  
  while (WiFi.status() != WL_CONNECTED) { // on attend d'être connecté
    delay(500);
    Serial.print(".");
  }

  Serial.println("");  // on affiche les paramètres 
  Serial.println("WiFi connecte");  
  Serial.print("Adresse IP du module ESP: ");  
  Serial.println(WiFi.localIP());
  Serial.print("Adresse IP de la box : ");
  Serial.println(WiFi.gatewayIP());
} 
 
void loop()
{
  Serial.print("connexion au serveur : ");
  Serial.println(host);
  
  // On se place dans le rôle du  client en utilisant WifiClient
  WiFiClient client;

  // le serveur Web attend sur le port 3000
  const int httpPort = 80;

  // Si la connexion échoue cela sera pour la prochaine fois
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = String("/nelsons/") + nelsonName;
  
  Serial.print("demande URL: ");
  Serial.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

  // On attend 1 seconde
  delay(10000);
  
  inBody = false; // on est dans l'en-tête
  
  // On lit les données reçues, s'il y en a
  while(client.available()){
    String line = client.readStringUntil('\r');
        
    if (line.length() == 1) inBody = true; /* passer l'en-tête jusqu'à une ligne vide */
    if (inBody) {  // ligne du corps du message, on cherche le mot clé
       int pos = line.indexOf(keyword);

      if (pos > 0) { /* mot clé trouvé */
       // indexOf donne la position du début du mot-clé, en ajoutant sa longueur
       // on se place à la fin.
       pos += keyword.length(); 

       Serial.println (&line[pos]);

       /*deg = atof(&line[pos]);*/ 

     } /* fin récupération du flottant */
    } /* fin de la recherche du mot clé */
  } /* fin data available */
  /*
  Serial.println();Serial.print ("Direction = ");
  Serial.println(deg); // wind.deg : Wind direction, degrees (meteorological)

  // si la valeur est > 180 alors on soustrait 179
  if(deg > 180)
    deg = deg -179;
    
  // et on l'envoie dans le servo :
  myservo.attach(D1);
  myservo.write(deg);

  Serial.println("connexion fermee");

  delay(10 * 1000); // 10 secondes*/
  
}
