#define SSID  "zd"                                //修改为你的wifi命名，仅限英文
#define PWD   "hahahahaha"                        //修改为你的wifi密码
#define HOST_NAME   "api.heclouds.com"            //onenet域名，无需修改
#define DEVICEID   "23332961"                     //设备id
#define PROJECTID "103417"                        //项目id
#define HOST_PORT   (80)                          //服务器端口
#define IDLE_TIMEOUT_MS  3000                     //超时时长限定
String apiKey="dOl3gECjrPZ38WPW22n55uOLfts=";     //修改为你自己的apikey
String passid;
char buf[10];
String postString;                                //用于存储发送数据的字符串
/////////////下面是关于舵机的变量/////////////
int flag=0,RX_Flag=0;//串口标志
char Code[14]; //用于存放读到串口数据
long Num=0;//解码数据
int serpin=7;//舵机引脚定义
int Door=0;
String numstr=""; //Num的字符串形式
#include <SoftwareSerial.h>
#include "ESP8266.h"
SoftwareSerial mySerial(10,11); // RX, TX
SoftwareSerial rdm6300(12,13);
ESP8266::wifi(mySerial);
////////id钥匙串读取id以及对应解码
void ReadID()
{
    int i=0;
    int ii; 
    char temp;
   
    for(i=0;(Serial.available()>0);i++)// 串口空闲
    //while(Serial.available()>0)
   // while (1)
    {   
      Serial.println("Read_id here");
      temp=rdm6300.read();
      delay(2);
      rdm6300.println(i);
      if(temp==0X02)  //接收起始位
        {
         flag=1;i=0;RX_Flag=0;//
        }
      if(flag==1)//检测到起始位,开始接收数据
      {
        if(temp==0X03)//检测到结束码,
        {
         flag=0;  //标志清零
         if(i==13) RX_Flag=1;//第13位为结束码，收到数据，标志置1
         else RX_Flag=0;
             for(ii=5;ii<11;ii++)//数据解码，6位字符转为十六进制数,对应十进制卡号
                  {
                    Num<<=4;
                      if(Code[ii]>64)  Num+=((Code[ii])-55);
                      else Num+=((Code[ii])-48);
                     // Serial.println(Num);
                  }
                //Num转字符串 
                numstr="";
                while (Num!=0)
                {
                  numstr=(char)((Num % 10)+48)+numstr;
                  Num=Num/10;
                }
                Serial.print("Now id is ");
                Serial.println(numstr);     
         break;
        }
         Code[i]=temp;
      } 
   
   }       
     flag=0;//标志清零
}
//////////////////////////////////////////////
////////////////////////舵机控制
void pulse( int serpin,int angle)
{
  int width;                             //舵机输出脉宽
  width=(angle*11)+500;                  //将角度转化为500-2480的脉宽值
  digitalWrite(serpin,HIGH);             //将舵机接口电平至高
  delayMicroseconds(width);              //延时脉宽值的微秒数
  digitalWrite(serpin,LOW);              //将舵机接口电平至低
  delay(20-width/1000);
}
///////////////////////////////////////////////
/////////////////进行舵机转动操作
void door()
{
   RX_Flag=0;
   ReadID();
   while(Serial.read()>0);//清除缓存区
   Serial.println("door here!");
   RX_Flag=0;//标志位清零
   Num=0;//数据清零
  
}
////////////////////////////////////////////////
void setup() {
  pinMode(serpin,OUTPUT);                         //舵机设置
  Serial.begin(9600);
  while (!Serial){
    ;
  }
  //ESP8266初始化
    Serial.print("setup begin\r\n");             // 在这个串口使用print数字没问题
  if (wifi.setOprToStationSoftAP()) {             // 切换模式为单SAT
    Serial.print("to station + softap ok\r\n");
  } else {
    Serial.print("to station + softap err\r\n");
  }

  if (wifi.joinAP(SSID, PWD)) {                   //加入无线网
    Serial.print("Join AP success\r\n");  
  } else {
    Serial.print("Join AP failure\r\n");
  }

  if (wifi.disableMUX()) {                        // 作用为启动单线模式
    Serial.print("single ok\r\n");
  } else {
    Serial.print("single err\r\n");
  }

  Serial.print("setup end\r\n");
  Serial.println("ready!");
}

void loop() {       
   getPassID();
   Serial.println("OK!!!");
   //delay(5000);
   //door(); 
   delay(5000);
}
void getPassID() {  
  //long ID=0; 
  passid="";
  if (wifi.createTCP(HOST_NAME, HOST_PORT)) { //建立TCP连接，如果失败，不能发送该数据    
    Serial.print("create tcp ok\r\n");    
///////////////////构建get报头
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
    postString+="\r\n";
  uint8_t buffer[512] = {0};                                  //储存回应数据
  const char *postArray = postString.c_str();                 //将str转化为char数组
  //Serial.println(postArray);
  wifi.send((const uint8_t*)postArray, strlen(postArray));    //send发送命令，参数必须是这两种格式，尤其是(const uint8_t*)
  Serial.println("send success");   
  uint32_t len = wifi.recv(buffer, sizeof(buffer), 10000);    //输出数据
    if (len > 0) {     
   Serial.println("here is the data");                                                                              
          uint32_t head=285;
          while (1)                                            //筛选passID
          {
              head++;
              if ((char)buffer[head]=='l')
              {
                  head+=6;
                  while ((char)buffer[head]!='\"')
                  {                 
                    Serial.print((char)buffer[head]);
                    passid=passid+(char)buffer[head];
                    head++;
                  }
                  break;
              }
              
          }
          Serial.println();
          Serial.println("door begin in 5 seconds!");
          delay(5000);
          door();
    }
         if (wifi.releaseTCP()) {                                 //释放TCP连接
        Serial.print("release tcp ok\r\n");
     } 
     else {
        Serial.print("release tcp err\r\n");
     }
      postArray = NULL;                                           //清空数组，等待下次传输数据
  } 
  else {
    Serial.print("create tcp err\r\n");
  }
}
