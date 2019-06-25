#ifndef FLOATTOSTRING
#define FLOATTOSTRING

/*
 *  floatToString.h
 *
 *  Usage: floatToString(buffer string, float value, precision, minimum text width)
 *
 *  Example:
 *  char test[20];    // string buffer
 *  float M;          // float variable to be converted
 *                 // precision -> number of decimal places
 *                 // min text width -> character output width, 0 = no right justify
 *
 *  Serial.print(floatToString(test, M, 3, 7)); // call for conversion function
 *  
 */
char * floatToString(char * outstr, float val, byte precision, byte widthp){
  char temp[16]; //increase this if you need more digits than 15
  byte i;

  temp[0]='\0';
  outstr[0]='\0';

  if(val < 0.0){
    strcpy(outstr,"-\0");  //print "-" sign
    val *= -1;
  }

  if( precision == 0) {
    strcat(outstr, ltoa(round(val),temp,10));  //prints the int part
  }
  else {
    unsigned long frac, mult = 1;
    byte padding = precision-1;
    
    while (precision--)
      mult *= 10;

    val += 0.5/(float)mult;      // compute rounding factor
    
    strcat(outstr, ltoa(floor(val),temp,10));  //prints the integer part without rounding
    strcat(outstr, ".\0"); // print the decimal point

    frac = (val - floor(val)) * mult;

    unsigned long frac1 = frac;

    while(frac1 /= 10)
      padding--;

    while(padding--)
      strcat(outstr,"0\0");    // print padding zeros

    strcat(outstr,ltoa(frac,temp,10));  // print fraction part
  }

  // generate width space padding
  if ((widthp != 0)&&(widthp >= strlen(outstr))){
    byte J=0;
    J = widthp - strlen(outstr);

    for (i=0; i< J; i++) {
      temp[i] = ' ';
    }

    temp[i++] = '\0';
    strcat(temp,outstr);
    strcpy(outstr,temp);
  }

  return outstr;
}
#endif
/*--------------------------------------------------------------------------------------------------------------------------------------------*/
#include <DHT.h>  // Including library for dht
#include <ESP8266WiFi.h>
#include "CTBot.h"


#define TEMPERATURA  "Temperatura"  
#define UMIDADE  "Umidade"
#define PERMISSAOSIM  "Sim"  
#define PERMISSAONAO  "Nao"
#define DHTPIN 0          //<-- pin onde o dht11 estÃ¡ conectado


CTBot myBot;
CTBotInlineKeyboard myKbd;
CTBotInlineKeyboard myKbd2;  
DHT dht(DHTPIN, DHT11);

String ssid = "brisa-307605";   //Login do Wifi
String pass = "fh84nbpc";       //Senha do Wifi
String token = "783990857:AAGIYcIICT1Ny4YEDAk7gzWmVyv-CM6nsEY";   // Token do Telegram.
 
void setup() 
{
  Serial.begin(115200);
  delay(10);
  dht.begin();

  // Conecta o ESP8266 ao Wifi.
  myBot.wifiConnect(ssid, pass);

  // Conecta o token do Telegram.
  myBot.setTelegramToken(token);

  // Checa se tá tudo certo.
  if (myBot.testConnection())
    Serial.println("\nTeste de Conecção: OK");
  else
    Serial.println("\nTeste de Conecção: NOK");

  // Seta os pinos do Arduino.
  pinMode(D1, INPUT);     //Entrada
  pinMode(D2, OUTPUT);    //Saida
  
  // Customização dos botões no Telegram.
  myKbd.addButton("Temperatura", TEMPERATURA, CTBotKeyboardButtonQuery); // Adiciona um botão escrito 'SIM'.
  myKbd.addButton("Umidade", UMIDADE, CTBotKeyboardButtonQuery); // Adiciona um botão escrito 'NÃO'.
  myKbd2.addButton("Sim", PERMISSAOSIM, CTBotKeyboardButtonQuery); // Adiciona um botão escrito 'SIM'.
  myKbd2.addButton("Não", PERMISSAONAO, CTBotKeyboardButtonQuery); // Adiciona um botão escrito 'NÃO'.
}
 
void loop() 
{
  int arduino = digitalRead(D1);
  int cont = 0;

  TBMessage msg;
  
  if (arduino == 1) {
    digitalWrite(D2, LOW);
    String porta;
      porta = (String)"Senhor, há alguém tentando abrir a porta.";
      myBot.sendMessage(msg.sender.id, porta);
      myBot.sendMessage(msg.sender.id, "Deseja permitir?", myKbd2);
  }



  

  if (myBot.getNewMessage(msg)){
    if (msg.messageType == CTBotMessageText) {
      if (msg.text.equalsIgnoreCase("Menu")) {
         myBot.sendMessage(msg.sender.id, "O que você deseja saber?", myKbd);
       }
      else{
        myBot.sendMessage(msg.sender.id, "Olá, eu sou seu assistente virtual. Por favor, digite 'Menu' para abrir as opções.");
      }
    }
    else if (msg.messageType == CTBotMessageQuery){
      if (msg.callbackQueryData.equals(TEMPERATURA)){
        float t = dht.readTemperature();
        if (isnan(t)) {
          myBot.endQuery(msg.callbackQueryID, "Falhou a comunicação com o sensor, tente novamente.", true);
        }
        else{
          char T[20];
          String TEMP;
          TEMP = floatToString(T, t, 2, 7) + (String)" °C";
          myBot.sendMessage(msg.sender.id, TEMP);
          Serial.print(t);
          delay(5000);
        }
      }
      
      if (msg.callbackQueryData.equals(UMIDADE)){
        float h = dht.readHumidity();
        if (isnan(h)){
          myBot.endQuery(msg.callbackQueryID, "Falhou a comunicação com o sensor, tente novamente.", true);
        }
        else{
          char H[20];
          String UMID;
          UMID = floatToString(H, h, 2, 7) + (String)" %";
          myBot.sendMessage(msg.sender.id, UMID);
          Serial.print(h);
          delay(5000);
        }
      }

      if (msg.callbackQueryData.equals(PERMISSAOSIM)) {
        myBot.endQuery(msg.callbackQueryID, "Permissão Concedida.", true);
        digitalWrite(D2, HIGH);
        delay(3000);
      }
      
      if(msg.callbackQueryData.equals(PERMISSAONAO)) {
        myBot.endQuery(msg.callbackQueryID, "Permissão Negada.", true);
        digitalWrite(D2, LOW);
        delay(3000);
      }
    }
  }
}
