#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include "TOF_Sense.h"
#include <HardwareSerial.h>
#include "stm32f1xx.h"
#include <MPU6050_light.h>
#include <EEPROM.h>



//Memory: 224 Bytes//
///////////////////////////////////   CONFIGURATION   /////////////////////////////
int address = 0;

bool isCounting = false;  
unsigned long startTime = 0;

unsigned long timer = 0;
TwoWire Wire2(PB11, PB10);  
MPU6050 mpu(Wire2);


int16_t Distance;
float roll, pitch, yaw , Height, fast, Distance2 ,Distance1 ;


int16_t buttonPressCount = 0;  // Counter for button presses
float lookDownAngle = 0;       // Angle for "look down"
float lookUpAngle = 0;         // Angle for "look up"
int16_t savedDistance = 0;     // Distance value

float x_Acceloffset, y_Acceloffset, z_Acceloffset, x_Gyrooffset, y_Gyrooffset, z_Gyrooffset;

int buffersize = 1000;  //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
int acel_deadzone = 8;  //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire2, OLED_RESET, 1000000, 1000000);
#define MAX_LINES (SCREEN_HEIGHT / 10)  // Assuming 10 pixels per menu item
int menuOffset = 0;                     // ตำแหน่งเริ่มต้นของรายการที่แสดง

#define BUTTON_L PA9
#define BUTTON_R PA8
#define BUTTON_SELECT PA10

#define sensoron PB0
HardwareSerial Serial2(PA3, PA2);  


int menuIndex = 0;
const char *menuItems[] = { "Debug sensor", "high's taget", "how far boy", "record" };
const int menuSize = sizeof(menuItems) / sizeof(menuItems[0]);

unsigned long lastButtonPress = 0;  // For debouncing

///////////////////////////////setup///////////////////////////////////

void setup() {
  Serial.begin(115200);
  TOF_UART.begin(921600);
   Wire2.begin(); 
  if (!OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // สั่งให้จอ OLED เริ่มทำงานที่ Address 0x3C
    Serial.println("SSD1306 allocation failed");
  } else {
    Serial.println("ArdinoAll OLED Start Work !!!");
  }
  OLED.display();
  delay(2000);

  pinMode(BUTTON_R, INPUT_PULLUP);
  pinMode(BUTTON_L, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(sensoron, OUTPUT);
  digitalWrite(sensoron, HIGH);

  byte status = mpu.begin();
   delay(1000);  // รอให้เซ็นเซอร์ตั้งตัวก่อน
 mpu.calcOffsets(true, true);
  x_Acceloffset = readFloatFromEEPROM(0);
  y_Acceloffset = readFloatFromEEPROM(4);
  z_Acceloffset = readFloatFromEEPROM(8);
  x_Gyrooffset = readFloatFromEEPROM(12):
  y_Gyrooffset = readFloatFromEEPROM(14);
  z_Gyrooffset = readFloatFromEEPROM(18);
  mpu.setAccOffsets(x_Acceloffset, y_Acceloffset, z_Acceloffset); // Adjust values based on actual offset readings
  mpu.setGyroOffsets(x_Gyrooffset, y_Gyrooffset,  z_Gyrooffset);

  // แสดงเมนูเริ่มต้น
  updateMenu();
}

enum ProgramState {
  MAIN_MENU,
  OPTION_1,
  OPTION_2,
  OPTION_3,
  OPTION_4,
  OPTION_5,
};

ProgramState currentState = MAIN_MENU;

void loop() {
  delay(100);
  switch (currentState) {
    case MAIN_MENU:
      handleMenuNavigation();  // จัดการเมนูหลัก
      break;

    case OPTION_1:
      showOptionScreen1("Debug sensor");
      break;

    case OPTION_2:
      showOptionScreen2("high's taget");
      break;

    case OPTION_3:
      showOptionScreen3("how far boy");
      break;

    case OPTION_4:
      showOptionScreen4("record");
      break;
  }
}

///////////////////////////////////   FUNCTIONS    ////////////////////////////////////


void handleMenuNavigation() {
  // ตรวจจับการปล่อยปุ่ม BUTTON_R
  if (buttonWasReleased(BUTTON_R)) {
    currentState = MAIN_MENU;  // กลับไปหน้าหลัก
    updateMenu();
    //  Serial.println("return");
    delay(100);
  }

  // ตรวจจับการปล่อยปุ่ม BUTTON_L
  if (buttonWasReleased(BUTTON_L)) {
    menuIndex = (menuIndex + 1 + menuSize) % menuSize;  // เลื่อนไปซ้าย
    updateMenu();
    //   Serial.println("BUTTON_L released");
    delay(100);
  }

  // ตรวจจับการปล่อยปุ่ม BUTTON_SELECT
  if (buttonWasReleased(BUTTON_SELECT)) {
    // Serial.print("Selected: ");
    //  Serial.println(menuItems[menuIndex]);

    // เปลี่ยนสถานะไปที่ Option ที่เลือก
    if (menuIndex == 0) currentState = OPTION_1;
    else if (menuIndex == 1) currentState = OPTION_2;
    else if (menuIndex == 2) currentState = OPTION_3;
    else if (menuIndex == 3) currentState = OPTION_4;
    else if (menuIndex == 4) currentState = OPTION_5;
    delay(100);
  }
}

//////////////Debug sensor//////////
void showOptionScreen1(const char *optionName) {
  OLED.clearDisplay();
  OLED.setCursor(20, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  OLED.println(optionName);
  displayDistance();
  OLED.setCursor(20, 10);
  OLED.println("GYRO reoffset");
 OLED.print("aY: "); OLED.print(x_Acceloffset); OLED.print("  aX: "); OLED.println(y_Acceloffset); OLED.print("aZ: "); OLED.println(z_Acceloffset);
  if (buttonWasReleased(BUTTON_SELECT)) {
    delay(10);
    OLED.clearDisplay();
  OLED.setCursor(20, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  OLED.println(optionName);
  displayDistance();
  OLED.setCursor(20, 10);
  OLED.println("GYRO MPU-6050");
  OLED.setCursor(55,30);
  OLED.println("offset setting");
  x_Acceloffset = mpu.getAccX();
  y_Acceloffset = mpu.getAccY();
  z_Acceloffset = mpu.getAccZ();
  x_Gyrooffset = mpu.getGyroX();
  y_Gyrooffset = mpu.getGyroY();
  z_Gyrooffset = mpu.getGyroZ();
  writeFloatToEEPROM(0, x_Acceloffset);
  writeFloatToEEPROM(4, y_Acceloffset);
  writeFloatToEEPROM(8, z_Acceloffset);
  writeFloatToEEPROM(12, x_Gyrooffset);
  writeFloatToEEPROM(14, y_Gyrooffset);
  writeFloatToEEPROM(18, z_Gyrooffset);
  mpu.setAccOffsets(x_Acceloffset, y_Acceloffset, z_Acceloffset); // Adjust values based on actual offset readings
  mpu.setGyroOffsets(x_Gyrooffset, y_Gyrooffset,  z_Gyrooffset); // Adjust values based on actual offset readings
  delay(1000);
  buttonPressCount = 0;  // Resetting button press count
  }

    if (buttonWasReleased(BUTTON_L)) {
    buttonPressCount++;
  }

  switch (buttonPressCount) {
    case 1:
    OLED.clearDisplay();
    gyroread(); 
    OLED.setCursor(0, 0); 
    OLED.print("Roll: ");
  OLED.println(roll, 2);  // แสดง Roll ใน 0-360 องศา
  OLED.print("Pitch: ");
  OLED.println(pitch, 2);  // แสดง Pitch ใน 0-360 องศา*/

   OLED.println("TOF_0");
   OLED.setCursor(0, 50);
    OLED.print("Distance "); OLED.print(TOF_0.dis); OLED.println(" MM");
  break;

  default:                 // Reset after the third press
      buttonPressCount = 0;  // Resetting button press count
      break;
  }
  
  if (buttonWasReleased(BUTTON_R)) {
    currentState = MAIN_MENU;
    //vl53.stopRanging();
    updateMenu();
    //   Serial.println("Returning to Main Menu");
  }
   OLED.display();
}

void showOptionScreen2(const char *optionName) {
  // Update the OLED display
  OLED.clearDisplay();
  OLED.setCursor(20, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  OLED.println(optionName);
  displayDistance();
  gyroread();
  OLED.setCursor(10, 50);
  OLED.println("Press S to record");

  // Handle button press actions
  if (buttonWasReleased(BUTTON_SELECT)) {
    buttonPressCount++;
  }

  switch (buttonPressCount) {
    case 1:  // First press: Record "look down" angle
      OLED.clearDisplay();
      OLED.setCursor(56,30);
        OLED.println("+");
      lookDownAngle = pitch;  // Assuming roll is already updated in gyroread()
      OLED.setCursor(0, 0);
      OLED.print("Look Down Angle: ");
      OLED.println(lookDownAngle);
      break;

    case 2:  // Second press: Record "look up" angle
      OLED.clearDisplay();
      lookUpAngle = 360 - pitch;
      OLED.setCursor(56,30);
      OLED.println("+");
      OLED.setCursor(0, 0);
      OLED.print("Look Up Angle: ");
      OLED.println(lookUpAngle);
      break;

    case 3:         
      OLED.clearDisplay();             // Third press: Measure distance
      savedDistance = Distance/10;  // Assuming Distance is updated in displayDistance()
      OLED.setCursor(56,30);
      OLED.println("+");
       OLED.setCursor(0, 0);
      OLED.print("Distance: ");
      OLED.print(savedDistance);
      OLED.println(" CM");
      OLED.setCursor(10, 52);
      OLED.print("Pitch ");
      OLED.print(pitch);
      break;

    case 4:  // Third press: Measure distance
      Highrecord_F();
      OLED.clearDisplay();
      OLED.setCursor(20, 0);
      OLED.print("Height ");
      OLED.print(Height);
      OLED.print(" CM");
      OLED.setCursor(10, 52);
      OLED.println("pass S to re-record");
      break;

    default:                 // Reset after the third press
      buttonPressCount = 0;  // Resetting button press count
      break;
  }

  OLED.display();  // Update the OLED display

  // Handle return to main menu
  if (buttonWasReleased(BUTTON_R)) {
    currentState = MAIN_MENU;
    buttonPressCount = 0;
    updateMenu();
  }
}


void showOptionScreen3(const char *optionName) {
  OLED.clearDisplay();
  OLED.setCursor(0, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  float i = Distance/100.00;
  OLED.print(i,2); OLED.print(" M  ");
  OLED.print(fast); OLED.print(" M/s");
  displayDistance();

  if (buttonWasReleased(BUTTON_SELECT)) { 
    buttonPressCount++;
}

if (buttonPressCount == 1) {  // ใช้ if แทน while เพื่อลดโอกาสติดลูป
    if (!isCounting) {
        isCounting = true;
        startTime = millis();  
        Distance1 = Distance / 100.00;  // เก็บระยะทางเริ่มต้น
        OLED.clearDisplay();
        OLED.println("Start Counting...");
    }

    unsigned long elapsedTime = millis() - startTime;
    Serial.print(elapsedTime);

    if (elapsedTime >= 1000) {  // เมื่อครบ 1 วินาที
        Distance2 = Distance / 100.00;
        Serial.println("Finished counting 1 second!");
        
       // vl53.stopRanging();  // หยุดการวัดระยะ
        fast = (Distance2 - Distance1) / (elapsedTime / 1000.0);  
        
        isCounting = false;  
        buttonPressCount = 0;  // รีเซ็ตค่าเพื่อนับใหม่
    }
}

    OLED.setCursor(53,30);
        OLED.println("+");
        OLED.setCursor(0,55);
        OLED.print("D1: "); OLED.print(Distance1,2); OLED.print("  D2: "); OLED.print(Distance2,2);
  OLED.display();

  if (buttonWasReleased(BUTTON_R)) {
    currentState = MAIN_MENU;
    updateMenu();
    //   Serial.println("Returning to Main Menu");
  }
}

void showOptionScreen4(const char *optionName) {
  OLED.clearDisplay();
  OLED.setCursor(20, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  OLED.println("Height record  >>");
  OLED.setCursor(0, 20);
  OLED.print("LDA: ");
  OLED.println(lookDownAngle);
  OLED.print("LUA: ");
  OLED.println(lookUpAngle);
  OLED.print("Distance:       ");
  OLED.print(savedDistance);
  OLED.println(" CM");
  OLED.print("Height:       ");
  OLED.print(Height);
  OLED.println(" CM");

  if (buttonWasReleased(BUTTON_L)) {
    buttonPressCount++;
  }

  switch (buttonPressCount) {
    case 1: 
      OLED.clearDisplay();
      OLED.setCursor(0, 0);
      OLED.println("<<   how fast");
      OLED.setCursor(0, 20);
      OLED.print("how fast ");
      OLED.print(fast); OLED.println(" M/s");
      OLED.print("D1: "); OLED.print(Distance1); OLED.println(" M");
      OLED.print("D2: "); OLED.print(Distance2); OLED.println(" M");

      break;

    default:               
      buttonPressCount = 0;  
      break;
  }
  OLED.display();

  if (buttonWasReleased(BUTTON_R)) {
    currentState = MAIN_MENU;
    //vl53.stopRanging();
    updateMenu();
    //   Serial.println("Returning to Main Menu");
  }
}

void updateMenu() {
  OLED.clearDisplay();
  OLED.setCursor(40, 0);
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE, BLACK);
  OLED.println("Menu");

  if (menuIndex < menuOffset) {
    menuOffset = menuIndex;  // Scroll up
  } else if (menuIndex >= menuOffset + MAX_LINES) {
    menuOffset = menuIndex - MAX_LINES + 1;  // Scroll down
  }

  // Display menu items
  for (int i = 0; i < MAX_LINES && (i + menuOffset) < menuSize; i++) {
    int itemIndex = i + menuOffset;

    OLED.setCursor(10, 20 + i * 10);
    if (itemIndex == menuIndex) {
      OLED.setTextColor(BLACK, WHITE);  // Highlight selection
    } else {
      OLED.setTextColor(WHITE, BLACK);
    }
    OLED.print(menuItems[itemIndex]);
  }

  OLED.display();  // Ensure the display updates
}

// ตรวจจับการปล่อยปุ่ม (LOW → HIGH)
bool buttonWasReleased(int buttonPin) {
  static unsigned long lastDebounceTime[3] = { 0, 0, 0 };
  static bool buttonState[3] = { HIGH, HIGH, HIGH };
  unsigned long debounceDelay = 50;  // 50ms

  bool currentButtonState = digitalRead(buttonPin);
  int buttonIndex = (buttonPin == BUTTON_L) ? 0 : (buttonPin == BUTTON_R) ? 1 : 2;

  if (currentButtonState != buttonState[buttonIndex] && millis() - lastDebounceTime[buttonIndex] > debounceDelay) {
    lastDebounceTime[buttonIndex] = millis();
    if (currentButtonState == HIGH) {  // ปล่อย
      buttonState[buttonIndex] = currentButtonState;
      return true;
    }
  }
  buttonState[buttonIndex] = currentButtonState;
  return false;
}

void writeFloatToEEPROM(int address, float value) {
  byte* p = (byte*)(void*)&value;
  for (int i = 0; i < sizeof(float); i++) {
    EEPROM.write(address + i, *(p + i));
  }
}

float readFloatFromEEPROM(int address) {
  byte buffer[4];
  for (int i = 0; i < sizeof(float); i++) {
    buffer[i] = EEPROM.read(address + i);
  }
  float value;
  memcpy(&value, buffer, sizeof(float));
  return value;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
void displayDistance() {
   TOF_Inquire_Decoding(0);//Query and decode TOF data 查询获取TOF数据，并进行解码
  // TOF_Inquire_Decoding(0);//Query and decode TOF data 查询获取TOF数据，并进行解码
         Distance = TOF_0.dis;
      Serial.println(Distance);
  delay(20);//The refresh rate defaults to 50HZ. If the refresh rate is set to 100HZ, the time here is 1/100=0.01
}


void Highrecord_F() {
  float angleTopRad = lookUpAngle *(PI / 180.0);
  float angleBottomRad = lookDownAngle *(PI / 180.0);

  float h1 = savedDistance * tan(angleTopRad); //ใช้หน่วยเป็นเรเดียน
  float h2 = savedDistance * tan(angleBottomRad);
  Height = h1 + h2;
}

void gyroread() {
  mpu.update();
  
  if((millis()-timer)>10){ // print data every 10ms
  // ปกติ: Pitch = atan2(Ay, Az), Roll = atan2(Ax, sqrt(Ay^2 + Az^2))
  // แกน y เป็น แกนหลักแทน x
  /*roll pitch หน่วยเป็นองศา*/
roll = atan2(mpu.getAccZ(), mpu.getAccX()) * RAD_TO_DEG; // (180 / PI)
pitch = atan2(mpu.getAccY(), sqrt(mpu.getAccX() * mpu.getAccX() + mpu.getAccZ() * mpu.getAccZ())) * RAD_TO_DEG;
	timer = millis();  
  }
  if (pitch < 0) pitch += 360.0;
  if (roll < 0) roll += 360.0;
}
