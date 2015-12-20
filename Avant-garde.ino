#include <SoftwareSerial.h>
#include <Adafruit_NeoPixel.h>

#define DOT_ROW 10
#define DOT_COL 8

#define DOT_PIN 6

#define SCROLL_INTERVAL 500

SoftwareSerial BTSerial(2, 3); // (RX, TX)

Adafruit_NeoPixel strip = Adafruit_NeoPixel(80, DOT_PIN, NEO_GRB + NEO_KHZ800);
 
void setup()  
{
  Serial.begin(9600);
  Serial.println("Hello!");
 
  BTSerial.begin(9600);  // set the data rate for the BT port

  // set neopixel
  strip.begin();
  strip.setBrightness(20);
  strip.show();
}

char buff[100];
int index=0;
int mode = 0;
int scroll = 0;
unsigned long scrollTime;
int col_size = 0;
uint32_t textColor = strip.Color(255, 0, 0);
void loop()
{
  // BT –> Data –> Serial
  if (BTSerial.available()) {
    char temp = BTSerial.read();

    Serial.println(temp);
    if(mode == 1)
    {
      if(temp == 'e') {
        mode = 0;
        // column 사이즈 측정
        col_size = index / DOT_ROW;
        // 테스트용
        for(int i=0; i<DOT_ROW; i++) {
          for (int j=0; j<col_size; j++) {
            Serial.print(buff[i * col_size + j]);
            Serial.print("  ");
          }
          Serial.println("");
        }
        Serial.print("len: ");
        Serial.println(index, DEC);
      } else {
        buff[index] = temp;
        index++;
      }
    }
    else if(mode == 0)
    {
      if(temp == 's')
      {
        mode = 1;
        index = 0;
        initDotmatrix();
      }
    }
  }
  // Serial –> Data –> BT
  
  if (Serial.available()) {
    BTSerial.write(Serial.read());
  }

  // print dotmatrix
  if(mode == 0) {
    if (millis() - scrollTime >= SCROLL_INTERVAL) {
      scrollTime = millis();
      printDotmatrix();
    }
  }
}

void printDotmatrix() {
  printBytes();
  scroll++;
  if (scroll > col_size - 8) scroll = 0;
}

void initDotmatrix() {
  scroll = 0;
  eraseBytes();
}

void printBytes() {
  for(int i=0; i<DOT_ROW; i++) {
    for(int j=0; j<DOT_COL; j++) {
      int idx;
      if (j % 2) idx = DOT_COL * i + j;
      else       idx = DOT_COL * (i + 1) - j;
      /*if (buff[i * col_size + j + scroll] == 1) {
        strip.setPixelColor(idx, textColor);
      } else {
        strip.setPixelColor(idx, 0, 0, 0);
      }*/
      strip.setPixelColor(idx, buff[i * col_size + j + scroll]);
    }
  }
  strip.show();
}

void eraseBytes() {
  for(int i=0; i<DOT_ROW * DOT_COL; i++) {
    strip.setPixelColor(i, 0, 0, 0);
  }
  strip.show();
}

