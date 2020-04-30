
//* libraries

#include <GSM.h>
#include <Adafruit_MAX31855.h>
#include <SdFat.h>
SdFat sd;
SdFile dataFile;
// Xively Client data
#define APIKEY         "fqpod5KGMFKnjM2c31o2idXGXtbEESKJzYwGVFMlcSEz3KMX"  // replace your xively api key here
#define FEEDID         626636250                     // replace your feed ID
#define USERAGENT      "OLM"              // user agent is the project name

// PIN Number
#define PINNUMBER ""

// APN data
#define GPRS_APN       "internet"  // replace your GPRS APN
#define GPRS_LOGIN     "guest"     // replace with your GPRS login
#define GPRS_PASSWORD  ""  // replace with your GPRS password

// initialize the library instance:
GSMClient client;
GPRS gprs;
GSM gsmAccess = true;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
// IPAddress server(216,52,233,121);    // numeric IP for api.xively.com
char server[] = "api.xively.com";      // name address for xively API

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
boolean lastConnected = false;                  // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000;  //delay between updates to Xively.com

//>define Chip Control

int sdCS = 4;
int thermoCS = 5;
int gsmCS = 10;
//>define other Thermocouple integers
int thermoDO = 8;
int thermoCLK = 6;

Adafruit_MAX31855 thermocouple(thermoCLK, thermoCS, thermoDO);




/* Startup routine */
void setup()
{
  
//>-----------------------------------------------
 
// Serial output
  Serial.begin(9600);
  Serial.println("Arduino Powered On");
  
//>-----------------------------------------------

// Enable GSM shield

Serial.println("Initialize GSM Shield...");

  // connection state
  boolean notConnected = true;
  
  // After starting the modem with GSM.begin()
  // attach the shield to the GPRS network with the APN, login and password
  while(notConnected)
  {
    if((gsmAccess.begin(PINNUMBER)==GSM_READY) &
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD)==GPRS_READY))
      notConnected = false;
    else
    {
      Serial.println("Not connected");
      delay(1000);
    }
  }
  
Serial.println("Initialize GSM Shield... DONE!");
  
}



/* Main program loop */
void loop()
{  
  // read the analog sensor:
  Serial.println("In MAIN LOOP...");
  

  double sensorReading = thermocouple.readCelsius(); //int sensorReading = analogRead(A0); 
  
 Serial.println(sensorReading);  


  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available())
  {
     char c = client.read();
     Serial.print(c);
  }

  // if there's no net connection, but there was one last time
  // through the loop, then stop the client:
  if (!client.connected() && lastConnected)
  {
    client.stop();
  }
  
  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && ((millis() - lastConnectionTime) > postingInterval))
  {
  sendData(sensorReading);
  }
  
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

/*
  This method makes a HTTP connection to the server.
*/
void sendData(int thisData)
{
  // if there's a successful connection:
  if (client.connect(server, 80))
  {
    Serial.println("connecting...");
    
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.xively.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(thisData);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();
    
    // here's the actual content of the PUT request:
    client.print("sensor1,");
    client.println(thisData);
  } 
  else
  {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
  // note the time that the connection was made or attempted
  lastConnectionTime = millis();
}

/*
  This method calculates the number of digits in the
  sensor reading.  Since each digit of the ASCII decimal
  representation is a byte, the number of digits equals
  the number of bytes.
*/
int getLength(int someValue)
{
  // there's at least one byte:
  int digits = 1;
  
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0)
  {
    dividend = dividend /10;
    digits++;
  }
  
  // return the number of digits:
  return digits;
}

  
