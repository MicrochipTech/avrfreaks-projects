# LM35

Uploaded by ulinnuhasna on 2016-08-08 03:17:00 (rating 2 out of 5)

## Summary

/* @Author Khasna Ulin Nuha and Menia Nurma Widita  

   @Date 01/08/2016  

 */  

   

//Wiring Sensor LM 35  

/*  

 * Modul -- Arduino  

 * VCC -- 5V  

 * GND -- GND  

 * Vout-- A0


 */  

int analogPin = 0;  

     

//Variabel untuk menyimpan data suhu  

float suhu;  

     

void setup()  

{  

  //Setup serial dengan baud 9600  

  Serial.begin(9600);  

  Serial.println( " Sensor LM 35  " );  

}  

     

void loop()  

{  

   

  suhu = analogRead(analogPin);    

  suhu = suhu * 0.048828125;  // Calculate ((5 V*100 / 1024)/100))


  Serial.println(suhu);  

  Serial.print( "Suhu : " );  

  //Delay sebelum melakukan pembacaan selanjutnya  

  delay(50);  

}

## Compilers

- Arduino IDE

## Tags

- Complete code
- Arduino IDE
