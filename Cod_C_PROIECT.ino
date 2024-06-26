#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Setare I2C LCD la adresa 0x27 pentru un display de 16x2
LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte RANDURI = 4; // patru rânduri
const byte COLOANE = 4; // patru coloane

// Definirea layout-ului matricei de butoane conform specificațiilor tale
char keys[RANDURI][COLOANE] = {
  {'A', '3', '2', '1'},
  {'B', '6', '5', '4'},
  {'C', '9', '8', '7'},
  {'D', '#', '0', '*'}
};

byte PiniRANDURI[RANDURI] = {2, 3, 4, 5};    // pinii Arduino conectați la rândurile matricei
byte PiniColoane[COLOANE] = {6, 7, 8, 9};    // pinii Arduino conectați la coloanele matricei

// Inițializarea obiectului Keypad
Keypad keypad = Keypad(makeKeymap(keys), PiniRANDURI, PiniColoane, RANDURI, COLOANE);

const int pinLength = 4;  // Lungimea codului PIN
char CodPinCurent[pinLength + 1] = "1234"; // Codul PIN curent 
char CodPinIntrodus[pinLength + 1]; // Array pentru a stoca codul PIN introdus (plus un caracter null terminator)
int pinIndex = 0;  // Indexul pentru introducerea codului PIN


bool ModPIN = false; // Flag pentru a determina dacă suntem în modul de setare PIN
bool introducerePIN = false; // Flag pentru a indica dacă utilizatorul introduce codul PIN

int led_verde = 13;
int buzzer = 12;

Servo myServo;  // Cream un obiect servo
const int servoPin = 11; // Pinul la care este conectat servomotorul

void setup() {
  Serial.begin(9600);
  pinMode(10, INPUT_PULLUP); // buton pentru a intra în modul de setare PIN

  // Initializarea servomotorului
  myServo.attach(servoPin);
  myServo.write(0); // Pozitia initiala a servomotorului

  // Initializarea LCD-ului
  lcd.init();
  lcd.backlight();
  lcd.clear();

  pinMode(buzzer, OUTPUT);
  pinMode(led_verde, OUTPUT);
}



void loop() {

   digitalWrite(buzzer ,LOW);
   digitalWrite(led_verde, LOW);

  if (digitalRead(10) == HIGH) {  // Butonul de setare PIN este apăsat
    ModPIN = true;
    pinIndex = 0;  // Resetăm indexul pentru noul cod PIN
    lcd.clear();
    lcd.print("Introdu PIN nou:");
    Serial.println("Introdu PIN nou:");
    delay(500);  // Debounce
  }

  if (ModPIN) {
    setPinCode();
  } else {
  
   if (!introducerePIN) {
      introducerePIN = true;
      pinIndex = 0;
      lcd.clear();
      lcd.print("Introdu PIN:");
      Serial.println("Introdu PIN:");
     
    }
    checkPinCode();
  }

}




void setPinCode() {
  char key = keypad.getKey();

  if (key) {
    lcd.setCursor(pinIndex, 1);
    lcd.print(key);
    Serial.print(key);  // Afișăm fiecare apăsare de buton în monitorul serial

    if (pinIndex < pinLength) {
      CodPinCurent[pinIndex] = key;  // Salvăm apăsarea de buton în array-ul codului PIN
      pinIndex++;
    }

    if (pinIndex == pinLength) {  // Dacă am introdus toate caracterele codului PIN
      CodPinCurent[pinIndex] = '\0';  // Adăugăm caracterul null terminator

      // Verificăm dacă noul PIN setat este același cu cel anterior sau cu cel default
      if (strcmp(CodPinCurent, CodPinIntrodus) == 0) {
        lcd.clear();
        lcd.print("Noul PIN este identic");
        Serial.println("\nNoul PIN este identic");
        delay(2000);
        lcd.clear();
        lcd.print("Introdu PIN:");
        Serial.println("Introdu PIN:");
        pinIndex = 0;  // Resetăm indexul pentru introducerea PIN-ului
      } else {
        lcd.clear();
        lcd.print("Noul PIN setat:");
        Serial.println("\nNoul PIN setat:");
        delay(2000);
        lcd.clear();
        lcd.print(CodPinCurent);
        Serial.println(CodPinCurent);
        delay(2000);
        strcpy(CodPinIntrodus, CodPinCurent); // Salvăm noul PIN ca fiind PIN-ul anterior
        ModPIN = false;  // Ieșim din modul de setare PIN
        introducerePIN = true;  // Setăm flag-ul pentru introducerea PIN-ului
        pinIndex = 0;  // Resetăm indexul pentru introducerea PIN-ului
        lcd.clear();
        lcd.print("Introdu PIN:");  // Afișăm mesajul pentru introducerea PIN-ului
        Serial.println("Introdu PIN:");
      }
    }
  }
}




void checkPinCode() {
  char key = keypad.getKey();

  if (key) {
    lcd.setCursor(pinIndex, 1);
    lcd.print(key);
    Serial.print(key);  // Afișăm fiecare apăsare de buton în monitorul serial

    if (pinIndex < pinLength) {
      CodPinIntrodus[pinIndex] = key;  // Salvăm apăsarea de buton în array-ul codului PIN introdus
      pinIndex++;
    }

    if (pinIndex == pinLength) {  // Dacă am introdus toate caracterele codului PIN
      CodPinIntrodus[pinIndex] = '\0';  // Adăugăm caracterul null terminator

      if (strcmp(CodPinIntrodus, CodPinCurent) == 0) {
        lcd.clear();
        lcd.print("Acces permis");
        Serial.println("\nAcces permis");
        digitalWrite(led_verde, HIGH);
        Servoactivat();  // Acționăm servomotorul
      } else {
        lcd.clear();
        lcd.print("Acces interzis");
        Serial.println("\nAcces interzis");
        digitalWrite(led_verde, LOW); 
        Buzzeractivat();  // Acționăm buzzerul
      }
      delay(2000); // Afișăm mesajul pentru 2 secunde
      introducerePIN = false;  // Resetăm flag-ul pentru următoarea introducere
    }
  }
}



void Servoactivat() {
  myServo.write(90); // Rotim servomotorul la 90 de grade
  delay(2000); // Așteptăm 2 secunde
  myServo.write(0); // Readucem servomotorul în poziția inițială
}



void Buzzeractivat() {
  for(int i=0;i<8;i++){
  digitalWrite(buzzer, HIGH); // Pornim buzzerul
  delay(250); // Așteptăm 250 ms
  digitalWrite(buzzer, LOW);
  delay(250);
  } // Oprim buzzerul
}