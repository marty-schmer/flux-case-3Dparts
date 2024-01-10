/*
Derrived from the work of Chris Strasburg, Ames Laboratory, US DOE <cstras@ameslab.gov>

Author: Myron Coleman, Agricultural Research Service, USDA, Lincoln NE
I used much of the logic and many variable/function names that Chris devloped.
My intention was to make the logic easier to read and follow.

Lines beginning with "Serial.xxxx" are intended to send text to a debugging window in the Arduino IDE.  You can delete those lines if you wish.

This code controls an automatic gas sampler.  Samples are collected from a chamber and stored in syringes.

Here's what should happen:
-Retract actuator (See sample time variable)
-Extend actuator (see sample time variable)
-Retract actuator (see sample time variable)
-Purge chamber twice
-collect 4 gas samples (purge before each sample) (wait ten minutes between)

*/

/*
  Pin mapping:
  DIO 8 - Extend Actuator (HIGH = OFF, LOW = ON)
  DIO 9 - Retract Actuator (HIGH = OFF, LOW = ON)
  DIO 2 - Solenoid #1 (LOW = Nothing, HIGH = Chamber)
  DIO 3 - Solenoid #2 (LOW = Vial, HIGH = Passthrough)
  DIO 4 - Solenoid #3 (LOW = Vial, HIGH = Passthrough)
  DIO 5 - Solenoid #4 (LOW = Vial, HIGH = Passthrough)
  DIO 6 - Solenoid #5 (LOW = Vial, HIGH = Passthrough)
  DIO 7 - Solenoid #6 (Low = Main Syringe to Sample Block | High Purge to Inside Orange Case)
*/

//Pin Definitions
const int ACTUATOR_X = 8; // Extend Actuator
const int ACTUATOR_R = 9; //Retract Actuator
const int SamplePurge = 7;  //valve that purges sample chamber to atmosphere of orange case

//Timing is expressed in millisecond (5 seconds = 5000 ms)
const unsigned long ACTUATOROPTIME = 2000 ; // Send signal for n/1000 seconds  **Adjust this up or down to change your collected air volume** 
const unsigned long ACTSIGDELAY = 100;     // half-second delay to ensure that two 'on' signals
                                           // are not sent to the actuator at the same time.
//Solenoid Definitions HIGH == passthrough, LOW == chamber/vial   //If valves behave inversely, swap these two around (change low to high and vise versa)
const int CHAMBERVIAL = LOW;
const int PASSTHROUGH = HIGH;

int currentSolenoid = 3;                      //start with the first sample valve ([0] is for the chamber/purge)

const unsigned long SAMPLEDELAY = 600000; // 10 minute delay between samples (600000)
//const unsigned long SAMPLEDELAY = 10000; // 10 second delay between samples (10000) FOR TESTING PURPOSES

void setup() {
   Serial.begin(9600); // Enable //Serial. Port Output For Debugging (Comment out if un-needed).
  
    int i=2;
    for (i = 0; i < 10; i++)  //Sets the state of relays 0-7 to passthrough
    {
        pinMode(i, OUTPUT);         //Set pin to output (needed to control valves)
        digitalWrite(i, PASSTHROUGH); //Set current solenoid to low/passthrough
    }
    
    //Retract Actuator in case it didn't close all the way before    
    digitalWrite(7,LOW);
    move_actuator(ACTUATOR_R, 5000); //retract for 5 seconds
    digitalWrite(7,HIGH);
    
    //Purge Once Here, and again before the first sample.  After this, one purge between samples.
    Serial.println("Begin");
    purge();

}

void loop() 
{
//Yes, I know This isn't as elegant as a for-loop.  I wanted to make it easier to read and follow.
//Each Time this loop executes, the program choses the next sample vial.
    switch (currentSolenoid) {
    
    case 3:
    {
        //Vial 1
        purge();
        delay(ACTSIGDELAY); //We need a very short break so the actuator doesn't get confused
        get_sample(currentSolenoid);
        
        currentSolenoid++;
        delay(SAMPLEDELAY);
        break;
    }
    case 4:
    {
        //Vial 2
        purge();
        delay(ACTSIGDELAY); //We need a very short break so the actuator doesn't get confused
        get_sample(currentSolenoid);
        
        currentSolenoid++;
        delay(SAMPLEDELAY);
        break;
    }
    case 5:
    {
        //Vial 3
        purge();
        delay(ACTSIGDELAY); //We need a very short break so the actuator doesn't get confused
        get_sample(currentSolenoid);
        
        currentSolenoid++;
        delay(SAMPLEDELAY);
        break;
    }
    case 6:
    {
        //Vial 4
        purge();
        delay(ACTSIGDELAY); //We need a very short break so the actuator doesn't get confused
        get_sample(currentSolenoid);
        
        currentSolenoid++;
        delay(SAMPLEDELAY);
        break;
    }
    default :
    {
        //This case is chosen after all four samples have been taken.  It's purgatory for the computer (until you reboot it)
        while (1)
        {
            Serial.println("Sleepy Time");
            delay(1000000); //sleep forever
        }
    }
    
}

}




void move_actuator(const int outpin, int runtime)
{
    //This function will turn on either side of the actuator relays.  Actuator_X will extend, Actuator_R will retract.
    //We have to drive the pin low to engage the direction pin we want, and we have to remember to turn it high when we're done.
       
    delay(100);
    digitalWrite(outpin, LOW);
    delay(runtime);
    digitalWrite(outpin, HIGH);
    delay(1000);
    
    }

void purge() {
    //This function will purge air from the chamber to the inside of the orange case
    Serial.println("Purge: Extend");
    move_actuator(ACTUATOR_X,5000); //Move main syringe for 5 seconds
    digitalWrite(SamplePurge, LOW);  
    Serial.println("Purge: Retract");
    move_actuator(ACTUATOR_R, 5000); //Move main syringe for 5 seconds
    delay(1000);
    digitalWrite(SamplePurge, HIGH);
}

void get_sample(const int currentvalve)
{
    //This Function will take a sample from the chamber and transfer it to the current valve/syringe (currentvalve)
    String out_message = "Current Sample: ";
    out_message += currentvalve-2;  //This makes the serial ouput show sample 1-4 instead of the valve number (3-7)
    Serial.println(out_message);
    Serial.println("Sample: Extend");
    move_actuator(ACTUATOR_X, ACTUATOROPTIME);                  //Extend Actuator
    digitalWrite(currentvalve,CHAMBERVIAL);     //Set Current Sample Valve to Vial
    Serial.println("Sample: Retract");
    move_actuator(ACTUATOR_R, ACTUATOROPTIME);                  //Transfer Sample to Current Syringe/Vial
    digitalWrite(currentvalve, PASSTHROUGH);    //Set Current Sample Valve to passthrough
        
   }
