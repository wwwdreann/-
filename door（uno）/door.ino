String passID;
int flag=0,RX_Flag=0;//串口标志
char Code[14]; //用于存放读到串口数据
long Num=0;//解码数据
int serpin=7;//舵机引脚定义
int Door=0;
#include <Wire.h>

void Read_ID(void)
{
    int i=0;
    char temp;
    for(i=0;(Serial.available()>0);i++)// 串口空闲
    //while(Serial.available()>0)
    {
      temp=Serial.read();
      //Serial.print(temp);
      delay(2);
      //Serial.println(i);
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
       break;
        }
         Code[i]=temp;
      } 
    }       
     flag=0;//标志清零
}
 
void pulse( int serpin,int angle)
{
  int width;//舵机输出脉宽
  width=(angle*11)+500;//将角度转化为500-2480的脉宽值
  digitalWrite(serpin,HIGH);//将舵机接口电平至高
  delayMicroseconds(width);//延时脉宽值的微秒数
  digitalWrite(serpin,LOW);//将舵机接口电平至低
  delay(20-width/1000);
}

void setup() 
{
  pinMode(12,OUTPUT);
  pinMode(serpin,OUTPUT);
  Wire.begin();
  Serial.begin(9600);
  Serial.println("This is a test for access control system");
  delay(100);
 // Serial.println(Password1);
}
void updata() //更新passID
{
  passID="";
  Wire.requestFrom(8,7);
  while (Wire.available()) { // slave may send less than requested
    passID = passID+(char)Wire.read(); // receive a byte as character
  }  
  //if (passID!="") Serial.println("Updata ok!");
  Serial.println("passID is");
  Serial.println(passID);
}
void loop()
{
  int i;
  long temp=0,time=0;
  RX_Flag=0;
  while(1)
  {
   Read_ID();
  if(RX_Flag==1)
  {
    for(i=5;i<11;i++)//数据解码，6位字符转为十六进制数,对应十进制卡号
    {
      Num<<=4;
        if(Code[i]>64)  Num+=((Code[i])-55);
        else Num+=((Code[i])-48);
       // Serial.println(Num);
    }    
   // Serial.println(Num);    
  String numstr="";
  while (Num!=0)
  {
    numstr=(char)((Num % 10)+48)+numstr;
    Num=Num/10;
  }
  Serial.println("numstr is");
  Serial.println(numstr);
  updata();
  if(numstr==passID)//识别成功
  {
      digitalWrite(13,HIGH);
      delay(5);
      for(int i=0;i<=50;i++)//给予舵机足够的时间让它转到指定角度
     {
        pulse(serpin,20);//引用脉冲函数 
      }
      //delay(2000);
      while(analogRead(Door)>400);
      //delay(1000);
      digitalWrite(13,LOW);
      delay(5);
      for(int i=0;i<=50;i++)//给予舵机足够的时间让它转到指定角度
     {
        pulse(serpin,180);//引用脉冲函数
      }
   }
    while(Serial.read()>0);//清除缓存区
    RX_Flag=0;//标志位清零
    Num=0;//数据清零
   }   
  }
}

