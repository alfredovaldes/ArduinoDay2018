#include <SPI.h>
#include <Ethernet.h>
#include <DHT.h>

#define DHTPIN 5
#define DHTTYPE DHT11

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(192, 168, 1, 177);

// fill in your Domain Name Server address here:
IPAddress myDns(1, 1, 1, 1);

// initialize the library instance:
EthernetClient client;

//char server[] = "www.arduino.cc";
IPAddress server(104, 196, 17, 88);

unsigned long lastConnectionTime = 0;             // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10L * 1000L; // delay between updates, in milliseconds
// the "L" is needed to use long type numbers
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  // start serial port:
  Serial.begin(9600);
  dht.begin();

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // give the ethernet module time to boot up:
  delay(1000);
  // start the Ethernet connection using a fixed IP address and DNS server:
    if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for (;;)
      ;
  }
  // print the Ethernet board/shield's IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
}

void loop() {
  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if ten seconds have passed since your last connection,
  // then connect again and send data:
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }

}

// this method makes a HTTP connection to the server:
void httpRequest() {
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float hic = dht.computeHeatIndex(t, h, false);
  // if you get a connection, report back via serial:
  if (client.connect(server, 3000)) {
    Serial.println("connected");
    // Make a HTTP request:
    client.print("GET /savedata?device=Jardin2&temp=");
    client.print(t);
    client.print("&hum=");
    client.print(h);
    client.print("&hin=");
    client.print(hic);
    client.println(" HTTP/1.1");
    client.println("Host: 104.196.17.88");
    client.println("User-Agent: arduino-ethernet");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made:
    lastConnectionTime = millis();
  } else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
  }
}
