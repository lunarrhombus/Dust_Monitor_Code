// Center For Dirt and Gravel Road Studies
// The Pennsylvania State University
// The Larson Transportation Institute
// Dust Monitoring System V.8 (stable)
// Created: 7 - 24 - 2018
// Lasted updated: 2 - 22 - 2020
// Written By Mohamed Abuzaid - lunarmoh@gmail.com
// This code takes data from three sensors, processes the data.
// After which the data is outputted via usb to a computer
// program (any serial terminal, however terra term is recommended)
// at a 1 Hz refresh rate.
// An onboard rec/stop button will be used to
// GPS codes used as references found on
// http://www.toptechboy.com/arduino/lesson-22-build-an-arduino-gps-tracker/
// http://webcache.googleusercontent.com/search?q=cache:http://www.doctormonk.com/2012/05/sparkfun-venus-gps-and-arduino.html
// *************************************************************************

int factor = 20;
const int sent_size = 80;
char sentence[sent_size];
const int sent_size_temp = 20;
char sentencetemp[sent_size_temp];

void controlled_loop();
bool io = false;

double analog_correction_factor = 0.000255;

double d10 = 0;
double d02 = 0;

bool check_io(){
  bool x = true;
    while(x){
    int ioi = 0;
    ioi = digitalRead(33);
    Serial.println(io);
    delay(650);
    if(ioi == HIGH){
      x = false;
    }
    ioi = 0;

}
return true;
}

void title () {
  Serial.println("*****************************************************************************************************");
  Serial.print("Time(UTC)");
  Serial.print("\t");
  Serial.print("Speed");
  Serial.print("\t");
  Serial.print("Course");
  Serial.print("\t");
  Serial.print("Lat.");
  Serial.print("\t\t");
  Serial.print("Long.");
  Serial.print("\t\t");
  Serial.print("#Sate");
  Serial.print("\t");
  Serial.print("10M");
  Serial.print("\t");
  Serial.print("2.5M");
  Serial.print("\t");
  Serial.print("R-T");
  Serial.print("\t");
  Serial.print("A-T");
  Serial.println();
}

void setup(){
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  Serial.begin(9600);  // Serial Monitor on
  Serial2.begin(115200);
  Serial1.begin(9600);

  digitalWrite(31, HIGH);
  analogWrite(4, 900);
  io = check_io();

  if (io == true){
    title();
    controlled_loop();
  }
}

void loop() {} // This will be left empty to have more control of the code

void controlled_loop(){
  int character_position = 0;
  while(true){

        if(Serial1.available()){
          char bit_data = Serial1.read();
          if(bit_data != '\n' && character_position < sent_size){
            sentence[character_position] = bit_data;
            character_position++;
          }
          else{
            sentence[character_position] = '\0';
            character_position = 0;
            gps_output();
          }
        }
}
}

void org_formatted_sentence(char * buffer, int index){ // picks out data from the NMEA sentence
  int sentence_position = 0;
  int field_position = 0;
  int comma_count = 0;
  while(sentence_position < sent_size){
    if(sentence[sentence_position] == ','){
      comma_count ++;
      sentence_position ++;
    }
    if(comma_count == index){
      buffer[field_position] = sentence[sentence_position];
      field_position ++;
    }
    sentence_position ++;
  }
buffer[field_position] = '\0';
}

void gps_output(){ // displays all data excluding title
  analogWrite(5, 900);

  char field[20];
  char field2[20];
  org_formatted_sentence(field, 0);
    if (strcmp(field, "$GPGGA") == 0){
    org_formatted_sentence(field2, 7); // satenumb
    }
  org_formatted_sentence(field, 0);
  if (strcmp(field, "$GPRMC") == 0){
   float x = 0 ;
    org_formatted_sentence(field, 1); // time
    Serial.print(field);
    Serial.print("\t");
    org_formatted_sentence(field, 7); // sog knots
    x = strtod(field,NULL);
    x = x * 1.150779 ; // knots to mph
    Serial.print(x);
    Serial.print("\t");
    org_formatted_sentence(field, 8); // cog
    Serial.print(field);
    Serial.print("\t");
    org_formatted_sentence(field, 3); // lat
    Serial.print(field);
    Serial.print("\t");
    org_formatted_sentence(field, 5);  // long
    Serial.print("-");
    Serial.print(field);
    Serial.print("\t");
    Serial.print(field2);
    Serial.print("\t");
    dustvolt();
    Serial.println();
    analogWrite(5, 0);
  }
}

void dustvolt(){
    d10 = analogRead(0) * (5.0/1024.0 + analog_correction_factor) * factor ;
    d02 = analogRead(3) * (5.0/1024.0 + analog_correction_factor) * factor ;
    Serial.print(d10);
    Serial.print("\t");
    Serial.print(d02);
    Serial.print("\t");
    temp_data_in();
  }

  void temp_data_in(){

    int pos = 0;
    int count = 0;
    float rt = 0;
    float at = 0;
    int com1 = 0;
    int fcp = 0;

int poscom = 0;
    bool x = true;
    for(int i = 0; i < sizeof(sentencetemp); i++){
      sentencetemp[i] = (char)0;
      }
    while(x){
      if(Serial2.available()){
        char tempbit = Serial2.read();
         if(tempbit != '\n' && pos < sent_size_temp){
            sentencetemp[pos] = tempbit;
            pos++;
            if(sentencetemp[pos] == ','){
              com1++;
              if(pos != 9 && com1 == 1){
                poscom = pos;
              }

              }
          }
          else{
            sentencetemp[pos] = '\0';
            x = false;
            tempdata();
          }
        }
      }
        }
void tempdata(){
  char field[4];
  double x = 0 ;
  proctemp(field, 1);
   x = strtod(field,NULL);
    x = x*(1.8) +32;
    Serial.print(x);
  proctemp(field, 2);
  x = strtod(field,NULL);
    x = x*(1.8) +32;
   Serial.print("\t");
  Serial.print(x);
  Serial.print("\t");
  Serial.print(sentencetemp);
  }
void proctemp(char * buffer, int index){
  int sentence_position = 0;
  int field_position = 0;
  int comma_count = 0;
  while(sentence_position < sent_size_temp){
    if(sentencetemp[sentence_position] == ','){
      comma_count ++;
      sentence_position ++;
    }
    if(comma_count == index){
      buffer[field_position] = sentencetemp[sentence_position];
      field_position ++;
      if(field_position == 4){
        sentence_position = sent_size_temp;
        }
    }
    sentence_position ++;
  }
buffer[field_position] = '\0';
  }
