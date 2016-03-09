
#define MOTOR_ENABLE   5
#define MOTOR_LEFT     6
#define MOTOR_RIGHT    7

#define MOTOR_ENC_A    3
#define MOTOR_ENC_B    2

#define MOTOR_MAX_RPM  220

//#define DEBUG

String inString = "";

int realRPM, desiredRPM, correctedRPM = 0;
int error, integralError = 0;
int Kp, Ki = 1;

bool runMotor = false;

void showHelp (){
  Serial.println ("P3: Motor Control"); 
  Serial.println ("1 - Show help");
  Serial.println ("2 - Start motor");
  Serial.println ("3 - Stop motor");
  Serial.println ("4 - Increment RPM in 10");
  Serial.println ("5 - Decrement RPM in 10");
}

void setRPM (int rpm){
  desiredRPM = max( min (rpm, MOTOR_MAX_RPM), 0);
  Serial.print ("Desired RPM setted to ");
  Serial.println (desiredRPM);
}

void startMotor (){
  runMotor = true;
  setRPM (desiredRPM);
  digitalWrite (MOTOR_LEFT,   HIGH);
  digitalWrite (MOTOR_RIGHT,  LOW);
}

void stopMotor (){
  runMotor = false;
  analogWrite (MOTOR_ENABLE, 0);
}

int getOption (){
  int option = -1;
  while (Serial.available() > 0){
    int inChar = Serial.read();
    if (isDigit(inChar))  inString += (char)inChar;
    
    if (inChar == '\n') {
      option = inString.toInt();
      inString = "";
    }
  }
  return option;
}

void handleOption (){
  switch (getOption ()){
    case -1: break;
    case 1:{                                          showHelp ();              break; }
    case 2:{  Serial.println ("Starting motor");      startMotor();             break; }
    case 3:{  Serial.println ("Stop motor");          stopMotor();              break; }
    case 4:{  Serial.println ("Increment RPM in 10"); setRPM (desiredRPM + 10); break; }
    case 5:{  Serial.println ("Decrement RPM in 10"); setRPM (desiredRPM - 10); break; }
  }
}

void printData (){
  #ifdef DEBUG
    Serial.print ("desired RPM: ");   Serial.println (desiredRPM);
    Serial.print ("real RPM: ");      Serial.println (realRPM);
    Serial.print ("corrected RPM: "); Serial.println (correctedRPM);
    Serial.print ("error: ");         Serial.println (error);
    Serial.print ("integralError: "); Serial.println (integralError);
    Serial.println ("");
  #else
    Serial.print (desiredRPM);    Serial.print (",");
    Serial.print (realRPM);       Serial.print (",");
    Serial.print (correctedRPM);  Serial.print (",");
    Serial.print (error);         Serial.print (",");
    Serial.println (integralError);
  #endif
}

void setup() {
  Serial.begin(115200);
  
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(MOTOR_LEFT,   OUTPUT);
  pinMode(MOTOR_RIGHT,  OUTPUT);
  
  pinMode (MOTOR_ENC_A, INPUT);
  pinMode (MOTOR_ENC_B, INPUT);

    
  digitalWrite (MOTOR_ENC_B, HIGH);

  setRPM (120);
  
  showHelp ();
}


/*
 *  360 polsos = 1 revolució 
 *  pulseBlength = microsegons que dura un pols 
 *  1/((pulseBength*180/1000000)/60) 
 */


void loop() {
  handleOption ();

  if (runMotor){    
    // velocity=readEencoder();
    realRPM = 1 / ((pulseIn (MOTOR_ENC_B, HIGH) * 3 / 1000000.0 ));

    // error=velocitySetpoint-velocity;
    error = desiredRPM - realRPM;

    // integralError=integralError+error;
    integralError = integralError + (desiredRPM - realRPM);    

    // controlSignal=Kp*error+Ki*IntegralError;
    correctedRPM = desiredRPM + ((Kp * error) + (Ki * integralError));

    // applyInActuators(controlSignal);
    analogWrite (MOTOR_ENABLE, min(correctedRPM,255));

    // en cas d'overflow, corregim els càlculs fets
    if (correctedRPM > 255) integralError -= (desiredRPM - realRPM);

    printData ();
  }

  delay (100);
}
