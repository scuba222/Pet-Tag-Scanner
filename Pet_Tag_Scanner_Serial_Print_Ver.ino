
/*  This project was inspired by Andreas Spiess   
 *   
 *  I watched his video     https://www.youtube.com/watch?v=y89yJ1Fq-hQ&t=42s
 *  
 *  my video               https://youtu.be/ix70PTXmay0
 *  
 *  The video determined which of his two cats were which.
 *  
 *  I wanted to display the Decimal tag number which is used by the animal ID databases to identify pets
 *  
 *  This number has a 3 digit, country code  and a 12 digit tag number      900 215000123456
 *  
 *  The size of number, causes some problems ,but finally i managed to  modified the code to work.
 *  
 *  There are two versions 
 *  
 *    serial print the ID 
 *    Display ID on an lcd Display
 *  
 *   The solution to ptinting the large uint64_t number was hound here from a user called Gruber  many thanks
 *   
 *   https://stackoverflow.com/questions/45974514/serial-print-uint64-t-in-arduino?fbclid=IwAR03QX7tfylqyNNYKTKPtRD7_6ElwIjsdr35uAfemmV3oXST5USOYqfRiu4
 *  
 *  
 *  
 *                                 Tech info how the module encodes the Tag ID number  
                                   https://allinbest.com/blog/1342k-animal-tag-fdxb-iso11784-reader-module/

   HEX encoded ASCII                                 
 *  
 * conversion info says treat data as Hex then convert to ASCII  get bits 11.12.13  
 * 
 *    ie  Hex 34 38 33          from RFID module
 *    
 *    Ascii   4  8  3  Lsb on left
 *    reverse   3  8  4  Lsb on right
 *    Hex 384 convert to Dec = 900  ( country code )
 * 
 *  
 * 
 * V1 for serial print version
 * V2 for lcd Display version          NB you can either display serial print or lcd screen but not BOTH at the same time.
 * 
 *                                        you can set this in the print_uint64_t FUNC coment out the appropriate one.
 * 
 * 

       This example code is in the public domain.
       modified by Paul Meynell
       11.1.2021
 */


char message[35];
unsigned long lastSignal = 0;
bool transmission = false;
byte state = 1;
int pos;

byte tagFlag =0;


void setup() {
  Serial.begin(9600);
 
  Serial.println("Start");
  lastSignal = millis();
}


void loop() {
  switch (state) {
    case 1: {

        if (Serial.available() > 0) {                        // listening for data
          lastSignal = millis();
          pos = 0;
          state = 2;
        }
        break;
      }
    case 2: {
                                                                   
        if (Serial.available() > 0 && pos < 35) {                 // Reading of message from RFID Module
          lastSignal = millis();
          message[pos] = Serial.read();                           // put the numbers into an array
          
                         Serial.print(message[pos], HEX);         // print out the numbers in Hex
                         Serial.print(" ");
          pos++;
        }
        if (millis() - lastSignal > 100) state = 3;               // if  data has stopped ciming in move to stage 3
        
        if (pos >= 35) {                                          // if all data has been read go and wait for the next tag to be scanned
          delay(300);
          state = 1;
        }
      }
      break;

      
    case 3: {
                                                                  // check the  checksum
        Serial.println();
        byte check = message[1];
        for (int i = 2; i < 27; i++) {
          check = check ^ message[i];
        }
        Serial.println();
              Serial.print(check);
             Serial.print(",");
              Serial.println(message[27], HEX);
        if (check == message[27]) {
                  Serial.println("Check OK");



                                         //*****************************************************************************               
                                                         //******** Decoding Data Hex to Decimal**********
                                          
          
                                     
          int countryNbr = hexInDec(message, 11, 4);      // convert hex country code into decimal 
          
          tagFlag =1;                                  // set flag after country code has been decoded
                                                              
          uint64_t id = hexInDec(message, 1, 10);    // convert hex card number  into decimal (put country code in reverse order into hex converter
           
          tagFlag =0;       // reset divide flag
          
         
        //  Serial.print("Country=");
       //   Serial.println(countryNbr);

          
       //   Serial.print("CardNumber=");
        //  print_uint64_t(id);                             // display ID country code and tag number


        
          state = 1;
        }
        break;
      }
    
   }           // close switch
   
}            // close void loop



                                     //******************** HEX TO DEC CONVERTER FUNCTION  ******************************


unsigned long hexInDec(char message[], int beg , int len)                 // beg ( where to begin )   len ( number of array elements to alter)

  {                        
                                                                  
     uint64_t mult = 1;                       
     uint64_t nbr = 0;                                                    // holds total decimal number
     byte nextInt;

  for (int i = beg; i < beg + len; i++) {                                 // convert each number and add to a decimal total
    nextInt = message[i];

 
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);

   
nbr = nbr + (mult * nextInt);

    mult = mult * 16;

   }              // close for loop
  print_uint64_t(nbr);
  return nbr;                                          
}    

// close func



   // *************************************************************

   void print_uint64_t(uint64_t num) {              // Serial print uint64_t FUNC

  char rev[128]; 
  char *p = rev+1;

  while (num > 0) {
    *p++ = '0' + ( num % 10);
    num/= 10;
  }
  p--;
  /*Print the number which is now in reverse*/
  while (p > rev) {
    Serial.print(*p--);
  }
}
