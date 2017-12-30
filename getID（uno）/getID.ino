#define SSID  "zd"
#define PWD   "hahahahaha"
#define HOST_NAME   "api.heclouds.com"
#define DEVICEID   "23332961"
#define PROJECTID "103417"
#define HOST_PORT   (80)
#define IDLE_TIMEOUT_MS  3000
String apiKey="dOl3gECjrPZ38WPW22n55uOLfts=";
char buf[10];
String postString;                                //用于存储发送数据的字符串
String passID;
#include <Wire.h>
#include <SoftwareSerial.h>
#include "ESP8266.h"
SoftwareSerial mySerial(10, 11); // RX, TX
ESP8266 wifi(mySerial);
void setup() {
  // put your setup code here, to run once:
  //初始化串口波特率 并且设置为从机 
  Wire.begin(8);         
  Serial.begin(9600);
  while (!Serial){
    ;
  }
  //ESP8266初始化
  Serial.print("setup begin\r\n");    // 在这个串口使用print数字没问题
  if (wifi.setOprToStationSoftAP()) {             // 切换模式为单SAT
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }
  if (wifi.joinAP(SSID, PWD)) {      //加入无线网   为什么只写了2个参数？
    Serial.print("Join AP success\r\n");  
  } else {
    Serial.print("Join AP failure\r\n");
  }
  if (wifi.disableMUX()) {             // 作用为启动单线模式
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }
  Serial.print("setup end\r\n");

}

void loop() {
  getPassID();
  Serial.println("passID is");
  Serial.println(passID);
  //while (1);
  Wire.onRequest(requestEvent);  
  delay(5000);
}
void requestEvent() {  
  for (int i=0;i<7; i++)
  {
    Serial.print(passID[i]);
    Wire.write(passID[i]); 
  }
  Serial.print("End");
  Serial.println();
  // as expected by master
}
void getPassID() {  
  //long ID=0; 
   passID="";
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据    
    Serial.print("create tcp ok\r\n");    
    
    postString="GET /devices/";
    postString+=DEVICEID;
    postString+="/datastreams/passID HTTP/1.1";
    postString+="\r\n";
    postString+="api-key:";
    postString+=apiKey;
    postString+="\r\n";
    postString+="Host:api.heclouds.com\r\n";
    postString+="Connection:close\r\n";
    postString+="\r\n";
  uint8_t buffer[512] = {0};    //储存回应数据
  const char *postArray = postString.c_str();                 //将str转化为char数组
  //Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   


  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);
  //输出数据
    if (len > 0) {     
   Serial.println("here is the data"); 
          Serial.println();
          //筛选passID
          uint32_t head=285;
          while (1)
          {
              head++;
              if ((char)buffer[head]=='l')
              {
                  head+=6;
                  while ((char)buffer[head]!='\"')
                  {
                    //ID=10*ID+(char)buffer[head]-48;
                    passID=passID+(char)buffer[head];
                    head++;
                  }
                  break;
              }
              
          }
    }
     if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
     } 
     else {
        Serial.print("release tcp err\r\n");
     }
      postArray = NULL;                                       //清空数组，等待下次传输数据
  
  } 
  else {
    Serial.print("create tcp err\r\n");
  }
//  return ID;
}
