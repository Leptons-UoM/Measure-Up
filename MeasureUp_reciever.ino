#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//OLED display
#define screen_width 128 //OLED display width in pixels
#define screen_height 64 //OLED display height in pixels

//Adafruit_SSD1306 display(screen_width, screen_height); 
//creating the display object
Adafruit_SSD1306 OLED = Adafruit_SSD1306(128,64,&Wire);

void oled_display(bool clear_display,int text_size,int cursor_x, int cursor_y,char* string){
  if (clear_display){
    OLED.clearDisplay();
  }
  OLED.setTextSize(text_size);
  OLED.setTextColor(WHITE);
  OLED.setCursor(cursor_x, cursor_y);
  OLED.print(string);
  OLED.display();
}

RF24 radio(3, 4); // CE, CSN

const byte address[6] = "00001";

void setup() {
  Serial.begin(9600);

  //setting up the reciever
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  //setting up the display
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  oled_display(true, 2, 10,27, "LEPTONS");
  delay(4000);
}

void loop() {
  // if radio available output the height
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);

    //OLED###########################################################
    /*OLED.clearDisplay();
    OLED.setTextColor(WHITE);

    OLED.setTextSize(2);
    OLED.setCursor(43,0);
    OLED.print("cm");
  
    OLED.setTextSize(3);
    OLED.setCursor(10,26);
    OLED.print(text);

    OLED.display();*/

    oled_display(true, 2, 0,0, "Height:");
    oled_display(false, 2, 43,0, "cm");
    oled_display(false, 3, 10,26, text);
    //OLED over##############################################################
  }

  else{
   oled_display(true, 2, 8,27, "Measuring..."); 
  }
}
