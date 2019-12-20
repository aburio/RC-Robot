#include <EmbAJAX.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>

// Motor shield control
int PWMA=5;//Right side 
int PWMB=4;//Left side 
int DA=0;//Right reverse 
int DB=2;//Left reverse

// Set up web server, and register it with EmbAJAX
DNSServer dnsServer;
EmbAJAXOutputDriverWebServerClass server(80);
EmbAJAXOutputDriver driver(&server);

// Define the main elements of interest as variables, so we can access to them later in our sketch.
EmbAJAXMomentaryButton forward("forward", "Avancer", 600);
EmbAJAXMomentaryButton backward("backward", "Reculer", 600);
EmbAJAXMomentaryButton left("left", "Gauche", 600);
EmbAJAXMomentaryButton right("right", "Droite", 600);

// Define a page (named "page") with our elements of interest, above, interspersed by some uninteresting
// static HTML. Note: MAKE_EmbAJAXPage is just a convenience macro around the EmbAJAXPage<>-class.
MAKE_EmbAJAXPage(page, "Cat Patrol", "<style>html, body { height: 100%; user-select: none; -webkit-user-select: none; -khtml-user-select: none; -moz-user-select: none; -ms-user-select: none; font-family: sans-serif;} canvas { background-color: grey; } button { width:100%; height: 100%; font-size: 1.3em; background-color: #0000FF; color: white; border: none; text-decoration: none; border-radius: .7em;} table, th, td {border: 0px solid black; } th, td { width:33%; height:12vh; padding-top: .7em; padding-left: .2em; padding-right: .2em; } .container{display: table; width: 100%; height: 55vh;} .wrapper{display: table-cell; height:100%; vertical-align: bottom;}</style><meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">",
  new EmbAJAXStatic("<h1 align=\"center\">CAT PATROL</h1><div class=\"container\"><div class=\"wrapper\"><table align=\"center\"><tr><td></td><td>"),
   &forward,
   new EmbAJAXStatic("</td><td></td></tr><tr><td>"),
  &left,
  new EmbAJAXStatic("</td><td></td><td>"),
  &right,
  new EmbAJAXStatic("</td></tr><tr><td></td><td>"),
  &backward,
  new EmbAJAXStatic("</td><td></td></tr></table></div></div>")
)

void motorControl() {
  if (forward.status() == 0) {
    digitalWrite(PWMA, HIGH); 
    digitalWrite(DA, LOW);   
    digitalWrite(PWMB, HIGH); 
    digitalWrite(DB, LOW);
  }
  else if (backward.status() == 0) {
    digitalWrite(PWMA, HIGH); 
    digitalWrite(DA, HIGH); 
    digitalWrite(PWMB, HIGH); 
    digitalWrite(DB, HIGH); 
  }
  else if (left.status() == 0) {
    digitalWrite(PWMA, HIGH); 
    digitalWrite(DA, HIGH); 
    digitalWrite(PWMB, HIGH); 
    digitalWrite(DB, LOW);
  }
  else if (right.status() == 0) {
    digitalWrite(PWMA, HIGH); 
    digitalWrite(DA, LOW);  
    digitalWrite(PWMB, HIGH); 
    digitalWrite(DB, HIGH);
  }
  else {
    digitalWrite(PWMA, LOW); 
    digitalWrite(DA, LOW); 
    digitalWrite(PWMB, LOW); 
    digitalWrite(DB, LOW);
  }
}

void setup() {
  pinMode(PWMA, OUTPUT); 
  pinMode(PWMB, OUTPUT); 
  pinMode(DA, OUTPUT); 
  pinMode(DB, OUTPUT); 

  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig (IPAddress (192,168,4,1), IPAddress (0,0,0,0), IPAddress (255,255,255,0));
  WiFi.softAP("Cat Patrol", "", 11, false, 1);

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(53, "*", WiFi.softAPIP());

  MDNS.begin("rc-robot");
  MDNS.addService("http", "tcp", 80);

  // Tell the server to serve our EmbAJAX test page on root
  // installPage() abstracts over the (trivial but not uniform) WebServer-specific instructions to do so
  driver.installPage(&page, "/", motorControl);
  server.begin();
}

void loop() {
  MDNS.update();
  dnsServer.processNextRequest();
  driver.loopHook();
}