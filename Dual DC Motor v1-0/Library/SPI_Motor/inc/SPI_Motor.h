/******************************************************************************* 
  This is a library for Hootie81's Dual DC Motor Shield for Spark Core

 The Dual DC Motor Shield uses a Maxim MAX6966 SPI PWM I/O expander 
 to drive a Toshiba TB6612FNG Dual DC motor Driver. The MAX6966 has a SPI
 Interface and allows shields to be stacked in a daisy chain configuration
 minimizing the required I/O. The MAX6966 also has a built in 32kHz PWM 
 controller saving the Spark more precious I/O.
 The TB6612FNG is a dual H-Bridge motor controller allowing forward and reverse
 directions and PWM speed control. There is short circuit brake function and 
 free wheel functions also. The TB6612FNG will drive 2 DC motors at 
 up to 1.2A continuous and 3.2A peak.
 
 The Card can be configured with solder jumpers, there are 4 options for CS on 
 the underside of the card - A0, A1, D0, D1
 
 on the top side you can choose if the card it the first card and or the last 
 card. This sets up the MISO And MOSI lines to pass from card to card.
 For single card choose Y for both First and Last.
 For multiple cards, 
    on the bottom most card - choose Y for First, and N for Last
    middle cards will be N for both First and Last
    on the top most card choose N for First and Y for last.
  
 Card Number will be 1 for the bottom most card, the upper limit it yet to 
 be determined, but will probably be limited by RAM on the core or Lag in motor
 control from the need to clock data through multiple cards.
 
  *** All motor cards must use the same CS line, as the MAX6966 SPI output 
 doesn't go high impedance ***
 
  Written by Chris Huitema. 30/7/14 
  
 ******************************************************************************/

#include "application.h"


enum direction_t {
    STOP, CW, CCW, BRAKE
}; //these are the control options

class SPI_Motor {
public:

    /***************************************************************************
     Constructors:
       Takes CS line for the card
       Sets the card number and total cards if more than one card
       used in daisy-chain mode. 
     **************************************************************************/

    SPI_Motor(uint8_t); // sets CS and assumes single card 
    SPI_Motor(uint8_t, uint8_t, uint8_t); // CS, Card number, total cards

    /***************************************************************************
     Begin:
       Sets up the CS Port as output and sets high
       Sets up the SPI
       Checks the total number of cards is correct, returns false if not
     **************************************************************************/
    
    boolean begin(void);

    /***************************************************************************
     Motor functions:
        3 different options
            Control word and PWM
            Control word only
            PWM only (faster speed changes as only PWM register changed)
        Control words are:
            STOP    this sets the motor in free wheel mode
            BRAKE   this short circuits the motor pins causing a braking effect
            CW      makes the motor turn in a CW direction
            CCW     makes the motor turn in a CCW direction
       
       All functions will return true is SPI return data looks correct
     **************************************************************************/

    boolean Stepper(uint32_t, uint8_t);
    boolean Stepper(uint32_t);
    boolean Stepper(uint8_t);
    boolean A(direction_t, uint8_t);
    boolean A(direction_t);
    boolean A(uint8_t);
    boolean B(direction_t, uint8_t);
    boolean B(direction_t);
    boolean B(uint8_t);
    
    /***************************************************************************
     Power save options:
       standby() sets the motor controller to low power mode.
       resume()  puts back to normal operation.
      All functions will return true is SPI return data looks correct
     **************************************************************************/

    boolean standby(void); // puts the motor controller and IO chip to low power
    boolean resume(void); // puts both back to normal operation.


private:

    enum CMD {
        //These are the control registers for the ports on the Max6966 
        _CMD_AIN1 = 0x01, //p1
        _CMD_AIN2 = 0x00, //p0
        _CMD_PWMA = 0x09, //p9
        _CMD_BIN1 = 0x03, //p3
        _CMD_BIN2 = 0x04, //p4
        _CMD_PWMB = 0x05, //p5
        _CMD_STBY = 0x02, //p2 low puts TB6612FNG in Standby
        _CMD_CONFIG = 0x10, // control register for MAX6966 config
        _CMD_NOOP = 0x20 //no-op control register used to pump the Data thru
    };

    // setup variables
    uint8_t _CS; //chip select
    uint8_t _card;  //this instances card number
    uint8_t _total;  //total cards
    uint8_t _checkCMD;  //used for return data checks
    uint8_t _checkDATA;  //used for return data checks
    uint32_t _prevStep; //store the previous 

    //function to send the SPI data and check return from card
    boolean sendToCard(uint8_t, uint8_t);
    //function to test how many cards there are. 
    boolean checkTotalCards(void);

};




/****************************************************************************** 
  This is a library for Hootie81's Dual DC Motor Shield for Spark Core

 The Dual DC Motor Shield uses a Maxim MAX6966 SPI PWM I/O expander 
 to drive a Toshiba TB6612FNG Dual DC motor Driver. The MAX6966 has a SPI
 Interface and allows shields to be stacked in a daisy chain configuration
 minimizing the required I/O. The MAX6966 also has a built in 32kHz PWM 
 controller saving the Spark more precious I/O.
 The TB6612FNG is a dual H-Bridge motor controller allowing forward and reverse
 directions and PWM speed control. There is short circuit brake function and 
 free wheel functions also. The TB6612FNG will drive 2 DC motors at 
 up to 1.2A continuous and 3.2A peak.
 
 The Card can be configured with solder jumpers, there are 4 options for CS on 
 the underside of the card - A0, A1, D0, D1
 
 on the top side you can choose if the card it the first card and or the last 
 card. This sets up the MISO And MOSI lines to pass from card to card.
 For single card choose Y for both First and Last.
 For multiple cards, 
    on the bottom most card - choose Y for First, and N for Last
    middle cards will be N for both First and Last
    on the top most card choose N for First and Y for last.
  
 Card Number will be 1 for the bottom most card, the upper limit it yet to 
 be determined, but will probably be limited by RAM on the core or Lag in motor
 control from the need to clock data through multiple cards.
 
********************************************************************************

Copyright 2014  Chris Huitema.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

*****************************************************************************/

#include "application.h"


enum direction_t {
    STOP, CW, CCW, BRAKE
}; //these are the control options

class SPI_Motor {
public:

    /***************************************************************************
     Constructors:
       Takes CS line for the card
       Sets the card number and total cards if more than one card
       used in daisy-chain mode. 
     **************************************************************************/

    SPI_Motor(uint8_t); // sets CS and assumes single card 
    SPI_Motor(uint8_t, uint8_t, uint8_t); // CS, Card number, total cards

    /***************************************************************************
     Begin:
       Sets up the CS Port as output and sets high
       Sets up the SPI
       Checks the total number of cards is correct, returns false if not
     **************************************************************************/
    
    boolean begin(void);

    /***************************************************************************
     Motor functions:
        3 different options
            Control word and PWM
            Control word only
            PWM only (faster speed changes as only PWM register changed)
        Control words are:
            STOP    this sets the motor in free wheel mode
            BRAKE   this short circuits the motor pins causing a braking effect
            CW      makes the motor turn in a CW direction
            CCW     makes the motor turn in a CCW direction
       
       All functions will return true is SPI return data looks correct
     **************************************************************************/

    boolean A(direction_t, uint8_t);
    boolean A(direction_t);
    boolean A(uint8_t);
    boolean B(direction_t, uint8_t);
    boolean B(direction_t);
    boolean B(uint8_t);

    /***************************************************************************
     Power save options:
       standby() sets the motor controller to low power mode.
       resume()  puts back to normal operation.
      All functions will return true is SPI return data looks correct
     **************************************************************************/

    boolean standby(void); // puts the motor controller and IO chip to low power
    boolean resume(void); // puts both back to normal operation.


private:

    enum CMD {
        //These are the control registers for the ports on the Max6966 
        _CMD_AIN1 = 0x01, //p1
        _CMD_AIN2 = 0x00, //p0
        _CMD_PWMA = 0x09, //p9
        _CMD_BIN1 = 0x03, //p3
        _CMD_BIN2 = 0x04, //p4
        _CMD_PWMB = 0x05, //p5
        _CMD_STBY = 0x02, //p2 low puts TB6612FNG in Standby
        _CMD_CONFIG = 0x10, // control register for MAX6966 config
        _CMD_NOOP = 0x20 //no-op control register used to pump the Data thru
    };

    // setup variables
    uint8_t _CS; //chip select
    uint8_t _card;  //this instances card number
    uint8_t _total;  //total cards
    uint8_t _checkCMD;  //used for return data checks
    uint8_t _checkDATA;  //used for return data checks

    //function to send the SPI data and check return from card
    boolean sendToCard(uint8_t, uint8_t);
    //function to test how many cards there are. 
    boolean checkTotalCards(uint8_t);

};