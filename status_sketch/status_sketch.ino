// Variables used to store messages coming through from serial port
String inputString = "";
// used to signify to loop() that the data has stopped coming through the Serial port
boolean stringComplete = false;
// TODO: check that we actually need long here
long data[15];

int offTime = 200;

// pins
int lowE = 13;
int medE = 12;
int higE = 11;

int lowK = 10;
int medK = 9;
int higK = 8;

int lowH = 7;
int medH = 6;
int higH = 5;

int butE = 4;
int butK = 3;
int butH = 2;

int butState = 0;

struct siteData {
  char idx;  // status of the site (1 = green, 2 = yellow, 3 = red)
  int cpu;   // current cpu%
  int ram;   // current ram%
  int app;   // current appdex
  int ccu;   // indication of concurrent users, converted to milliseconds to have LED on for
  double ela;  // timestamp for timings
  int val; // current setting of LED (HIGH/1 or LOW/0)
};

siteData siteE;
siteData siteK;
siteData siteH;

void setup()
{
  Serial.begin(9600);
  inputString.reserve(200);
  pinModes();
  defaultSites();  
}

void loop()
{
  if (stringComplete) {
    setData();
  }     

  if (digitalRead(butE) == HIGH) {
    butState = 1;
  } else if (digitalRead(butK) == HIGH) {
    butState = 2;
  } else if (digitalRead(butH) == HIGH) {
    butState = 3;
  } else {
    butState = 0;
  }
  if (butState > 0) {
    // here we match the values in the array to whate we need.
    // rather than having massive if structures, simply copy the struct we need into a temp struct and access that
    siteData siteT;
    if (butState == 1) {
      // EWN
      siteT = siteE;
    } else if (butState == 2) {
      // Kfm
      siteT = siteK;
    } else if (butState == 3) {
      // Highveld
      siteT = siteH;
    }
    // Show status of CPU
      if (siteT.cpu <=  40) {
        digitalWrite(lowE, HIGH);
        digitalWrite(medE, LOW);
        digitalWrite(higE, LOW);
      } else if (siteT.cpu > 40 && siteT.cpu < 70) {
        digitalWrite(lowE, LOW);
        digitalWrite(medE, HIGH);
        digitalWrite(higE, LOW);
      } else if (siteT.cpu >= 70) {
        digitalWrite(lowE, LOW);
        digitalWrite(medE, LOW);
        digitalWrite(higE, HIGH);
      }

    // Show statu of RAM
      if (siteT.ram <= 40) {
        digitalWrite(lowK, HIGH);
        digitalWrite(medK, LOW);
        digitalWrite(higK, LOW);
      } else if (siteT.ram > 40 && siteT.ram < 70) {
        digitalWrite(lowK, LOW);
        digitalWrite(medK, HIGH);
        digitalWrite(higK, LOW);
      } else if (siteT.ram >= 70) {
        digitalWrite(lowK, LOW);
        digitalWrite(medK, LOW);
        digitalWrite(higK, HIGH);
      }

    // Show status of Appdex
       if (siteT.app <= 40) {
        digitalWrite(lowH, HIGH);
        digitalWrite(medH, LOW);
        digitalWrite(higH, LOW);
      } else if (siteT.app > 40 && siteT.app < 70) {
        digitalWrite(lowH, LOW);
        digitalWrite(medH, HIGH);
        digitalWrite(higH, LOW);
      } else if (siteT.app >= 70) {
        digitalWrite(lowH, LOW);
        digitalWrite(medH, LOW);
        digitalWrite(higH, HIGH);
      }
   
  } else {

    double tnow = millis();
    if (siteE.idx == 1) {
      digitalWrite(lowE, siteE.val);
      digitalWrite(medE, LOW);
      digitalWrite(higE, LOW);
    } else if (siteE.idx == 2) {
      digitalWrite(lowE, LOW);
      digitalWrite(medE, siteE.val);
      digitalWrite(higE, LOW);
    } else if (siteE.idx == 3) {
      digitalWrite(lowE, LOW);
      digitalWrite(medE, LOW);
      digitalWrite(higE, siteE.val);
    }
    if (siteK.idx == 1) {
      digitalWrite(lowK, siteK.val);
      digitalWrite(medK, LOW);
      digitalWrite(higK, LOW);
    } else if (siteK.idx == 2) {
      digitalWrite(lowK, LOW);
      digitalWrite(medK, siteK.val);
      digitalWrite(higK, LOW);
    } else if (siteK.idx == 3) {
      digitalWrite(lowK, LOW);
      digitalWrite(medK, LOW);
      digitalWrite(higK, siteK.val);
    }
    if (siteH.idx == 1) {
      digitalWrite(lowH, siteH.val);
      digitalWrite(medH, LOW);
      digitalWrite(higH, LOW);
    } else if (siteH.idx == 2) {
      digitalWrite(lowH, LOW);
      digitalWrite(medH, siteH.val);
      digitalWrite(higH, LOW);
    } else if (siteH.idx == 3) {
      digitalWrite(lowH, LOW);
      digitalWrite(medH, LOW);
      digitalWrite(higH, siteH.val);
    }
  
    if (siteE.val == 0) {
      if ((tnow - siteE.ela) > offTime) {
        siteE.val = (~(siteE.val&1))&(siteE.val|1);
        siteE.ela = tnow;
      }  
    } else {
      if ((tnow - siteE.ela) > siteE.ccu) {
        siteE.val = (~(siteE.val&1))&(siteE.val|1);
        siteE.ela = tnow;
      }  
    }
    
    if (siteK.val == 0) {
      if ((tnow - siteK.ela) > offTime) {
        siteK.val = (~(siteK.val&1))&(siteK.val|1);
        siteK.ela = tnow;
      }  
    } else {
      if ((tnow - siteK.ela) > siteK.ccu) {
        siteK.val = (~(siteK.val&1))&(siteK.val|1);
        siteK.ela = tnow;
      }  
    }
    
    if (siteH.val == 0) {
      if ((tnow - siteH.ela) > offTime) {
        siteH.val = (~(siteH.val&1))&(siteH.val|1);
        siteH.ela = tnow;
      }  
    } else {
      if ((tnow - siteH.ela) > siteH.ccu) {
        siteH.val = (~(siteH.val&1))&(siteH.val|1);
        siteH.ela = tnow; 
      }  
    }

  }

}

void setData() {
  
    int numArgs = 0;
    
    int beginIdx = 0;
    int idx = inputString.indexOf(",");
    
    String arg;
    char charBuffer[16];
    
    while (idx != -1)
    {
        arg = inputString.substring(beginIdx, idx);
        arg.toCharArray(charBuffer, 16);
    
        data[numArgs++] = atol(charBuffer);
        beginIdx = idx + 1;
        idx = inputString.indexOf(",", beginIdx);
    }
    arg = inputString.substring(beginIdx);
    arg.toCharArray(charBuffer, 16);
    data[numArgs++] = atol(charBuffer);

    siteE.idx = data[0];
    siteE.cpu = data[1];
    siteE.ram = data[2];
    siteE.app = data[3];
    siteE.ccu = data[4];
    siteE.ela = millis();

    siteK.idx = data[5];
    siteK.cpu = data[6];
    siteK.ram = data[7];
    siteK.app = data[8];
    siteK.ccu = data[9];
    siteK.ela = millis();

    siteH.idx = data[10];
    siteH.cpu = data[11];
    siteH.ram = data[12];
    siteH.app = data[13];
    siteH.ccu = data[14];
    siteH.ela = millis();

    inputString = "";
    stringComplete = false;
 
}

void pinModes() {
  pinMode(lowE, OUTPUT);
  pinMode(medE, OUTPUT);
  pinMode(higE, OUTPUT);
  pinMode(lowK, OUTPUT);
  pinMode(medK, OUTPUT);
  pinMode(higK, OUTPUT);
  pinMode(lowH, OUTPUT);
  pinMode(medH, OUTPUT);
  pinMode(higH, OUTPUT);
  pinMode(butE, INPUT);
  pinMode(butK, INPUT);
  pinMode(butH, INPUT);
}

void defaultSites() {

  siteE.idx = 1;
  siteE.cpu = 20;
  siteE.ram = 20;
  siteE.app = 20;
  siteE.ccu = 1000;
  siteE.ela = millis();
  siteE.val = 1;
  
  siteK.idx = 1;
  siteK.cpu = 20;
  siteK.ram = 20;
  siteK.app = 20;
  siteK.ccu = 1000;
  siteK.ela = millis();
  siteK.val = 1;

  siteH.idx = 1;
  siteH.cpu = 20;
  siteH.ram = 20;
  siteH.app = 20;
  siteH.ccu = 1000;
  siteH.ela = millis();
  siteH.val = 1;

}


void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
    if (inChar == ';') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }
}




