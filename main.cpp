#define FIREBASE_HOST "minhnode-1eacf-default-rtdb.firebaseio.com"
#define FIREBASE_SC "pEPj37MbbZQGWcXvDWzHQsmcGPkRmdC4qlbubWwq"
#include <Adafruit_Fingerprint.h>
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <FirebaseESP32.h>
#include <ESP_Mail_Client.h>

#include <ESP32Servo.h> // thu vien servo

#include <SPI.h>
#include <MFRC522.h>
FirebaseData firebaseData;
FirebaseJson json;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

char ssid[] = "minh";
char pass[] = "minh12345";


#define D1 32 // chưa set
#define D2 33
#define D3 25
#define D4 26 // chưa set
//#define D5 26
//#define D6 27
#define LED_PIN_FINGER 27
#define LED_PIN_RFID 14

#define SS_PIN    0  // ESP32 pin GPIO5 
#define RST_PIN   2 // ESP32 pin GPIO27 


#define SMTP_server "smtp.gmail.com"
#define SMTP_Port 465

#define sender_email "nhatminh10a20@gmail.com"
#define sender_password "zakcjeppsuxnzppj"   // App Password của bạn
#define Recipient_email "nhatminh10a20@gmail.com"  // Gửi cho chính bạn luôn
SMTPSession smtp;
ESP_Mail_Session session;    // Toàn cục: chỉ setup 1 lần
SMTP_Message message;        // Toàn cục: chỉ setup 1 lần

#define MODEM_RX 16
#define MODEM_TX 17
#define mySerial Serial2 // use for ESP32
bool CD1, CD2, CD3, CD4, CD5, CD6;
uint8_t FINGER_ADD[128];
bool present_toch;

MFRC522 rfid(SS_PIN, RST_PIN);  // RFID
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;
int check1,check2,check3 = 0;
uint8_t dem=0,dem1=0;
Servo myservo;  // SERVO
byte managerKeyUID[128][4]={
  {0xA1, 0xFE, 0x67, 0x1D}
};       // cho trước


void setup()  
{
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }
  

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected!");
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_SC;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
  Serial.println("Firebase Active");



  //RFID
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  

  myservo.attach(13);
  pinMode(LED_PIN_FINGER, OUTPUT);
  pinMode(LED_PIN_RFID, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);
  //pinMode(D5, OUTPUT);
  //pinMode(D6, OUTPUT);
  digitalWrite(LED_PIN_FINGER, LOW);
  digitalWrite(LED_PIN_RFID, LOW);
  digitalWrite(D3,HIGH);
  digitalWrite(D4,HIGH);
  finger.emptyDatabase();
   smtp.debug(1);

  // Setup SMTP session 1 lần duy nhất
  session.server.host_name = SMTP_server;
  session.server.port = SMTP_Port;
  session.login.email = sender_email;
  session.login.password = sender_password;
  session.login.user_domain = "";

  // Setup email message 1 lần duy nhất
  message.sender.name = "ESP32";
  message.sender.email = sender_email;
  message.subject = "ESP32 Cảnh Báo Xóm Trọ 1";
  message.addRecipient("Receiver", Recipient_email);

  String textMsg = "Có Người Đang Cố Xâm Nhập";
  message.text.content = textMsg.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;
}

void sendEmail() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  if (!smtp.connect(&session)) {
    Serial.println("Could not connect to SMTP server");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email: " + smtp.errorReason());
  } else {
    Serial.println("Email sent successfully!");
  }

  smtp.closeSession();
}



bool CHECK_THE(int id) {
  int dem2 = 0;

  while (dem2 < 3) {
    Serial.println("\nHÃY ĐẶT RFID VÀO");
    digitalWrite(LED_PIN_RFID,1);
   
    while (!rfid.PICC_IsNewCardPresent()) {
      delay(100);  
    }

    if (rfid.PICC_ReadCardSerial()) {
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      if (rfid.uid.uidByte[0] == managerKeyUID[id][0] &&
          rfid.uid.uidByte[1] == managerKeyUID[id][1] &&
          rfid.uid.uidByte[2] == managerKeyUID[id][2] &&
          rfid.uid.uidByte[3] == managerKeyUID[id][3]) {

        Serial.println("\nĐÃ DUYỆT");
        digitalWrite(LED_PIN_RFID,0);
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
        return true;
      } else {
        dem2++;
        digitalWrite(LED_PIN_RFID,0);
        delay(500);
        digitalWrite(LED_PIN_RFID,1);
        Serial.print("\nLẦN THỬ LẠI THỨ "); Serial.println(dem2);
        delay(2000);  // Chờ trước khi thử lại
        
      }

      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
    }
  }

  Serial.println("\nCOOK");
  digitalWrite(LED_PIN_RFID,0);
  return false;
}


bool ADD_THE(int id) {

  while (!rfid.PICC_IsNewCardPresent()) {
    delay(100);  
  }
    if (rfid.PICC_ReadCardSerial()) {
      // Lưu UID vào mảng
      managerKeyUID[id][0] = rfid.uid.uidByte[0];
      managerKeyUID[id][1] = rfid.uid.uidByte[1];
      managerKeyUID[id][2] = rfid.uid.uidByte[2];
      managerKeyUID[id][3] = rfid.uid.uidByte[3];

      Serial.print("\nĐã add id "); Serial.println(id);     
      rfid.PICC_HaltA();
      rfid.PCD_StopCrypto1();
     digitalWrite(LED_PIN_RFID,0);
      return true;
    }
  

  // Nếu không quét được thẻ
  Serial.println("\nCHƯA ADD - không có thẻ hoặc lỗi đọc thẻ");
  return false;
}


bool OPEN_DOOR(int id){
  if(!id)   // phan quyen
{
myservo.write(180);
  Serial.print("\nCửa đã mở");
  delay(5000);
  myservo.write(0);
  Serial.print("\nCửa đã đóng");
  return true;
  
}

  present_toch = CHECK_THE(id);
  if(present_toch){
  myservo.write(180);
  Serial.print("\nCửa đã mở");
  delay(5000);
  myservo.write(0);
  Serial.print("\nCửa đã đóng");
  return true;
                }
  return false;

}



uint8_t getFingerprintEnroll() {

  int p = -1;
  digitalWrite(LED_PIN_FINGER,1);
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("\nImage taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  digitalWrite(LED_PIN_FINGER,0);
  delay(200);
  digitalWrite(LED_PIN_FINGER,1);
  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("\nImage taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.print("Stored!");  // da luu van tay
    digitalWrite(LED_PIN_FINGER,0);
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
 
  } 
   digitalWrite(LED_PIN_RFID,1);
   ADD_THE(id);

}

uint8_t getFingerprintID() {
  Firebase.setInt(firebaseData, "/TOTAL_PEOPLE/IN", dem);
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("\nImage taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("\nNo finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK success!
  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    digitalWrite(LED_PIN_FINGER,1);
    delay(400);
    digitalWrite(LED_PIN_FINGER,0);
    Serial.println("Did not find a match");
    check3++;
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
 
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  check3 = 0;
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  check2 = OPEN_DOOR(finger.fingerID);
  if (dem < 0) dem = 0;
  bool check = false;
  if(check2){
  // Tìm và xóa phần tử
  for (int i = 0; i < dem; i++) {
      if (FINGER_ADD[i] == finger.fingerID) {
         
          for (int j = i; j < dem - 1; j++) {
              FINGER_ADD[j] = FINGER_ADD[j + 1];  
          }
          FINGER_ADD[dem - 1] = 0;  
          check = true;
          break;
      }
  }
  
  if (!check) {
    Firebase.setInt(firebaseData, "/IN_OUT/ID",finger.fingerID);
      if (dem < 128) { 
          FINGER_ADD[dem] = finger.fingerID;
          Firebase.setInt(firebaseData, "/IN_OUT/Trang_Thai",1);
         delay(50);
          dem++;
          Serial.print("\nIN");
          digitalWrite(D3,LOW);
          delay(3000);
          digitalWrite(D3,HIGH);
      } else {
          Serial.print("\nArray is full!");
      }
  } else {
      dem--;  // Giảm số phần tử khi đã xóa
      Firebase.setInt(firebaseData, "/IN_OUT/ID",finger.fingerID);
      Firebase.setInt(firebaseData, "/IN_OUT/Trang_Thai",0);
      Serial.print("\nOUT");
          digitalWrite(D4,LOW);
          delay(3000);
          digitalWrite(D4,HIGH);
  }
}

  
  return finger.fingerID;
}



void DELETE_FINGER(uint8_t id){
  uint8_t p = -1;
  p = finger.deleteModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Deleted!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error 7");
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not delete in that location");
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
  } else {
    Serial.print("Unknown error: 0x"); Serial.println(p, HEX);
  }

  // xoá RFID
  for(int i = 0; i < 4 ; i ++){
    managerKeyUID[id][i] = 0xFF; 
  }
}
void CHECK(){
  if (Firebase.getInt(firebaseData, "/FINGER_DATA/check1")) {
    int check1 = firebaseData.intData();
    if (check1 == 1) {
      if (Firebase.getInt(firebaseData, "/FINGER_DATA/ID1")) {
        id = firebaseData.intData();
        Serial.printf("thêm %d\n",id);
        getFingerprintEnroll() ;
        Firebase.setInt(firebaseData, "/FINGER_DATA/check1", 3);
      }
    } else if (check1 == 0) {
      if (Firebase.getInt(firebaseData, "/FINGER_DATA/ID1")) {
        id = firebaseData.intData();
        Serial.printf("xoá %d\n", id);
        DELETE_FINGER(id);
        Firebase.setInt(firebaseData, "/FINGER_DATA/check1", 3);
      }
        }
           }
}
void CHECK_DEVICE(){
 

if (Firebase.getInt(firebaseData, "/thietbi1/device1")) {
  CD1 = firebaseData.intData();
  
   digitalWrite(D1, CD1 == 1 ? LOW : HIGH);
}
if (Firebase.getInt(firebaseData, "/thietbi2/device2")) {
  CD2 = firebaseData.intData();
  digitalWrite(D2, CD2 == 1 ? LOW : HIGH); 
}
}

void CHECK_DOOR(){
 
if (Firebase.getInt(firebaseData, "/CUA_1/device1")) {
  int CHECK_DOOR = firebaseData.intData();
  if(CHECK_DOOR) OPEN_DOOR(0);
  Firebase.setInt(firebaseData, "/CUA_1/device1",0);
}


}
void loop() {
  CHECK_DEVICE(); 
  CHECK_DOOR();
  getFingerprintID();
  delay(50);
 
  if (Firebase.getInt(firebaseData, "/FINGER_DATA/check1")) {
    check1 = firebaseData.intData();
  }
 
  if (check1 != 3) {
    CHECK(); 
  } else {
    CHECK_DEVICE(); 
  }
  delay(300); 

   for(int i =0 ; i < dem ; i++){
     if(FINGER_ADD[i] != 0  && FINGER_ADD[i] > 0 ){ 
      Serial.print("\t");
      Serial.print(FINGER_ADD[i]);
         
     }
   }
   Serial.println(check3);
  if(check3 == 3){
    sendEmail();
    check3 = 0;
  }
}
