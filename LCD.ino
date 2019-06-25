#include <SPI.h>                  // Suporte dos pinos do RFID,
#include <MFRC522.h>              // Biblioteca do RFID.
#include <Wire.h>                 // Supore dos pinos do Display,
#include <LiquidCrystal_I2C.h>    // Biblioteca do Display.

// Definindo algumas portas do Arduino para o RFID, a porta 9 para o reset, por exemplo. 
#define SS_PIN 10
#define RST_PIN 9

// Criando uma instância MFRC522 que é a do RFID.
MFRC522 mfrc522(SS_PIN, RST_PIN);   

// Inicializa o display no endereco 0x27.
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

char st[20];
 
void setup() 
{
  Serial.begin(9600);   // Inicia a serial do Arduino.
  SPI.begin();          // Inicia  SPI bus.
  mfrc522.PCD_Init();   // Inicia MFRC522.
  Serial.println("Aproxime o seu cartao do leitor...");
  Serial.println();
  lcd.begin (16,2);     //Display de 16 colunas e 2 Linhas 
  mensageminicial();

  pinMode(6, OUTPUT);   // Led Verde
  pinMode(8, OUTPUT);   // Led Branco
  pinMode(7, OUTPUT);   // Led Vermelho

  pinMode(2, OUTPUT);   // Irá para o NodeMCU
  pinMode(3, INPUT);

  digitalRead(3) == LOW;
}
 
void loop() 
{
  // Procura o cartão.
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Seleciona o cartão
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Mostra UID do cartão na serial.
  Serial.print("UID da tag :");
  String conteudo= "";
  byte letra;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     conteudo.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     conteudo.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Mensagem : ");
  conteudo.toUpperCase();

  // Se for este cartão, então o acesso será permitido.
  if (conteudo.substring(1) == "C6 25 F6 93") //UID 1 - Chaveiro
  {
    Serial.println("Ola USUÁRIO !");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ola USUARIO !");
    lcd.setCursor(0,1);
    lcd.print("Acesso liberado!");
    digitalWrite(6, HIGH);
    digitalWrite(8, LOW);
    digitalWrite(7, LOW);
    // Pisca o LCD
    delay(1000);
    lcd.setBacklight(LOW);
    delay(400);
    mensageminicial();
  }

  // Caso contrário, irá negar o acesso.
  else
  {
    digitalWrite(2, HIGH);
    Serial.println("Ola DESCONHECIDO !");
    Serial.println();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Ola DESCONHECIDO !");
    lcd.setCursor(0,1);
    lcd.print("Acesso Negado !");
    digitalWrite(7, HIGH);
    digitalWrite(8, LOW);
    digitalWrite(6, LOW);
    delay(5000);
    digitalWrite(2, LOW);
    delay(10000);
    
    
    if(digitalRead(3) == HIGH){
      delay(3000);
      lcd.setCursor(0,1);
      lcd.print("Acesso permitido!");
      digitalWrite(6, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(7, LOW);
    }
    else if(digitalRead(3) == LOW){
      delay(3000);
      lcd.setCursor(0,1);
      lcd.print("Acesso recusado");
      digitalWrite(7, HIGH);
      digitalWrite(8, LOW);
      digitalWrite(6, LOW);
    }
    else{
      // Pisca o LCD.
      delay(3000);
      lcd.setBacklight(LOW);
      delay(400);
      mensageminicial();
    }
    
    // Pisca o LCD.
    delay(3000);
    lcd.setBacklight(LOW);
    delay(400);
    mensageminicial();
  }
} 

// Função que mostra a mensagem inicial do display e organiza os leds.
void mensageminicial()
{
  lcd.setBacklight(HIGH);
  lcd.setCursor(0,0);
  lcd.clear();
  lcd.print(" Aproxime o seu");  
  lcd.setCursor(0,1);
  lcd.print("cartao do leitor");
  digitalWrite(8, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);  
}

/* FONTES DE PESQUISA
 *  https://www.arduinoecia.com.br/2014/12/modulo-i2c-display-16x2-arduino.html
 *  https://www.filipeflop.com/blog/controle-acesso-leitor-rfid-arduino/
 */
