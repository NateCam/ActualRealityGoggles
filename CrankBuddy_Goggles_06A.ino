// CrankBuddy Goggles Nov 2016
//
// make sure programmer is : USBtinyISP Board Adafruit Trinket 16Hz may need to power board externally  
//changelog 3: change implementation off brightness settings
//changelog 4 buggy adding 4 more modes
//changelog 5: change more 4 more modes and implementation of dimming changed to prevent problems dimming.  general bug fixes
//changelog 6: add headlight mode & clean up of code and simplfyy color assignments and finish implemenet EEPROM and misc bug fixes
//6A EEPROM Intitial values entered- 0=fps setting, 1= brightness 2= darker false
//adafruit trinket 16mhz board

#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
 #include <avr/power.h>
#endif

#define PIN 0// talk to neopixel
#define PIN_BRIGHTNESS 1 /* input for controlling brightness.  
its attached to LED on board so input has a pull-down.  
one leg of button connects to this pin, one goes to 5v supplt pin reads HIGH when button is pressed, LOW when released.*/
#define PIN_BUTTON 2// pin for trigger different modes, i.e. frame rate choices.
//uint8_t brightness= 60; // load this on to board, change brightnness via button, then comment out and un-comment EEPROM read 
#define PIN_PIEZO 4 // pin 4 is piezo



int brightness= EEPROM.read(1); //brightness level//was int brightness=10 ;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(32, PIN);
//int piezoPin =3;

//uint8_t  mode = 0, // set this once, load onto board, then change mode via button, and then comment it out and uncomment EEPOROM read
uint8_t mode =EEPROM.read (0); //read EEPROM 0 for mode or FPS setting
uint8_t offset = 0; // Position of spinny eyes

////COLOR ASSIGNMENTS///
uint32_t color;  //COLOR OF CHASE LIGHT
uint32_t c = 0;
uint32_t colorOffset=0;	 //COLOR OF FIELD
uint32_t sixteenFPScolor=  0x3366FF;//blue
uint32_t sixteenFPSaltColor= 0x0000ff;//alt blue
uint32_t eighteenFPScolor=0x00FF00 ;//green
uint32_t eighteenFPSaltColor= 0x012c00;//alt Green
uint32_t twentyFourFPScolor=0xFFcD00 ;//Yellow 255 205,00
uint32_t twentyFourFPSAltColor=0x404000; //q true yellow//0x643300;//quarter original yellow//0x7f6600;// half of orinal yellow//0x888800 ;// 9f8200 alt Yellow (half yellow?
uint32_t thirtyFPScolor=0xFF0000 ;// Red
uint32_t thirtyFPSaltColor=0x640000; //q red// 0x890000;//0xFF3300;//Alt Red orangeish
uint32_t headlight=0x7f7f7f;// chase lights
uint32_t headlightAlt=0xffffff;//field//

uint32_t prevTime;
uint8_t beepPoint=0;
boolean darker= EEPROM.read(2); //was default false


void setup() {
//Serial.begin (9600);
// intital EEPROM VALUES first time chip is loaded
  //EEPROM.write (0,0);
 // EEPROM.write (1,10);
 //  EEPROM.write(2,0);
 
    //END EEPROM VALUES ENTER  COMMENT OUT to make not 6A
  //setting input pins to high means turning on internal pull up resistor
  digitalWrite (PIN_BUTTON,HIGH);
  
  //remember the buttons are active-low they read LOW when not pressed
 pinMode(PIN_PIEZO,OUTPUT);
 // digitalWrite (piezoPin, LOW);
#ifdef __AVR_ATtiny85__ // Trinket, Gemma, etc.
  if(F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin();
  pixels.setBrightness(brightness); // 60= 1/3 brightness
 prevTime = micros();
}


/*/////////******the sound producing function for trinket**** ////////
void beep (unsigned char speakerPin, int frequencyInHertz, long timeInMilliseconds)
{    // http://web.media.mit.edu/~leah/LilyPad/07_sound_code.html
          int x;    
          long delayAmount = (long)(1000000/frequencyInHertz);
          long loopTime = (long)((timeInMilliseconds*1000)/(delayAmount*2));
          for (x=0;x<loopTime;x++)    
          {    
              digitalWrite(speakerPin,HIGH);
              delayMicroseconds(delayAmount);
              digitalWrite(speakerPin,LOW);
              delayMicroseconds(delayAmount);
          }    
}
*/

void loop() {
  prevTime= micros();
  uint8_t  i;
  uint32_t t;
  
  //*********MODE CHANGING//FPS CHANGING********//
 
  if (digitalRead(PIN_BUTTON) == LOW) { mode++; 
 if (mode >8) {
      mode =0;
    }
    EEPROM.write (0,mode);
 
  for(i=0; i<16; i++) {
     
    //  uint32_t c = 0; // wipe all pixels off

      
      switch(mode) {
        case 0:
        
       color  = sixteenFPScolor ; //BLUE//
       c=0;
      if(i<6 || i>13 )c = color;// 2 frames per pixel
    // get display to start at specific place
    break;
      
       case 1:
       color  = eighteenFPScolor ; //green//
        c=0;
      if(i<6 || i>12 )c = color;// 2 frames per pixel
      break;
      
       case 2:
       color  =twentyFourFPScolor; //0xFF9900 ;//yellow
       c=0;
      if(i<6 || i>9 )c = color;// 2 frames per pixel
      break;
      

       case 3:
       color  = thirtyFPScolor ; //red//
       c=0;
      if(i<6 || i>6 )c = color;// 2 frames per pixel
      break;
      
      
      case 4: // blue inverted
       color  = sixteenFPScolor ;
       c=sixteenFPSaltColor;
      if(i<6 || i>13 )c = color;// 2 frames per pixel
      break;
      
      case 5: //green inverted
       color  = eighteenFPScolor ; //green//
       c=eighteenFPSaltColor;
      if(i<6 || i>12 )c = color;// 2 frames per pixel speed indicator
      break;
      
      case 6: //yellow inverted
      color=twentyFourFPScolor; //0xFF9900 ;//yellow
      c=  twentyFourFPSAltColor;
       if(i<6 || i>9 )c = color;// 2 frames per pixel speed indicator
      break;
      
      case  7: //red inverted

// TEST CHANGES//
      
       color  = thirtyFPScolor;
        c= thirtyFPSaltColor;//ORANGE RED  //RED// 
      if(i<6 || i>6 )c = color;// 2 frames per pixel speed indicator
      break;
      
      case 8:
      color = headlight;
      c= headlightAlt;
      // no assignment of c= color//
      }
 
 pixels.setPixelColor(   i, c);
 pixels.setPixelColor (16+i,c); //second eye 
  }     
 pixels.show();
 delay (1500);
 }
 
 ///******END MODE CHANGING*****//
 
    
 //*******Brightness of NEOPIXELS*******//
 //NOTE HAVE TO SET PIXEL COLOR EVERYTIME BRIGHNESS CHANGES OR WIERD STUFF HAPPENS***//
 
 if (digitalRead(PIN_BRIGHTNESS) == HIGH) {
 
    /*for(i=0; i<32; i++) {
    
  if (mode<4) {pixels.setPixelColor(   i, color); 
  }
  if (mode>3) {pixels.setPixelColor(i,colorOffset); //use backdrop for color indicator
   // IN MODE 4-6 USES COLOROFFSET AS FIELD COLOR8
  }
    
    pixels.show(); // light up all pixels in color
    delay (200);//INITIAL LIGHT UP SHOWING ORIGINAL VALUE
  }
  */
    
     for(int x=0; x<110; x++) //increment brightness up or down 110 units
    {

for(i=0; i<32; i++) {
 
  if (mode<4) {pixels.setPixelColor(   i, color); 
  }
  if (mode>3) {pixels.setPixelColor(i,colorOffset); //use backdrop for color indicator
  } 
   }
 pixels.setBrightness (brightness);
 /* you aren't really supposed to use brightness except at setup, 
 as it can cause skews in color as it dims and brightens in sketch, 
 but rather control brightness by adjusting color, 
 but what are you gonna do?  
 if you choose primary colors carefully the colors won't shift, 
 which is what I did here.*/
        pixels.show();
        delay(1);
        if (x==0) {delay (200);} // initial delay
      switch (darker)
       {
         case true:
       brightness= brightness -1;
       break;
       
        case false:
        brightness = brightness +1;
        break;
       }
  
     if (brightness <=10) {darker= false;}
 if (brightness >=230) {darker= true;}
  }
    
    
    EEPROM.write (2,darker);

  pixels.setBrightness(brightness); 
  EEPROM.write (1,brightness);
 pixels.show();
 delay (500); 
 
  
 }
///////*****END BRIGHTNESS CONTROL*****////
  
  //*********main code**********//
  
switch(mode) {
      
      case 0://16fps /* 2 revolutions or 32 intervals so 31.25 millis or 30 + 1250 micro
      color  = sixteenFPScolor ;
      colorOffset = 0;
     delay(29);
      delayMicroseconds(2250-(t-prevTime));
     
      break;
      
      // 16fps rate //
  //  31.250 millisecons so 30 +1250*/
  // delayMicroseconds(1250-(t-prevTime));//fine tune rate (1000ms in 1 delay unit)
  //  break;

  
    case 1: //18fps
    color  = eighteenFPScolor ; //green//
    colorOffset=0;
     delay(25);
   // 18fps rate 2fps remainder 4 more pixels so 36 pixels 27.778 millis 0r 25+2778
  delayMicroseconds(2778-(t-prevTime));
 //fine tune rate 
  break;
  
  case 2: //24fps
  color  = twentyFourFPScolor;//0xFF9900 ;//yellow
  colorOffset=0;
    delay(18);
    // 24fps rate   3 rev/per sec or 48 pixels  1/48=  41.667 millisecons or 3 revolutions  or 48 pixels 20.833
 delayMicroseconds(2833-(t-prevTime));
  //fine tune rate 
   break;
   
   case 3:// 30 fps  (
    color  = thirtyFPScolor ; //red//
    colorOffset = 0;
    
   //  if(((offset+5) & 15) < 1) // one beep at bottom of rotation
  //  {
  // beep(PIN_PIEZO, 4000, 15);
 // }
  //   else { 

     delay(14);
   //} 
   //60 pixels or 16.667
   delayMicroseconds(2667-(t-prevTime));
      //fine tune rate 
 break;
 
 
  case 4://16fps INVERT/* 2 revolutions or 32 intervals so 31.25 millis or 30 + 1250 micro
      color  = sixteenFPScolor ;
      colorOffset=sixteenFPSaltColor; // 
     delay(29);
delayMicroseconds(2250-(t-prevTime));
    break;
      
     // end of 16 inverted
      
      // 16fps rate 
   
  //  31.250 millisecons so 30 +1250*/
  
   // delayMicroseconds(1250-(t-prevTime));//fine tune rate (1000ms in 1 delay unit)
  //  break;
  
    case 5: //18fps INVERT
    color  = eighteenFPScolor ; //green CHASE LIGHT
    colorOffset= eighteenFPSaltColor;     //FIELD
     delay(25);
 
   // 18fps rate 2fps remainder 4 more pixels so 36 pixels 27.778 millis 0r 25+2778
  delayMicroseconds(2778-(t-prevTime));
 //fine tune rate 
 
  break;
  
  case 6: //24fps INVERT
  color  = twentyFourFPScolor; //pure yellow CHASE LIGHT
colorOffset= twentyFourFPSAltColor;//yellow at half approc FIELD
    delay(18);
  
    // 24fps rate   3 rev/per sec or 48 pixels  1/48=  41.667 millisecons or 3 revolutions  or 48 pixels 20.833
 delayMicroseconds(2833-(t-prevTime));
  //fine tune rate 
   break;
   
   case 7 :// 30 fps INVERT (
   // changes//
    color  = thirtyFPScolor;//chase
    colorOffset=thirtyFPSaltColor;// field
    
     delay(14); 
   //60 pixels or 16.667
   delayMicroseconds(2667-(t-prevTime));
      //fine tune rate 
 break;
 
 case 8: //headlight mode
 color= headlight; //chase lights//
 colorOffset= headlightAlt; //field
 delay (62);
 break;
    }
   
    for(i=0; i<16; i++) { // this "for" paints or assignes the colors of all pixels
 c=colorOffset;
      if(((offset + i) & 15) < 2) c = color; // 4 pixels on...
      pixels.setPixelColor(i, c); 
        pixels.setPixelColor (16+i,c); //second eye (flipped)
     
    }// end of "for" that assigns all pixels
      
 
    
     //prevTime= millis();
    pixels.show();
    offset++;
 // if (i=beepPoint)
   
  // if(((offset + i) & 15) < 3)beep(PIN_PIEZO, 4000, 15);
    t= micros();
   
 }  


