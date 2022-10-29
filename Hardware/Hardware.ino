//                LEFT SIDE
int LeftSide_Red [] =   {53,50,47,44,41,38,35,32,29,26,23,20,17};      //Array to manage the LEDs red color pins
int LeftSide_Green [] = {52,49,46,43,40,37,34,31,28,25,22,19,16};    //Array to manage the LEDs green color pins
int LeftSide_Blue [] =  {51,48,45,42,39,36,33,30,27,24,21,18,15};   //Array to manage the LEDs blue color pins
int ChannelSize = 13; //Channel size 


//Set up serial port and pins
void setup() {
  Serial.begin(9600);
  //Set LEDs configuration as an output
  for  (int thisPin=0; thisPin < ChannelSize; thisPin++){
    pinMode(LeftSide_Red[thisPin], OUTPUT);   //Setting red pins as an output
    pinMode(LeftSide_Green[thisPin], OUTPUT); //Setting green pins as an output
    pinMode(LeftSide_Blue[thisPin], OUTPUT); //Setting blue pins as an output
  } 
}

void loop() {
  //The serial input need to be lile 000000000
  //where 0 is a white space, 1 a mormal ship,
  //2 a fishing ship and 3 a police ship
  
  if  (Serial.available()){ //Check if the serial port is available

      String inputRead = Serial.readString(); //Read what is entering on the port
      Serial.println(inputRead);   //Print what ir read by the port
      
      //Send the data to Light Led function so the LEDs can shine
      //the input partition is strip so each LED can represent it
      LightLED(inputRead.substring(0,1),LeftSide_Red[0], LeftSide_Green[0],LeftSide_Blue[0]); 
      LightLED(inputRead.substring(1,2),LeftSide_Red[1], LeftSide_Green[1],LeftSide_Blue[1]); 
      LightLED(inputRead.substring(2,3),LeftSide_Red[2], LeftSide_Green[2],LeftSide_Blue[2]); 
      LightLED(inputRead.substring(3,4),LeftSide_Red[3], LeftSide_Green[3],LeftSide_Blue[3]); 
      LightLED(inputRead.substring(4,5),LeftSide_Red[4], LeftSide_Green[4],LeftSide_Blue[4]); 
      LightLED(inputRead.substring(5,6),LeftSide_Red[5], LeftSide_Green[5],LeftSide_Blue[5]); 
      LightLED(inputRead.substring(6,7),LeftSide_Red[6], LeftSide_Green[6],LeftSide_Blue[6]);
      LightLED(inputRead.substring(7,8),LeftSide_Red[7], LeftSide_Green[7],LeftSide_Blue[7]); 
      LightLED(inputRead.substring(8,9),LeftSide_Red[8], LeftSide_Green[8],LeftSide_Blue[8]);  
      LightLED(inputRead.substring(9,10),LeftSide_Red[9], LeftSide_Green[9],LeftSide_Blue[9]);  
      LightLED(inputRead.substring(10,11),LeftSide_Red[10], LeftSide_Green[10],LeftSide_Blue[10]);  
      LightLED(inputRead.substring(11,12),LeftSide_Red[11], LeftSide_Green[11],LeftSide_Blue[11]);  
      LightLED(inputRead.substring(12),LeftSide_Red[12], LeftSide_Green[12],LeftSide_Blue[12]);  
      Serial.println(inputRead);   //Print what ir read by the port
    } 
      
}

void LightLED (String color, int RedPin, int GreenPin, int BluePin){
  
  if(color == "3"){
    Serial.println("RED");
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, LOW); 
  }
  if(color == "2"){
    Serial.println("GREEN");
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, LOW); 
  }
  if(color == "1"){
    Serial.println("BLUE");
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, HIGH); 
  }    
  if(color == "0"){
    Serial.println("WHITE");
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, HIGH); 
  }
  if(color == "4"){
    Serial.println("PURPLE");
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, LOW);
    digitalWrite(BluePin, HIGH); 
  }
  if(color == "5"){
    Serial.println("BROWN");
    digitalWrite(RedPin, HIGH);
    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, LOW); 
  }
  if(color == "6"){
    Serial.println("TURQUOISE");
    digitalWrite(RedPin, LOW);
    digitalWrite(GreenPin, HIGH);
    digitalWrite(BluePin, HIGH); 
  }  
}
