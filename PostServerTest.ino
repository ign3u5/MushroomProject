#include <ArduinoComponents.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef STASSID
#define STASSID "Post Server"
#define STAPSK  "your-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;
int ledPin = 2;

class WebPage
{
private:
  components::Vector<String> _domComponents;
  String _productWebPage;
public:
  WebPage() { }
  void AddDomComponent(String domComponent)
  {
    _domComponents.push(domComponent);
  }
  String RenderPage()
  {
    for ( int a = 0; a < _domComponents.size(); a++)
    {
      _productWebPage += _domComponents.get(a);
    }
    return _productWebPage;
  }
};



class MainPageBuilder
{
private:
  WebPage MainPage = WebPage();
public:
  String Build()
  {
    String domHead = R"###(
            <html>
            <head>
           <meta name = 'viewport' content = 'width=device-width, initial-scale=1.0'>
           <title>ESP8266 Web Server POST handling</title>
          <style>
           body{ background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }
           </style>
           </head>
            )###";
    String domBody = R"###(
    <body>
           <h1>Update Settings</h1><br>
         <form method = 'post' enctype = 'application/x-www-form-urlencoded' action = '/postform/'>
           <label>Temperature:</label>
          <input type = 'text' name = 'Temperature' value = 'world'><br>
         <label>Temperature Precision(+/ -) : </label>
        <input type = 'text' name = 'Temperature Precision' value = 'world'><br>
           <label>Humidity : </label>
           <input type = 'text' name = 'Humidity' value = 'world'><br>
            <label>Humidity Precision(+/ -) : </label>
           <input type = 'text' name = 'Humidity Precision' value = 'world'><br>
          <input type = 'submit' value = 'Submit'>
          </form>
          </body>
            </html>
            )###";

    MainPage.AddDomComponent(domHead);
    MainPage.AddDomComponent(domBody);
    return MainPage.RenderPage();
  }

};

String postForms = "";

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", postForms);
  digitalWrite(led, 0);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "POST form was:\n";
    for (uint8_t i = 0; i < server.args(); i++) {
      message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
  if(server.arg(0) == "on")
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("Handle Form HIGH");
  }
  if(server.arg(0) == "off")
  {
    digitalWrite(ledPin, LOW);
    Serial.println("Handle Form LOW");
  }
  Serial.println("Handle Form");
  server.send(200, "text/plain", message);
  digitalWrite(led, 0);
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  if(server.arg(0) == "on")
  {
    digitalWrite(ledPin, HIGH);
    Serial.println("Handle Not Found HIGH");
  } 
  if(server.arg(0) == "off")
  {
    digitalWrite(ledPin, LOW);
    Serial.println("Handle Not Found LOW");
  }
  Serial.println("Handle Not Found");
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  MainPageBuilder mainPageBuilder = MainPageBuilder();
  postForms = mainPageBuilder.Build();
  delay(1000);
  Serial.begin(115200);
  
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  //WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.println("IP address: ");
  Serial.print(myIP);

  server.on("/", handleRoot);

  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
