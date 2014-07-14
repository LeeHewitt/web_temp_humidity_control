double RH, T_C;
int H_dat, T_dat;
int RHplot, T_Cplot;
byte   _status;
int heatpin = D4;
int waterpin = D7;
int tempsetting = 70;
int rhsetting = 30;
int tempreceived;
int rhreceived;
unsigned long LastUpTime = 0;
unsigned long lastwatertime = 0;
char resultstr[64];

void setup() {
    pinMode(heatpin, OUTPUT);
    pinMode(waterpin, OUTPUT);
    Spark.variable("rhreading", &RH, DOUBLE);
    Spark.variable("tempreading", &T_C, DOUBLE);
    Spark.variable("tempreceived", &tempreceived, INT);
    Spark.variable("rhreceived", &rhreceived, INT);
    Spark.function("tempsetting", setTemp);
    Spark.function("rhsetting", setRH);
    Spark.variable("result", &resultstr, STRING); 
    Wire.begin();
    delay(3000);
}

void loop() {
    
    _status = fetch_humidity_temperature(&H_dat, &T_dat);
    RH = (float) H_dat * 6.10e-3;
    T_C = (float) T_dat * 1.007e-2 - 40.0;

    if(T_C<tempsetting){
        digitalWrite(heatpin,HIGH);
        if(RH<rhsetting){
            if (millis()-lastwatertime>5000){
            digitalWrite(waterpin,HIGH);
            delay(200);
            digitalWrite(waterpin,LOW);
            lastwatertime = millis();
            }
        }
        else digitalWrite(waterpin,LOW);
    }
    else digitalWrite(heatpin,LOW);
    tempreceived = tempsetting;
    rhreceived = rhsetting;
    T_Cplot = (int)T_C;
    RHplot = (int)RH;
    sprintf(resultstr, "{\"T_Cplot\":%d,\"RHplot\":%d}", T_Cplot, RHplot); 
   delay(1000);
}


byte fetch_humidity_temperature(int *p_H_dat, int *p_T_dat)
{
  byte address, Hum_H, Hum_L, Temp_H, Temp_L, _status;
  int H_dat, T_dat;
  address = 0x27;
  ;
  Wire.beginTransmission(address); 
  Wire.endTransmission();
  delay(100);

  Wire.requestFrom((int)address, (int) 4);
  Hum_H = Wire.read();
  Hum_L = Wire.read();
  Temp_H = Wire.read();
  Temp_L = Wire.read();
  Wire.endTransmission();

  _status = (Hum_H >> 6) & 0x03;
  Hum_H = Hum_H & 0x3f;
  H_dat = (((int)Hum_H) << 8) | Hum_L;
  T_dat = (((int)Temp_H) << 8) | Temp_L;
  T_dat = T_dat / 4;
  *p_H_dat = H_dat;
  *p_T_dat = T_dat;
  return(_status);
}

int setTemp(String value){
    tempsetting = value.toInt();
    return 0;
}

int setRH(String value){
    rhsetting = value.toInt();
    return 0;
}
