#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//pins
const int trigPin = 6;   // Ultrasonic sensor trigger pin
const int echoPin = 7;   // Ultrasonic sensor echo pin
const int buzzer = 9;

//variables
const int max_height = 250;
const int min_height = 60;
const int min_fixing_height = 120;
int height, mean_height;
int sum = 0;
int len_ground;
int len_head = 0;
int human_detect_count = 0;
const int n = 10;
int height_array[n];
const int max_tolerence = 4; //allowable maximum tolerence in cm

RF24 radio(3, 4); // CE, CSN

const byte address[6] = "00001";

void array_reset(void){
  for (int i=0; i<n; i++){
    height_array[i] = 0;
  }
}

float ping_ultrasonic(){
  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo pulse duration
  long duration = pulseIn(echoPin, HIGH);

  // Calculate distance in centimeters
  float distance = duration * 0.034 / 2;
  return distance;
}

bool heights_are_valid(){
  int min = 600;
  int max = 0;
  for(int i=0; i<n; i++){
    if (height_array[i]<min) min = height_array[i];
    if (height_array[i]>max) max = height_array[i];
    if (height_array[i]<=min_height || height_array[i]>=max_height) {
      return false;
    }
  }

  //Serial.print("max - ");
  //Serial.println(max);
  //Serial.print("min - ");
  //Serial.println(min);
 
  if (max - min >= max_tolerence) {
    return false;
  }
  else {
    return true;
  } 
}

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();
  
  // Initialize serial communication
  Serial.begin(9600);

  // Configure pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  //Start taking the ground_height
  int ground=0;
  //to detect the groung level
  while(ping_ultrasonic()<min_fixing_height){
    delay(100);
  }

  for(int i=0; i<n; i++){
    int a = ping_ultrasonic();
    if (a>ground) {
      ground = a;
    }
    delay(400);
  }

  len_ground = ground;

  //buzzering trice
  for(int a=0; a<3; a++){
    digitalWrite(buzzer, HIGH);
    delay(1000);
    digitalWrite(buzzer, LOW);
    delay(1000);
  }
}

void loop() {
  
  //print the mean_height
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
 
  char destination[8];
  dtostrf(distance,5,2,destination);

  //const char text[] = String(distance);
  radio.write(&destination, sizeof(destination));
  delay(1000);

  // methanin uda chathurage code eka

  //
  len_head = ping_ultrasonic();
  //Serial.print(len_head);
  //Serial.print(" - ");
  height = len_ground - len_head;
  //Serial.println(height);

  //error checking part
  if (height>min_height && height<max_height){
    height_array[human_detect_count] = height;
    human_detect_count++;
    sum += height;
  }
  else{
    array_reset();
    human_detect_count = 0;
    sum = 0;
  }
  //Serial.print("human -  ");
  //Serial.println(human_detect_count);

  if (human_detect_count == n){
    mean_height = sum/n;

    if (heights_are_valid()== true){
      human_detect_count = 0;
      sum = 0;

      digitalWrite(buzzer, HIGH);
      delay(1000);
      digitalWrite(buzzer, LOW);

      //trasmitting height to the receiver side
      //print the mean_height
      Serial.print("Height: ");
      Serial.print(distance);
      Serial.println(" cm");
    
      char destination[8];
      dtostrf(distance,5,2,destination);

      //const char text[] = String(distance);
      radio.write(&destination, sizeof(destination));

      //given time to the man who measure height to leave the device
      delay(3000);

    }
    else{
      array_reset();
      human_detect_count = 0;
      sum = 0;
    }
  }
  delay(400);
}