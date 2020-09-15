
const int PIN_TRIGGER = 6;
const int PIN_ECHO = 5;

unsigned int tiempo, distancia;

unsigned int accX, accY, accZ;
unsigned int prevX, prevY, prevZ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_TRIGGER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_TRIGGER, LOW);
  delayMicroseconds(100);

  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);
  
  tiempo = pulseIn(PIN_ECHO, HIGH);
  distancia = tiempo / 58;
//  Serial.print(distancia);
//  Serial.println(" cm");
  Serial.print('D');
  Serial.print(tiempo);
//  Serial.println(" t");
  accX = analogRead(A0);
  accY = analogRead(A1);
  accZ = analogRead(A2);

  prevX = (accX + prevX + prevX + prevX) * 0.25;
  prevY = (accY + prevY + prevY + prevY) * 0.25;
  prevZ = (accZ + prevZ + prevZ + prevZ) * 0.25;

  Serial.print("X");
  Serial.print(prevX);
  Serial.print("Y");
  Serial.print(prevY);
  Serial.print("Z");
  Serial.println(prevZ);

  // senal estandar (cero) = 340
  // senal -1g = 270
  // 1g = 70
  

  delay(100);
}
