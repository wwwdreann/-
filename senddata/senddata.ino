#include <IRremote.h>
#include <U8glib.h>
#define INTERVAL_LCD             20             //定义OLED刷新时间间隔  
int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);
decode_results results;
unsigned long lcd_time = millis();                 //OLED刷新时间计时器
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);     //设置OLED型号  
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_SS u8g.setFont(u8g_font_fub25n)
#define INTERVAL_SENSOR   17000             //定义传感器采样时间间隔  597000
#define INTERVAL_NET      17000             //定义发送时间
//传感器部分================================   
#include <Wire.h>                                  //调用库  
#include "ESP8266.h"
#define  sensorPin_1  A0
#define SSID           "wei"                   // cannot be longer than 32 characters!
#define PASSWORD       "12345678"
#define IDLE_TIMEOUT_MS  3000     
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "23332961"
#define PROJECTID "103417"
#define HOST_PORT   (80)
String apiKey="dOl3gECjrPZ38WPW22n55uOLfts=";
char buf[10];
#define INTERVAL_sensor 2000
unsigned long sensorlastTime = millis();
#define INTERVAL_OLED 1000
String jsonToSend;
#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); 
ESP8266 wifi(mySerial);
unsigned long net_time1 = millis();                          //数据上传服务器时间
unsigned long sensor_time = millis();                        //传感器采样时间计时器

//int SensorData;                                   //用于存储传感器数据
String postString;                                //用于存储发送数据的字符串
//String jsonToSend;                                //用于存储发送的json格式参数
String idToSend = "0"; //用于传输准入ID的字符串
void setup(void)     //初始化函数  
{       
  //初始化串口波特率  
    irrecv.enableIRIn(); // 启动红外解码
    //Wire.begin();
    Serial.begin(115200);   
    while(!Serial);
    pinMode(sensorPin_1, INPUT);

   //ESP8266初始化
    Serial.print("setup begin\r\n");   

  Serial.print("FW Version:");
  Serial.println(wifi.getVersion().c_str());

  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PASSWORD)) {      //加入无线网
    Serial.print("Join AP success\r\n");  
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

  Serial.print("setup end\r\n");
}
void loop()
{ 
   if (irrecv.decode(&results)) {
    u8g.firstPage();
    do {        
      setFont_L;
      u8g.setPrintPos(4, 16);
      if(results.value==33456255){
      idToSend = 3688112;
      u8g.print("IDcard A upload");
      sendIt();
      
      }
      if(results.value==33439935){
      idToSend = 3655379;
      u8g.print("IDcard B upload");
      sendIt();
      
      }
      if(results.value==33472575){
      idToSend = 3675631;
      u8g.print("IDcard C upload");
      sendIt();
      
      }
      if(results.value==33431775){
      idToSend = 3669169;
      u8g.print("IDcard D upload");
      sendIt();
      
      }
      if(results.value==33448095){
      idToSend = 3670528;
      u8g.print("IDcard E upload"); 
      sendIt(); 
      }
    }while( u8g.nextPage() );
    irrecv.resume();
}
}
void sendIt()
{
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据
    Serial.print("create tcp ok\r\n");
    
    jsonToSend="{\"passID\":";   
    jsonToSend+="\""+idToSend+"\"";
    jsonToSend+="}";

    postString="POST /devices/";
    postString+=DEVICEID;
    postString+="/datapoints?type=3 HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="Content-Length:";
    postString+=jsonToSend.length();
    postString+="\r\n";
    postString+="\r\n";
    postString+=jsonToSend;
    postString+="\r\n";
    postString+="\r\n";
    postString+="\r\n";

  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("send err\r\n");
  }
}

