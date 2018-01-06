#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include<DHT.h>

char ssid[] = "s008-flying";
char pass[] = "dianxin132";

#define  greenhouseID  "8e27efe0-eb81-11e7-8341-353f63eeab61"
#define  greenhouseKEY "i8j8gFS00lwaNzSOxrEe"

#define GPIO0 0
#define GPIO2 2
#define GPIO4 4

unsigned int httpPort = 8181;

const char *host = "121.42.180.30";

char  ServerAddr[] = "117.21.178.99";
char StateTopicAddr[] = "v1/devices/me/telemetry";
char ControllTopicAddr[] = "v1/devices/me/rpc/request/+";

WiFiClient wifiClient;
PubSubClient client(wifiClient);

int status = WL_IDLE_STATUS;

boolean gpioState[] = {false, false};

long last_time = 0;

void setup()
{
  Serial.begin(115200);
  delay(10);
  pinMode(GPIO2, OUTPUT);
  pinMode(GPIO0, OUTPUT);
  pinMode(GPIO4, OUTPUT);
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED)//WiFi.status() ，这个函数是wifi连接状态，返回wifi链接状态
  {
    delay(500);
    Serial.print(".");
  }//如果没有连通向串口发送.....

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());//WiFi.localIP()返回8266获得的ip地址
  Serial.println("mac address");
  Serial.println(WiFi.macAddress());

}

void loop()
{
  while (!wifiClient.connected())//几个非连接的异常处理
  {
    if (!wifiClient.connect(host, httpPort))
    {
      Serial.print("disconnect ");
      //client.stop();
      delay(500);
    }
    else
    {
      Serial.print("chickin\n");
      wifiClient.print("{\"M\":\"checkin\",\"ID\":\"3115\",\"K\":\"52e38a188\"}\r\n");
      delay(500);
    }

  }
  while (wifiClient.available())//，无线读取到的数据转发到到串口
  {
    toggle(GPIO4);
    String s = wifiClient.readString();
    String str = "";
    //    char a[100] = "";
    int count_f; //储存分离出来字符串
    int count_r; //储存分离出来字符串
    int num = 0; //储存云端下发的阀值

    //    for (int i = 0; i < 100; i++)
    //    {
    //      a[i] = s[i];
    //    }
    if ( s.indexOf("offOn") != -1 )
    {
      toggle(GPIO2);
    }
    else if ( s.indexOf("play") != -1 )
    {
      toggle(GPIO0);
    }
    Serial.println(s);
    count_f = s.indexOf("\"C\"");
    count_r = s.indexOf("\"T\"");
    if (count_f != -1) //如果有分隔符存在就向下执行
    {

      // Serial.println( s.substring(0, count_f)); //打印出第一个逗号位置的字符串
      str = s.substring(count_f + 5, count_r - 2); //打印字符串，从当前位置+5开始 得到不带引号的字符数字
//      Serial.println(str);
      char cstr[10] = "";
      for (int i = 0; i < 10; i++)
      {
        cstr[i] = str[i];
      }
      num = atoi(cstr);
      Serial.println(num);
    }

    delay(500);
  }
  //  if (Serial.available())//串口读取到的转发到wifi，因为串口是一位一位的发送所以在这里缓存完再发送
  //  {
  //    size_t counti = Serial.available();
  //    uint8_t sbuf[counti];
  //    Serial.readBytes(sbuf, counti);
  //    client.write(sbuf, counti);
  //    delay(500);
  //  }

  if (millis() - last_time > 2000) 
  {
    last_time = millis();
    String s = "{\"M\":\"update\",\"ID\":\"3115\",\"V\":{\"2948\":\"15.3\"}}\n";
    wifiClient.print(s);
    delay(1000);
    String t = "{\"M\":\"update\",\"ID\":\"3115\",\"V\":{\"2948\":\"30.5\"}}\n";
    wifiClient.print(t);
  }
}

void toggle(int GPIO)   //以灯的反转作为测试代码
{
  if ( HIGH == digitalRead(GPIO) )
  {
    digitalWrite(GPIO, LOW);
  }
  else
  {
    digitalWrite(GPIO, HIGH);
  }
}






