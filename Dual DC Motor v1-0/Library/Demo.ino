/*******************************************************************
  This is a quick demo of the features of the SPI_Motor library 
          for Hootie81's Dual DC motor driver Shield
 

*******************************************************************/

#include "application.h"

//include the library....
#include "SPI_Motor.h"

// First we create an instance for each card, basically give the card a name 
// the format below is (CS pin number, card number, total cards)

//SPI_Motor pumpCard(A0, 1, 2);  //this is the bottom card 
//SPI_Motor stirCard(A0, 2, 2);  //this is the one above
SPI_Motor singleCard(A1);  //this card has a different CS line the library automagicly assumes card 1 of 1


void setup() {
    
    //open up the Serial port so we can see the demo output
    Serial.begin(115200);
    while (!Serial.available()) SPARK_WLAN_Loop();
    //push any key to continue
    
    // turn on the Communications with the first card and test it, this does a check to ensure 
    // data is able to pass through all the cards in the daisy-chain and make it back to the core. 
    // as the data does the loop it counts the total number of cards and compares to the number
    // specified when the instance was created.
    /*
    if (pumpCard.begin()){
        Serial.println("Pump card Initialised!");
    }
    //turn on the Communications with the second card and test it 
    if (stirCard.begin()){
        Serial.println("Stir card Initialised!");
    }
    */
    //turn on the Communications with the singleCard and test it 
    if (singleCard.begin()){
        Serial.println("Single card Initialised!");
    }
    
    //make sure they are all turned on, they should start in low power mode!
    //pumpCard.resume();
    //stirCard.resume();
    singleCard.resume();
    
    //set all 6 motors to STOP. they should be anyway but making sure we know the state
    //A soft reset of the core will leave pwm/direction data in the motor card
    //pumpCard.A(STOP, 0); 
    //pumpCard.B(STOP, 0);
    //stirCard.A(STOP, 0);
    //stirCard.B(STOP, 0);
    singleCard.A(STOP, 0);
    singleCard.B(STOP, 0);
     
    /******************************************************************************* 
    In the above lines.... format as follows
    
     the 'name' of the card eg: pumpCard
     a period '.' 
     the port on the card A or B
     in brackets the Command and/or PWM
    
     Commands are STOP, BRAKE, CW and CCW 
     PWM is 0-255 and only effects CW and CCW 
    *******************************************************************************/
    
}

void loop() {
    /* 
    Serial.println("Starting Motors in 5sec");
    delay(5000);
    
    Serial.println("Running Motors now 80% power for 5 seconds");
    
    pumpCard.A(CW,200);
    pumpCard.B(CCW,200);
    stirCard.A(CW,200);
    stirCard.B(CCW,200);
    //singleCard.A(CW, 200);
    //singleCard.B(CCW, 200);
    delay(5000);
    
    Serial.println("Motor A Brake, Motor B Freewheel");
    
    pumpCard.A(BRAKE);
    pumpCard.B(STOP);
    stirCard.A(BRAKE,200); //doesn't matter if we send PWM too, makes no difference
    stirCard.B(STOP,200);  //doesn't matter if we send PWM too, makes no difference
    //singleCard.A(BRAKE, 0);
    //singleCard.B(STOP, 0);
    delay(5000);
    
    Serial.println("Reversing Motor directions now");
    
    pumpCard.A(CCW,200);
    pumpCard.B(CW,200);
    stirCard.A(CCW,200);
    stirCard.B(CW,200);
    //singleCard.A(CCW, 200);
    //singleCard.B(CW, 200);    
    delay(5000);
    
    Serial.println("Stopping all motors");
    
    pumpCard.A(STOP, 0); //sets pwm to minimum ready for next operation
    pumpCard.B(STOP, 0);
    stirCard.A(STOP, 0);
    stirCard.B(STOP, 0);
    //singleCard.A(STOP, 0);
    //singleCard.B(STOP, 0);
    delay(5000);
    
    Serial.println("Ramping up all motors now");
    
    pumpCard.A(CW); //setting just the direction, PWM will be same last time it was set 
    pumpCard.B(CW);
    stirCard.A(CW);
    stirCard.B(CW);
    //singleCard.A(CW);
    //singleCard.B(CW);    
    
    int i;
    for(i=0; i<255; i++){  //create a loop with a increasing number
    pumpCard.A(i);
    pumpCard.B(i);
    stirCard.A(i);
    stirCard.B(i);
    //singleCard.A(i);
    //singleCard.B(i); 
    delay(115); //ramp from 0 to 255 should take about 30sec (maybe as a guess!!)
    }
    
    Serial.println("Ramping Down all motors now");
    for(i=255; i>0; i--){
    pumpCard.A(i);
    pumpCard.B(i);
    stirCard.A(i);
    stirCard.B(i);
    //singleCard.A(i);
    //singleCard.B(i); 
    delay(115); //ramp from 0 to 255 should take about 30sec (maybe as a guess!!)
    }
    
    Serial.println("Putting all cards to low power mode now");
    
    pumpCard.standby();
    stirCard.standby();
    //singleCard.standby();
    Serial.println("Check power consumption now.. quick you have 10seconds!");
    delay(10000);
    
    Serial.println("Putting all cards to Normal/Run mode now");
    
    pumpCard.resume();
    stirCard.resume();
    //singleCard.resume();
    
    //return to the top of loop and do it again
    */
    
    
    
    /* Stepper Test Section */
    int t; //delay time  
    int i; //general counter
    int a = 0; //Step number
     
    Serial.println("CW");
    
    for(t=10; t>4; t--){ // speed up
        for(i=1; i<100; i++){ 
            a++;
            singleCard.Stepper(a, 255);
            delay(t);    
        }
    }
    for(t=4; t<10; t++){ // slow down
        for(i=1; i<100; i++){ 
            a++;
            singleCard.Stepper(a, 255);
            delay(t);    
        }
    }
    Serial.println("CCW");  
    
    for(t=10; t>4; t--){ //speed up
        for(i=1; i<100; i++){  
            a--;
            singleCard.Stepper(a, 255);
            delay(t);    
        }
    }
    for(t=4; t<10; t++){ //slow down
        for(i=1; i<100; i++){
            a--;
            singleCard.Stepper(a, 255);
            delay(t);    
        }
    }
    //end of stepper test
    
    
      
 
    
}