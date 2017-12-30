//调用库
#include <IRremote.h>                               //红外传感
#include <U8glib.h>                                 //OLED应用
#include <Wire.h>                                   //iic串口通讯  
#include "ESP8266.h"                                //wifi定义
///////////////////////////////////////////////////////////
#define INTERVAL_LCD             5                  //定义OLED刷新时间间隔  
int RECV_PIN = 10;
IRrecv irrecv(RECV_PIN);                           //重定义命名，便于使用
decode_results results;                           //重定义命名，便于使用
U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);        //设置OLED型号  
//-------字体设置，大、中、小
#define setFont_L u8g.setFont(u8g_font_7x13)
#define setFont_M u8g.setFont(u8g_font_fixed_v0r)
#define setFont_S u8g.setFont(u8g_font_fixed_v0r)
#define setFont_SS u8g.setFont(u8g_font_fub25n)
///////////////////////////////////////////
#define  sensorPin_1  A0                            //设置红外传感器串口
//wifi相关设置//////////////////////////////
#define SSID           "zd"                         //wifi名，不允许中文
#define PASSWORD       "hahahahaha"                 //wifi密码
#define IDLE_TIMEOUT_MS  3000                       //设置超限时间
#define HOST_NAME   "api.heclouds.com"              //onenet地址
#define DEVICEID   "23332961"                       //设备id
#define PROJECTID "103417"                          //项目id
#define HOST_PORT   (80)                            //服务器端口
String apiKey="dOl3gECjrPZ38WPW22n55uOLfts=";       //apikey
////////////////////////////////////////////
char buf[10];
#define INTERVAL_sensor 2000                        //传感器间隔
#define INTERVAL_OLED 1000                          //OLED间隔
unsigned long sensorlastTime = millis();
//串口定义//////////////////////////////////
#include <SoftwareSerial.h>                       //软串口库
SoftwareSerial mySerial(2, 3);                    //软串口D2,D3
ESP8266 wifi(mySerial);                             //定义wifi串口
////////////////////////////////////////////
unsigned long net_time1 = millis();                 //数据上传服务器时间
unsigned long sensor_time = millis();               //传感器采样时间计时器
String jsonToSend;                                  //json流定义
String postString;                                  //用于存储发送数据的字符串
String idToSend = "0";                              //用于传输准入ID的字符串
void setup(void)     //初始化函数  
{        
    irrecv.enableIRIn();                            //启动红外解码
    Serial.begin(115200);                          //定义串口波特率   
    while(!Serial);                                
    pinMode(sensorPin_1, INPUT);
//wifi设置////////////////////////////////////////
    Serial.print("setup begin\r\n");   
  if (wifi.setOprToStationSoftAP()) {
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }
//连入wifi，检查wifi///////////////////////////////////////
  if (wifi.joinAP(SSID, PASSWORD)) {                //加入无线网
  } else {
    Serial.print("Join AP failure\r\n");   //
  }

  if (wifi.disableMUX()) {
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

  Serial.print("setup end\r\n");
}
///////////////////////////////////////////////
void loop()
{ 
   if (irrecv.decode(&results)) {
    //OLED///
    u8g.firstPage();
    do {        
      setFont_L;
      u8g.setPrintPos(4, 16);
      if(results.value==33456255){
      idToSend = 3688112;
      u8g.print("IDcard A upload");
      sendIt();
      }
      else
      if(results.value==33439935){
      idToSend = 3655379;
      u8g.print("IDcard B upload");
      sendIt();
      
      }
      else
      if(results.value==33472575){
      idToSend = 3675631;
      u8g.print("IDcard C upload");
      sendIt();
      
      }
      else
      if(results.value==33431775){
      idToSend = 3669169;
      u8g.print("IDcard D upload");
      sendIt();
      
      }
      else
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
//建立json串//////////////////    
    jsonToSend="{\"passID\":";   
    jsonToSend+="\""+idToSend+"\"";
    jsonToSend+="}";
//建立post报头,具体发送的数据////////////////
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
////////////////////////////////////////////////////////
  const char *postArray = postString.c_str();                 //将str转化为char数组
  Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");
  u8g.setPrintPos(4, 32);
  u8g.print("ok!");
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } else {
    Serial.print("send err\r\n");
  }
}

