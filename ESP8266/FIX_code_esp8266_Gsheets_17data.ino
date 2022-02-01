// Example Arduino/ESP8266 code to upload data to Google Sheets
// Follow setup instructions found here:
// https://github.com/StorageB/Google-Sheets-Logging
// reddit: u/StorageB107
// email: StorageUnitB@gmail.com


#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <HTTPSRedirect.h>

// Enter network credentials:
//"Edewe";//
//"kepooooo";//
const char* ssid     = "Edewe";
const char* password = "kepooooo";

// Enter Google Script Deployment ID:
const char *GScriptId = "AKfycbxTR0bgwScq96-SBjL4U2nPx5DVbRRKPjEJyNfhmdRDlfQNXuzpTIE5s3UkH7OqEuBZ";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet2\", \"values\": ";
String payload = "";

String dataIn;
String dt[1000];
String KR;
int i;
boolean parsing=false;

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

// Declare variables that will be published to Google Sheets
int z = 0;
int retval;

void setup() {

  Serial.begin(115200);        
  delay(100);
//  Serial.println('\n');
}
void wifi(){
  // Connect to WiFi            
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password); 
  while (WiFi.status() != WL_CONNECTED) {
    delay(8000);
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  Serial.print("Connected to "); 
  Serial.println(ssid); 
//  Serial.print("IP address:\t");
//  Serial.println(WiFi.localIP());
  spreadsheet();
}

void spreadsheet(){
    // Use HTTPSRedirect class to create a new TLS connection
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
    
    Serial.print("Connecting to ");
    Serial.println(host);
  
    // Try to connect for a maximum of 5 times
    bool flag = false;
    for (int i=0; i<5; i++){ 
      retval = client->connect(host, httpsPort);
      if (retval == 1){
         flag = true;
         Serial.print("Connected to ");
         Serial.println(host);
         z=1;
         break;
      }
      else;
        Serial.println("Connection failed. Retrying...");
        z=0;
    }
    if (!flag){
      Serial.print("Could not connect to server: ");
      Serial.println(host);
      WiFi.mode(WIFI_OFF);
      return;
    }
    delete client;    // delete HTTPSRedirect object
    client = nullptr; // delete HTTPSRedirect object
}


void loop() {
   while((WiFi.status() == WL_CONNECTED)){//ini kalo KONEK ke INTERNET
       if (Serial.available()<1){
          Serial.println("No Data");
          delay(4000); 
       }  
       if (Serial.available()>0){
          char inChar = (char)Serial.read();
          dataIn += inChar;
          if (inChar == '\n') {parsing = true;}
         //  Serial.write(Serial.read());
       }
       if(parsing){
         parsingData();
         parsing=false;
         dataIn="";
         Serial.print(dataIn);
         return;
       }
     }
     wifi();
}

      
      //delay(900000);//15 menit  
void parsingData()
{
      delay(300);
      int j=0;
      //kirim data yang telah diterima sebelumnya
      Serial.println("");
      Serial.print("Data Masuk : ");
      Serial.print(dataIn);
      //inisialisasi variabel, (reset isi variabel)
      dt[j]="";
      //proses parsing data
      for(i=1;i<dataIn.length();i++)
      {
             //pengecekan tiap karakter dengan karakter (#) dan (,)
             if ((dataIn[i] == '#') || (dataIn[i] == ','))
             {
                   //increment variabel j, digunakan untuk merubah index array penampung
                   j++;
                  dt[j]=""; //inisialisasi variabel array dt[j]
             }
             else
             {
                  //proses tampung data saat pengecekan karakter selesai.
                  dt[j] = dt[j] + dataIn[i];
             }
      }
      //kirim data hasil parsing
      Serial.print("Rata-Rata Kelembaban Tanah : ");
      Serial.println(dt[0]);
      Serial.print("Rata-Rata Kelembaban Udara : ");
      Serial.println(dt[1]);
      Serial.print("Rata-Rata Suhu Udara       : ");
      Serial.println(dt[2]);
      Serial.print("Kondisi Relay              : ");
      Serial.println(dt[3]);
      Serial.print("Kondisi Relay 2            : ");
      Serial.println(dt[4]);
      Serial.print("Nilai Relay 1 (Sugeno)     : ");
      Serial.println(dt[5]);
      Serial.print("Nilai Relay 2 (Sugeno)     : ");
      Serial.println(dt[6]);
      Serial.print("Kadar Air Tanah Kiri(%)    : ");
      Serial.println(dt[7]);
      Serial.print("Kadar Air Tanah Kanan(%)   : ");
      Serial.println(dt[8]);
      Serial.print("Kelembaban Udara Kiri(%)   : ");
      Serial.println(dt[9]);
      Serial.print("Kelembaban Udara Kanan(%)  : ");
      Serial.println(dt[10]);
      Serial.print("Suhu Udara Kiri(%)         : ");
      Serial.println(dt[11]);
      Serial.print("Suhu Udara Kanan(%)        : ");
      Serial.println(dt[12]);
      Serial.print("Kelembaban Udara Kiri(REAL) : ");
      Serial.println(dt[13]);
      Serial.print("Kelembaban Udara Kanan(REAL): ");
      Serial.println(dt[14]);
      Serial.print("Suhu Udara Kiri(REAL)       : ");
      Serial.println(dt[15]);
      Serial.print("Suhu Udara Kanan(REAL)      : ");
      Serial.println(dt[16]);

      static bool flag = false;
          if (!flag){
            client = new HTTPSRedirect(httpsPort);
            client->setInsecure();
            flag = true;
            client->setPrintResponseBody(true);
            client->setContentTypeHeader("application/json");
          }
          if (client != nullptr){
            if (!client->connected()){
              client->connect(host, httpsPort);
            }
          }
          else{
        //    Serial.println("Error creating client object!");
          }
          
        //  // Create json object string to send to Google Sheets
          payload = payload_base + "\"" + dt[0] + "," + dt[1] + "," + dt[2] + "," + dt[3] + "," + dt[4] + "," + dt[5] + "," + dt[6] + "," + dt[7] + "," + dt[8] + "," + dt[9] + "," + dt[10] + "," + dt[11] + "," + dt[12] + "," + dt[13] + "," + dt[14] + "," + dt[15] + "," + dt[16] + "\"}";
          
          // Publish data to Google Sheets
          Serial.println("Publishing data...");
          Serial.println(payload);
//          if (WiFi.status() != WL_CONNECTED) {
//            Serial.print("Connecting to ");
//            Serial.println(ssid);
//            delay(3000);
//          }
          if(client->POST(url, host, payload)){ 
          // do stuff here if publish was successful
          }
          else{
            // do stuff here if publish was not successful
            Serial.println("Error while connecting");
          }
}
