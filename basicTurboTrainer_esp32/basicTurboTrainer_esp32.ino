// Adrian Pilkington 2025

// massively cut down turbo trainer, just runs relays to a set exercise program

#define RELAY_1_PIN 25
#define RELAY_2_PIN 26
#define RELAY_3_PIN 27
#define RELAY_4_PIN 14
#define NUMBER_OF_RELAYS 4
  
bool inExerciseProg = true;
int inExerciseProgCounter = true;

int relayPinArray[NUMBER_OF_RELAYS] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN};

#define INTERVAL_WARMUP 60
#define INTERVAL_STARTING 120
#define INTERVAL_INCREASING 240
#define INTERVAL_MAX_EFFORT_1 270
#define INTERVAL_BETWEEN    300
#define INTERVAL_MAX_EFFORT_2 330
#define INTERVAL_COOLDOWN 360
#define INTERVAL_END 390

void setup()
{
  pinMode(RELAY_1_PIN, OUTPUT);
  digitalWrite(RELAY_1_PIN, LOW);
  pinMode(RELAY_2_PIN, OUTPUT);
  digitalWrite(RELAY_2_PIN, LOW);
  pinMode(RELAY_3_PIN, OUTPUT);
  digitalWrite(RELAY_3_PIN, LOW);
  pinMode(RELAY_4_PIN, OUTPUT);
  digitalWrite(RELAY_4_PIN, LOW);

  Serial.begin(115200);

  // cycle the realsy to as an audible check everythings running 
  Serial.println("Cycling all relays");

  for (int i = 0; i <= NUMBER_OF_RELAYS; i++)
  {
    Serial.print("relay ");
    Serial.print(i);
    Serial.println(" off");    
    digitalWrite(relayPinArray[i], LOW);          
    digitalWrite(relayPinArray[i], HIGH);         
    Serial.print("relay ");
    Serial.print(i);
    Serial.println(" on"); 
    sleep(1);
    digitalWrite(relayPinArray[i], LOW);          
    Serial.print("relay ");
    Serial.print(i);
    Serial.println(" off again");    
  }
  sleep(2);
}



void loop()
{
  Serial.print(inExerciseProgCounter);
  Serial.print(" ");
  // inExerciseProg causes the relays to be set in a predefined sequence
  if (inExerciseProg == true)
  {
     if (inExerciseProgCounter < INTERVAL_WARMUP)
     {
         Serial.println("Warmup");
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, LOW);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < INTERVAL_STARTING)
     {
        Serial.println("Increasing:)");
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < INTERVAL_INCREASING)
     {
        Serial.println("More Effort!");
         digitalWrite(RELAY_1_PIN, HIGH);
         digitalWrite(RELAY_2_PIN, HIGH);
         digitalWrite(RELAY_3_PIN, LOW);
         digitalWrite(RELAY_4_PIN, LOW);
     }
     else if (inExerciseProgCounter < INTERVAL_MAX_EFFORT_1)
     {
        Serial.println("Max Effort!");
         digitalWrite(RELAY_1_PIN, HIGH);
         digitalWrite(RELAY_2_PIN, HIGH);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < INTERVAL_BETWEEN)
     {
        Serial.println("Increasing:)");
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }
     else if (inExerciseProgCounter < INTERVAL_MAX_EFFORT_2)
     {
        Serial.println("Max Effort!");
         digitalWrite(RELAY_1_PIN, HIGH);
         digitalWrite(RELAY_2_PIN, HIGH);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }          
     else if (inExerciseProgCounter < INTERVAL_COOLDOWN)
     {
        Serial.println("Cooldown 1");
         digitalWrite(RELAY_1_PIN, LOW);
         digitalWrite(RELAY_2_PIN, LOW);
         digitalWrite(RELAY_3_PIN, HIGH);
         digitalWrite(RELAY_4_PIN, HIGH);
     }

     inExerciseProgCounter++;


     if (inExerciseProgCounter > INTERVAL_END)
     {
        inExerciseProgCounter = 0;
         Serial.println("Restart!");
     }
  }
  sleep(1);
}
