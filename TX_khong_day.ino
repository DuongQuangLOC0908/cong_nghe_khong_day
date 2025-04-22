#include <RF24.h>
#include <LiquidCrystal.h>
#include<SPI.h>
#define up    A1
#define select A2
#define dw    A0
#define back  A3
#define ON    1
#define OFF   0
RF24 radio(9, 10); 
const byte address[6] = "12345";
LiquidCrystal lcd(7,6,5,4,3,2);
unsigned tt=1;
int ena=0;
bool motor1_status = OFF;  
bool motor2_status = OFF;  
bool motor3_status = OFF;  
bool motor4_status = OFF; 
const int motor1 = 1;
const int motor2 = 2;
const int motor3 = 3;
const int motor4 = 4;
bool result;
typedef struct linker
{
  struct linker *pre;
  int menuID;
  char title[16];
  char list1[4];
  struct linker *menulist1;
  void(*activationON)(int,int);
  char list2[4];
  struct linker *menulist2;
  void(*activationOFF)(int,int);
  char list3[4];
  struct linker *menulist3;
  char list4[4];
  struct linker *menulist4;
} menu;
menu *ptr;

void send_data(int menuID, int status) {
  char message[6]=""; 
  sprintf(message, "%d%s", menuID, status ? "ON" : "OFF");
  radio.write(&message, sizeof(message));
}
void activation(int devices, int status){
  switch(devices){
    case motor1:
      motor1_status = status;  
      send_data(devices, status);
      break;
    case motor2:
      motor2_status = status;  
      send_data(devices, status);
      break;
    case motor3:
      motor3_status = status;  
      send_data(devices, status);
      break;
    case motor4:
      motor4_status = status; 
      send_data(devices, status);
      break;
  }
}
menu Motor1 = {
  NULL,motor1,
  "               ",
  "ON ", NULL,&activation,
  "OFF", NULL,&activation,
  "   ", NULL,
  "   ", NULL, 
};

menu Motor2 = {
  NULL,motor2,
  "               ",
  "ON ", NULL,&activation,
  "OFF", NULL,&activation,
  "   ", NULL,
  "   ", NULL, 
};

menu Motor3 = {
  NULL,motor3,
  "               ",
  "ON ", NULL,&activation,
  "OFF", NULL,&activation,
  "   ", NULL,
  "   ", NULL, 
};

menu Motor4 = {
  NULL,motor4,
  "               ",
  "ON ", NULL,&activation,
  "OFF", NULL,&activation,
  "   ", NULL,
  "   ", NULL, 
};

menu mainmenu = {
  NULL,NULL,
  "DEVICES STATUS",
  "1  ", NULL,NULL,
  "2  ", NULL,NULL,
  "3  ", NULL,
  "4  ", NULL,
};



void displaymenu(menu *menu)
{
  lcd.setCursor(0,0);
  lcd.print(menu->title);
  lcd.setCursor(1,1);
  lcd.print(menu->list1);
  lcd.setCursor(5,1);
  lcd.print(menu->list2);
  lcd.setCursor(9,1);
  lcd.print(menu->list3);
  lcd.setCursor(13,1);
  lcd.print(menu->list4);
}
void contro_lcd() {
  lcd.setCursor(0,1); lcd.print(tt==1 ? ">" : " ");
  lcd.setCursor(4,1); lcd.print(tt==2 ? ">" : " ");
  lcd.setCursor(8,1); lcd.print(tt==3 ? ">" : " ");
  lcd.setCursor(12,1); lcd.print(tt==4 ? ">" : " ");
}
void button_up()
{
   if(digitalRead(up)==0)
   {
      delay(20);
      if(digitalRead(up)==0)
      { 
        if(ena==1){
          tt++;
          if(tt>2)tt=1;
        }
        else{  
         tt++;
         if(tt>4)tt=1;
        }
        while(digitalRead(up)==0);
      }
   }
}
void button_dw()
{
   if(digitalRead(dw)==0)
   {
      delay(20);
      if(digitalRead(dw)==0)
      {
        if(ena==1){
          if(tt>=0){
            tt--;
            if(tt<=0)tt=2;
          }
        }
        else{
        if(tt>=0){
         tt--;
         if(tt<=0)tt=4;
        }
        }
         while(digitalRead(dw)==0);
      }
   }
}
void button_select()
{
  if(digitalRead(select)==0)
   {
      delay(20);
      if(digitalRead(select)==0)
      { 
        if(ena==0){

          if(tt==1){
            ptr=(ptr->menulist1==NULL)?ptr:ptr->menulist1;
          }
          if(tt==2){
            ptr=(ptr->menulist2==NULL)?ptr:ptr->menulist2;
          }
          if(tt==3){
            ptr=(ptr->menulist3==NULL)?ptr:ptr->menulist3;
          }
          if(tt==4){
            ptr=(ptr->menulist4==NULL)?ptr:ptr->menulist4;
          }   
          ena=1;
          tt=1;  
        } 
        if(ena==1){
            if(tt==1){
              if(ptr->activationON!=NULL)ptr->activationON(ptr->menuID,ON);
            }
            if(tt==2){
              if(ptr->activationOFF!=NULL)ptr->activationOFF(ptr->menuID,OFF);
            }
          }  
        }
         while(digitalRead(select)==0);
    }
   

}
void button_back()
{
  if(digitalRead(back)==0)
   {
      delay(20);
      if(digitalRead(back)==0)
      { 
        ena=0;
        ptr=(ptr->pre==NULL)?ptr:ptr->pre;
        tt=1;
         while(digitalRead(select)==0);
      }
   }

}
void update_mainmenu_status() {
  strcpy(mainmenu.list1,(motor1_status==ON)?"1ON":"1OF");
  strcpy(mainmenu.list2,(motor2_status==ON)?"2ON":"2OF");
  strcpy(mainmenu.list3,(motor3_status==ON)?"3ON":"3OF");
  strcpy(mainmenu.list4,(motor4_status==ON)?"4ON":"4OF");
}
void setup_struct(){
  mainmenu.pre= NULL;
  Motor1.pre=&mainmenu;
  Motor2.pre=&mainmenu;
  Motor3.pre=&mainmenu;
  Motor4.pre=&mainmenu;
  mainmenu.menulist1=&Motor1;
  mainmenu.menulist2=&Motor2;
  mainmenu.menulist3=&Motor3;
  mainmenu.menulist4=&Motor4;
}
void setup_radio(){
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);  // Giảm công suất để tránh quá tải nguồn
  radio.setChannel(80);           // 125 kênh từ 0-124; TX và RX phải cùng kênh
  radio.setDataRate(RF24_250KBPS);  // Chậm lại để ổn định hơn
  radio.stopListening(); // Chế độ gửi
}
void setup_pin(){
  pinMode(up,INPUT);
  pinMode(select,INPUT);
  pinMode(dw,INPUT);
  pinMode(back,INPUT);
}
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    tt=1;
    setup_struct();
    setup_pin();
    setup_radio();
    lcd.begin(16,2);
    ptr=&mainmenu; 
    
}
void loop() {
  // put your main code here, to run repeatedly:
  button_up();
  button_dw();
  displaymenu(ptr);
  contro_lcd();
  button_select();
  button_back();
  update_mainmenu_status();
  

}
