//SKY x FAMILY face LED matrix
#include <LedControl.h>

//  SB
#include <SerialBridge.hpp>
#include <InoHardwareSerial.hpp>
#include "./Emotion.hpp"

#define EMOTION_TX_ID 20

SerialDev *dev = new InoHardwareSerial(&Serial1);
SerialBridge serial(dev);

Emotion emotion_msg;

//Face expression changeing variable for communication
#define B0 10
#define B1 9
int state = 0;

//defining pin, function
int brightness = 15;
int eye1 = 0;
int eye2 = 1;
int mo1 = 3;
int mo2 = 4;
int ch1 = 2;
int ch2 = 5;
int DIN = 11;
int CS =  12;
int CLK = 13;
int e_start = 0;
int e_normal = 0;
int e_funny = 0;
int e_sadly = 0;
int m_normal = 0;
LedControl lc=LedControl(DIN,CLK,CS,6);

//defining timer value
unsigned long prev = 0;
unsigned long m_prev = 0;
long turn_0 = 3000;
long turn_m = 4000;
long anim_speed = 100;
long exp_speed = 3000;

//defining sprite
//eye
byte eye[7][8]={
  {0x3c,0x7a,0xf9,0xff,0x8f,0x81,0x42,0x3c},  //normal
  {0x00,0x3c,0x7a,0xfd,0x8f,0x42,0x3c,0x00},  //normal interpolate
  {0x00,0x00,0x00,0x81,0x7e,0x00,0x00,0x00},  //closing
  {0x00,0x00,0x0f,0xf0,0xf0,0x0f,0x00,0x00},  //funny interpolate
  {0x03,0x0c,0x30,0xc0,0xc0,0x30,0x0c,0x03},  //funny
  {0x00,0x00,0xf0,0x0f,0x0f,0xf0,0x00,0x00},  //funny inter reverse
  {0xc0,0x30,0x0c,0x03,0x03,0x0c,0x30,0xc0}   //funny reverse

};
//left-half mouth
byte mouth1[5][8]={
  {0x00,0xfc,0x06,0xfa,0x02,0x04,0x18,0xe0},  //open
  {0x00,0x00,0xfe,0x06,0x02,0x04,0xf8,0x00},  //open interpolate
  {0x00,0x00,0x04,0xf8,0x00,0x00,0x00,0x00},  //close
  {0xfe,0x06,0xfa,0x06,0x02,0x04,0x18,0xe0},  //funny
  {0x00,0xf8,0x0c,0xf2,0x02,0x02,0x84,0x78}   //sadly  
};
//right-half mouth
byte mouth2[5][8]={//right
  {0x00,0x3f,0x60,0x5f,0x40,0x20,0x18,0x07},  //open
  {0x00,0x00,0x7f,0x60,0x40,0x20,0x1f,0x00},  //open interpolate
  {0x00,0x00,0x20,0x1f,0x00,0x00,0x00,0x00},  //close
  {0x7f,0x60,0x5f,0x40,0x40,0x20,0x18,0x07},  //funny
  {0x00,0x1f,0x30,0x4f,0x40,0x40,0x21,0x1e}   //sadly
};
//cheek, tear
byte cheek[5][8]={
  {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  //dim
  {0x55,0xaa,0x00,0x00,0x00,0x00,0x00,0x00},  //normal
  {0x55,0xaa,0xaa,0x00,0x00,0x00,0x00,0x00},  //funny
  {0xe0,0x50,0x48,0x48,0x30,0x00,0x00,0x00},  //tear_left
  {0x07,0x0a,0x12,0x12,0x0c,0x00,0x00,0x00}   //tear_right
};

//initialize
void setup(){
  //  setup SerialBridge
  Serial1.begin(115200);
  serial.add_frame(EMOTION_TX_ID, &emotion_msg);

  //  setup led control
  int ini = 0;
    for( ini=0;ini<8;ini++ ){
      lc.shutdown(ini,false);
      lc.setIntensity(ini,brightness);
      lc.clearDisplay(ini);
    }
  pinMode( B0, INPUT );
  pinMode( B1, INPUT );
}

void loop(){
  if(serial.update() == 0)
  {
    if(emotion_msg.was_updated())
    {
      state = (int) emotion_msg.data.face;
    }
  }
  //system time
  unsigned long now = millis();

  //starting face with closed eyes
  if (state == 0){
    if (e_start == 0){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      draw(ch1, cheek[0]);
      draw(ch2, cheek[0]);
      if (now - prev > 2000){prev = now;  e_start = 1;}
    }
    if (e_start == 1){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      draw(ch1, cheek[0]);
      draw(ch2, cheek[0]);
      if (now - prev > anim_speed){prev = now;  state = 1;}
    }
  }
  else{
    /*
    if (digitalRead(B0) == LOW){
      state = 2;
    }
    if (digitalRead(B1) == LOW){
      state = 3;
    }
    */
  }

  //normal face
  if (state == 1){
    if (e_normal == 0){
      draw(eye1, eye[0]);
      draw(eye2, eye[0]);
      draw(ch1, cheek[1]);
      draw(ch2, cheek[1]);
      if (now - prev > turn_0){prev = now; e_normal = 1;}
    }
    if (e_normal == 1){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      if (now - prev > anim_speed){prev = now; e_normal = 2;}
    }
    if (e_normal == 2){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      if (now - prev > 150){prev = now; e_normal = 3;}
    }
    if (e_normal == 3){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      if (now - prev > anim_speed){turn_0 = random(500,10000); prev = now; e_normal = 0;}
    }

    //okuchi pakupaku
    if (m_normal == 0){
      draw(mo1, mouth1[0]);
      draw(mo2, mouth2[0]);
      if (now - m_prev > turn_m){m_prev = now; m_normal = 1;}
    }
    if (m_normal == 1){
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      if (now - m_prev > anim_speed){turn_m = random(1000,8000); m_prev = now; m_normal = 2;}
    }
    if (m_normal == 2){
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      if (now - m_prev > turn_m){m_prev = now; m_normal = 3;}
    }
    if (m_normal == 3){
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      if (now - m_prev > anim_speed){turn_m = random(7000,20000); m_prev = now; m_normal = 0;}
    }
  }

  //funny face
  if (state == 2){
    if (e_funny == 0){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      if (now - prev > anim_speed){prev = now;  e_funny = 1;}
    }
    if (e_funny == 1){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      
      if (now - prev > anim_speed){prev = now;  e_funny = 2;}
    }
    if (e_funny == 2){
      draw(eye1, eye[3]);
      draw(eye2, eye[5]);
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      draw(ch1, cheek[2]);
      draw(ch2, cheek[2]);
      if (now - prev > anim_speed){prev = now;  e_funny = 3;}
    }
    if (e_funny == 3){
      draw(eye1, eye[4]);
      draw(eye2, eye[6]);
      draw(mo1, mouth1[3]);
      draw(mo2, mouth2[3]);
      draw(ch1, cheek[2]);
      draw(ch2, cheek[2]);
      if (now - prev > exp_speed){prev = now; e_funny = 4;}
    }
    if (e_funny == 4){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      if (now - prev > anim_speed){prev = now;  e_funny = 5;}
    }
    if (e_funny == 5){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      if (m_normal == 0){
        draw(mo1, mouth1[1]);
        draw(mo2, mouth2[1]);
      }
      if (now - prev > anim_speed){prev = now; m_prev = now; e_funny = 0; state = 1;}
    }
  }

  //sadly face
  if (state == 3){
    if (e_sadly == 0){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      draw(ch1, cheek[1]);
      draw(ch2, cheek[1]);
      if (now - prev > anim_speed){prev = now;  e_sadly = 1;}
    }
    if (e_sadly == 1){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      draw(ch1, cheek[1]);
      draw(ch2, cheek[1]);
      if (now - prev > anim_speed){prev = now;  e_sadly = 2;}
    }
    if (e_sadly == 2){
      draw(eye1, eye[3]);
      draw(eye2, eye[5]);
      draw(mo1, mouth1[1]);
      draw(mo2, mouth2[1]);
      if (now - prev > anim_speed){prev = now;  e_sadly = 3;}
    }
    if (e_sadly == 3){
      draw(eye1, eye[4]);
      draw(eye2, eye[6]);
      draw(mo1, mouth1[4]);
      draw(mo2, mouth2[4]);
      draw(ch1, cheek[3]);
      draw(ch2, cheek[4]);
      if (now - prev > exp_speed){prev = now; e_sadly = 4;}
    }
    if (e_sadly == 4){
      draw(eye1, eye[2]);
      draw(eye2, eye[2]);
      draw(mo1, mouth1[2]);
      draw(mo2, mouth2[2]);
      if (now - prev > anim_speed){prev = now;  e_sadly = 5;}
    }
    if (e_sadly == 5){
      draw(eye1, eye[1]);
      draw(eye2, eye[1]);
      if (m_normal == 0){
        draw(mo1, mouth1[1]);
        draw(mo2, mouth2[1]);
      }
      draw(ch1, cheek[0]);
      draw(ch2, cheek[0]);
      if (now - prev > anim_speed){prev = now; m_prev = now; e_sadly = 0; state = 1;}
    }
  }

}

//drawing function
void draw(uint16_t unit, byte ch []){
  int col = 0;
  for(col=0;col<8;col++){
    lc.setRow(unit,col,ch[col]);
  }
}