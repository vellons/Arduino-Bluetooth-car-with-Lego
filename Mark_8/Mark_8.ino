// Mark 8
// Creator: Vellons
// Last hardware edit: 01/10/2016
// Last software edit: 01/10/2016


//bluetooth
String readString;             //variabile per dati ricevuti da bluetooth


//servomotore sterzo
#include <Servo.h>
Servo Sterzo;

//variabili per inclinazione sterzo
int destra = 135;
int dritto = 90;
int sinistra = 45;


//variabili per controllo luminosità
int statoLuciAuto = 0;             //int luci automatiche attivo/disattivo
int luceX = 15;                    //int per accenzione lui auto      (dato variabile in base alla luminosità ambientale)
long tempocorrenteLuce = 0;        //int per controllare luminosità ogni tot



//collegamenti pin per HC-SR04 e variabili distanze e sicurezza
//frontale
int triggerPort1 = 12;
int echoPort1 = 7;

int spazio1 = 0;                //int spazio rimanente


int distanzaX1 = 25;            //int distanza di sicurezza        (dato variabile in base alla velocità della macchina)

int statoAirBagAuto = 0;        //int air bag attivo/disattivo
int vaiIndietro = 0;            //se 1 prima di fare incidente va indietro; se 0 si ferma
int fermaRetro = 0;             //la macchina mentre vai indietro prima di indare a sbattere può ricevere altri comandi
long tempocorrenteRetro = 0;    //tempo di intervallo per fermare macchina



//variabili funzionamento
int motore1ST = 0;             //int per capire se la macchina va avanti, ferma, indietro stato movimento macchina
int statoAutomatico = 0;       //int pilota automatico attivo/disattivo
long adesso = 0;               //long del tempo in quel momento scritto con millis (  adesso=millis();  )  variabile di tempo importante!





//variabili nomi uscite/entrate arduino
int led = 13;
//12 HC-SR04 trig1
int motore1 = 11;      // PWM
int motore2 = 10;      // PWM
//9 HC-SR04 trig2      // PWM NON ATTACCATO
//8 servomotore Sterzo        NON ATTACCATO
//7 HC-SR04 echo1
int clacson = 6;       // PWM
//5 HC-SR04 echo2      // PWM NON ATTACCATO
//4 servomotore Collo
//3 HC-SR04 trig3      // PWM NON ATTACCATO
//2 HC-SR04 echo3             NON ATTACCATO
//1 HC-06 RX (legge  segnale nella porta seriale)
//0 HC-06 TX (scrive segnale nella porta seriale)

//AO
int sensoreL = A1;      //sensore luminosità
//A2
//A3
//A4
int ledRetro = 19;      //led luci per retromarcia





void controlloDistanza1()
{
  digitalWrite(triggerPort1, LOW);
  digitalWrite(triggerPort1, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPort1, LOW);
  long duration1 = pulseIn(echoPort1, HIGH);
  int distanza1 = 0.034 * duration1 / 2;
  if ( duration1 > 25000 )
  {
    spazio1 = 340;
  }
  else
  {
    spazio1 = distanza1;
  }
}



void setup()
{
  Serial.begin(9600);
  Sterzo.attach(8);
  Sterzo.write(dritto);
  pinMode(led, OUTPUT);
  pinMode(ledRetro, OUTPUT);
  pinMode(motore1, OUTPUT);
  pinMode(motore2, OUTPUT);
  pinMode(clacson, OUTPUT);

  pinMode(triggerPort1, OUTPUT);
  pinMode(echoPort1, INPUT);
  
  statoLuciAuto = 1;
  statoAirBagAuto = 1;
}

void loop()
{

  //Piolota automatico beta non isponibile
  if (statoAutomatico == 1)
  {
    Serial.println("Pilota automatico non disponibile in Mark8Slim");
    statoAutomatico = 4;
  }


  //controllo distanza di sicurezza avanti squando va avanti
  if ((motore1ST == 1) && (statoAirBagAuto == 1))
  {

    controlloDistanza1();

    Serial.print("Distanza impatto:  ");
    Serial.println(spazio1);

    if (spazio1 < distanzaX1)
    {
      controlloDistanza1();
      if (spazio1 < distanzaX1)
      {
        controlloDistanza1();
        if (spazio1 < distanzaX1)
        {
          Serial.println("AIRBAG POSTERIORE");
          digitalWrite(motore1, LOW);
          digitalWrite(motore2, LOW);
          motore1ST = 0;
          if (vaiIndietro == 1)
          {
            delay(100);
            digitalWrite(ledRetro, HIGH);
            digitalWrite(motore1, LOW);
            digitalWrite(motore2, HIGH);
            fermaRetro = 1;
            motore1ST = 2;
            adesso = millis();
            tempocorrenteRetro = adesso;
          }
        }

      }
    }
  }


  //se la macchina stava andando indietro perchè stava facendo un incidente > dopo 1.6 sec fermati
  if (fermaRetro == 1)
  {
    adesso = millis();
    if (adesso - tempocorrenteRetro >= 1600)
    {
      if (motore1ST == 2)
      {
        fermaRetro = 0;
        digitalWrite(motore1, LOW);
        digitalWrite(motore2, LOW);
        digitalWrite(ledRetro, LOW);
        motore1ST = 0;
      }

    }
  }




  //controllo se la luminosità è maggiore della luminosita minima(luceX)
  if (statoLuciAuto == 1)  //luci automatiche (ON)
  {

    adesso = millis();
    if (adesso - tempocorrenteLuce >= 1500)//ogni 1.5 sec
    {
      tempocorrenteLuce = adesso;

      int luminosita = analogRead (sensoreL);
      //Serial.println(luminosita);

      if (luminosita < (luceX + 3))  //luci automatiche (ON)
      {
        int luminosita = analogRead (sensoreL);
        if (luminosita < luceX)
        {
          digitalWrite(led, HIGH);
        }
      }


      if (luminosita > (luceX + 8)) //luci automatiche (OFF)
      {
        int luminosita = analogRead (sensoreL);
        if (luminosita > (luceX + 9))
        {
          int luminosita = analogRead (sensoreL);
          if (luminosita > (luceX + 10))
          {
            digitalWrite(led, LOW);
          }
        }
      }
    }
  }




  //aspetto input da porta seriale...   (bluetooth)
  while (Serial.available())
  {
    delay(8);
    char c = Serial.read();
    readString += c;
  }

  //se ricevo...
  if (readString.length() > 0)
  {
    Serial.println(readString);


    if (readString == "SINISTRA")
    {
      Sterzo.write(sinistra);
    }

    if (readString == "DESTRA")
    {
      Sterzo.write(destra);
    }

    if (readString == "DRITTO")
    {
      Sterzo.write(dritto);
    }

    if (readString == "AVANTI")
    {
      if (motore1ST == 0)
      {
        digitalWrite(motore1, HIGH);
        digitalWrite(motore2, LOW);
        digitalWrite(ledRetro, LOW);
        motore1ST = 1;
      }
      else if (motore1ST == 1)
      {
        digitalWrite(motore1, HIGH);
        digitalWrite(motore2, LOW);
        digitalWrite(ledRetro, LOW);
        motore1ST = 1;
      }
      else if (motore1ST = 2)
      {
        digitalWrite(motore1, LOW);
        digitalWrite(motore2, LOW);
        digitalWrite(ledRetro, LOW);
        motore1ST = 0;
      }
    }

    if (readString == "INDIETRO")
    {
      if (motore1ST == 0)
      {
        digitalWrite(motore1, LOW);
        digitalWrite(motore2, HIGH);
        digitalWrite(ledRetro, HIGH);
        motore1ST = 2;
      }
      else if (motore1ST == 1)
      {
        digitalWrite(motore1, LOW);
        digitalWrite(motore2, LOW);
        digitalWrite(ledRetro, LOW);
        motore1ST = 0;
      }
      else if (motore1ST == 2)
      {
        digitalWrite(motore1, LOW);
        digitalWrite(motore2, HIGH);
        digitalWrite(ledRetro, HIGH);
        motore1ST = 2;
      }
    }

    if (readString == "CLACSON")
    {
      analogWrite(clacson, 128);
      delay(333);
      analogWrite(clacson, 0);
    }

    if (readString == "LUCIAUTO")
    {
      statoLuciAuto = 1;
      digitalWrite(led, HIGH);
      delay(125);
      digitalWrite(led, LOW);
      delay(125);
      digitalWrite(led, HIGH);
      delay(125);
      digitalWrite(led, LOW);
      delay(125);
    }

    if (readString == "LUCION")
    {
      digitalWrite(led, HIGH);
      statoLuciAuto = 0;
    }

    if (readString == "LUCIOFF")
    {
      digitalWrite(led, LOW);
      statoLuciAuto = 0;
    }

    if (readString == "AIRBAGON")
    {
      statoAirBagAuto = 1;
    }

    if (readString == "AIRBAGOFF")
    {
      statoAirBagAuto = 0;
    }

    if (readString == "AUTOON")
    {
      statoAutomatico = 1;
      statoAirBagAuto = 0;
      analogWrite(clacson, 128);
      delay(200);
      analogWrite(clacson, 0);
      delay(200);
      analogWrite(clacson, 128);
      delay(200);
      analogWrite(clacson, 0);
    }

    if (readString == "AUTOOFF")
    {
      statoAutomatico = 0;
      statoAirBagAuto = 1;
      digitalWrite(motore1, LOW);
      digitalWrite(motore2, LOW);
      analogWrite(clacson, 128);
      delay(200);
      analogWrite(clacson, 0);
      motore1ST = 0;
    }

    if (readString == "DISCONNESSIONE")
    {
      digitalWrite(ledRetro, LOW);
      digitalWrite(motore1, LOW);
      digitalWrite(motore2, LOW);
      Sterzo.write(dritto);
      motore1ST = 0;

      digitalWrite(led, HIGH);
      analogWrite(clacson, 128);
      delay(333);
      analogWrite(clacson, 0);
      digitalWrite(led, LOW);
      statoAirBagAuto = 1;
      statoLuciAuto = 1;
      statoAutomatico = 0;
      vaiIndietro = 0;
    }

    if (readString == "INDIETROON")
    {
      vaiIndietro = 1;
    }

    readString = "";
  }

}
