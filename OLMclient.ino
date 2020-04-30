#include "WiFly.h"
#include "Credentials.h"

Timer t;
long lastUpdate=0;

byte server[] = { 106,187,94,198 }; // artiswrong.com

Client client("artiswrong.com", 80);

void setup() {
  
  Serial.begin(115200);

  WiFly.begin();
  
  if (!WiFly.join(ssid, passphrase)) {
    Serial.println("Association failed.");
    while (1) {
      // Hang on failure.
    }
  }  

}

void post()
{
  Serial.println("connecting...");
  String PostData="sample={\"fittingId\":1,";
  unsigned char i;
  for(i=0;i<6;i++)
  {
    PostData=PostData+"\"channel-";
    PostData=String(PostData+i);
    PostData=PostData+"\":";
    PostData=String(PostData + String(analogRead(i)));
    if(i!=5)
      PostData=PostData+",";
  }
    PostData=PostData+"}";  
  Serial.println(PostData);
  if (client.connect()) {
    Serial.println("connected");
  client.println("POST /tinyFittings/index.php HTTP/1.1");
  client.println("Host:  artiswrong.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded;");
  client.print("Content-Length: ");
  client.println(PostData.length());
  client.println();
  client.println(PostData);
  } else {
    Serial.println("connection failed");
  }
}


void loop() {
    post();
    delay(500);
}

