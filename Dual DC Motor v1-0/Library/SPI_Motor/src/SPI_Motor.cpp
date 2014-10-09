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
 
 *** All motor cards must use the same CS line, as the MAX6966 SPI output 
 doesn't go high impedance ***
  
  Written by Chris Huitema. 30/7/14 
  
 *****************************************************************************/

#include "SPI_Motor.h"

SPI_Motor::SPI_Motor(uint8_t pinCS) {
    _CS = pinCS;
    _card = 1;
    _total = 1;
    _prevStep = 0;
};

SPI_Motor::SPI_Motor(uint8_t pinCS, uint8_t cardNumber, uint8_t cardTotal) {
    _CS = pinCS;
    _card = cardNumber;
    _total = cardTotal;
    _prevStep = 0;
}

boolean SPI_Motor::begin(void) {
    pinMode(_CS, OUTPUT);
    digitalWrite(_CS, HIGH);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV8);
    SPI.setDataMode(SPI_MODE3);
    SPI.begin();
    if (checkTotalCards())return true;
    return false;

}
// remember STOP, CW, CCW, BRAKE are the options for direction_t Type

boolean SPI_Motor::Stepper(uint32_t step, uint8_t PWM) {
    if (Stepper(step) && Stepper(PWM)) return true;
    return false;
}

boolean SPI_Motor::Stepper(uint8_t PWM) {
    uint8_t _PWM = map(PWM, 0, 255, 254, 3); //map it so its actually PWM output (MAX6966 needs 3-254 to output PWM)
    if (sendToCard(_CMD_PWMA, _PWM) && sendToCard(_CMD_PWMB, _PWM)) return true;
    return false;
}

boolean SPI_Motor::Stepper(uint32_t step) {
    uint8_t stepNum;
    stepNum = step % 4; //get the step number
    if (step == _prevStep + 1) {
        _prevStep = step;
        if (stepNum == 0) {
            if (sendToCard(_CMD_BIN1, 0x00) && sendToCard(_CMD_BIN2, 0x01)) return true;
        } else if (stepNum == 1) {
            if (sendToCard(_CMD_AIN1, 0x00) && sendToCard(_CMD_AIN2, 0x01)) return true;
        } else if (stepNum == 2) {
            if (sendToCard(_CMD_BIN2, 0x00) && sendToCard(_CMD_BIN1, 0x01)) return true;
        } else if (stepNum == 3) {
            if (sendToCard(_CMD_AIN2, 0x00) && sendToCard(_CMD_AIN1, 0x01)) return true;
        }
        return false;
    } else if (step == _prevStep - 1) {
        _prevStep = step;
        if (stepNum == 0) {
            if (sendToCard(_CMD_AIN2, 0x00) && sendToCard(_CMD_AIN1, 0x01)) return true;
        } else if (stepNum == 1) {
            if (sendToCard(_CMD_BIN1, 0x00) && sendToCard(_CMD_BIN2, 0x01)) return true;
        } else if (stepNum == 2) {
            if (sendToCard(_CMD_AIN1, 0x00) && sendToCard(_CMD_AIN2, 0x01)) return true;
        } else if (stepNum == 3) {
            if (sendToCard(_CMD_BIN2, 0x00) && sendToCard(_CMD_BIN1, 0x01)) return true;
        }
        return false;
    } else {
        _prevStep = step;
        uint8_t _AIN1, _AIN2;
        uint8_t _BIN1, _BIN2;
        if (stepNum == 0) {
            _AIN1 = 0x01;
            _AIN2 = 0x00;
            _BIN1 = 0x00;
            _BIN2 = 0x01;
        } else if (stepNum == 1) {
            _AIN1 = 0x00;
            _AIN2 = 0x01;
            _BIN1 = 0x00;
            _BIN2 = 0x01;
        } else if (stepNum == 2) {
            _AIN1 = 0x00;
            _AIN2 = 0x01;
            _BIN1 = 0x01;
            _BIN2 = 0x00;
        } else if (stepNum == 3) {
            _AIN1 = 0x01;
            _AIN2 = 0x00;
            _BIN1 = 0x01;
            _BIN2 = 0x00;
        }
        if ((sendToCard(_CMD_AIN1, _AIN1)) && (sendToCard(_CMD_AIN2, _AIN2)) &&
                (sendToCard(_CMD_BIN1, _BIN1)) && (sendToCard(_CMD_BIN2, _BIN2))) {
            return true; //not quite right but run with it. it will probably be first run
        }
    }

    return false;

}

boolean SPI_Motor::A(direction_t Direction, uint8_t PWM) {
    if (A(Direction) && A(PWM)) return true;
    return false;

}

boolean SPI_Motor::A(direction_t Direction) { //control option, assumes previous PWM
    uint8_t _AIN1, _AIN2;

    if (Direction == STOP) {
        //set AIN1 low, AIN2
        _AIN1 = 0x00;
        _AIN2 = 0x00;
    } else if (Direction == BRAKE) {
        //set AIN1 High, AIN2
        _AIN1 = 0x01;
        _AIN2 = 0x01;
    } else if (Direction == CW) {
        //set AIN1 High, AIN2 Low
        _AIN1 = 0x01;
        _AIN2 = 0x00;
    } else if (Direction == CCW) {
        //set AIN1 Low, AIN2 High
        _AIN1 = 0x00;
        _AIN2 = 0x01;
    }

    if ((sendToCard(_CMD_AIN1, _AIN1)) && (sendToCard(_CMD_AIN2, _AIN2))) return true;

    return false;
};

boolean SPI_Motor::A(uint8_t PWM) {
    uint8_t _PWMA = map(PWM, 0, 255, 254, 3); //map it so its actually PWM output (MAX6966 needs 3-254 to output PWM)

    if (sendToCard(_CMD_PWMA, _PWMA)) return true;

    return false;
};

boolean SPI_Motor::B(direction_t Direction, uint8_t PWM) {
    if (B(Direction) && B(PWM)) return true;
    return false;

}

boolean SPI_Motor::B(direction_t Direction) { //control option, assumes previous PWM
    uint8_t _BIN1, _BIN2;
    if (Direction == STOP) {
        //set BIN1 low, BIN2 low
        _BIN1 = 0x00;
        _BIN2 = 0x00;
    } else if (Direction == BRAKE) {
        //set BIN1 High, BIN2 high
        _BIN1 = 0x01;
        _BIN2 = 0x01;
    } else if (Direction == CW) {
        //set BIN1 High, BIN2 Low
        _BIN1 = 0x01;
        _BIN2 = 0x00;
    } else if (Direction == CCW) {
        //set BIN1 Low, BIN2 High
        _BIN1 = 0x00;
        _BIN2 = 0x01;
    }

    if ((sendToCard(_CMD_BIN1, _BIN1)) && (sendToCard(_CMD_BIN2, _BIN2))) return true;

    return false;
};

boolean SPI_Motor::B(uint8_t PWM) {
    uint8_t _PWMB = map(PWM, 0, 255, 254, 3); //map it so its actually PWM output (MAX6966 needs 3-254 to output PWM)

    if (sendToCard(_CMD_PWMB, _PWMB)) return true;

    return false;
};

boolean SPI_Motor::standby(void) {// puts both back to low power operation.
    uint8_t _STBY = 0x00;

    if ((sendToCard(_CMD_STBY, _STBY)) && (sendToCard(_CMD_CONFIG, _STBY))) return true;

    return false;
};

boolean SPI_Motor::resume(void) {// puts both back to normal operation.
    uint8_t _STBY = 0x01;

    if ((sendToCard(_CMD_STBY, _STBY)) && (sendToCard(_CMD_CONFIG, _STBY))) return true;

    return false;
};

boolean SPI_Motor::checkTotalCards() {
    // If we don't take CS high during this, the data will be pumped all the way back to us and wont effect the motors at all
    int j;
    uint8_t CMD = 0x55; // unique dummy number
    uint8_t DATA = 0xF0; // unique dummy number
    uint8_t _NOOP = 0x00;
    PIN_MAP[_CS].gpio_peripheral->BRR = PIN_MAP[_CS].gpio_pin; //set CS low
    _checkCMD = SPI.transfer(CMD); //send control byte
    _checkDATA = SPI.transfer(DATA); //send data byte
    for (j = 1; j < 10; j++) { //send no-op + blank data to clear data past last card (not required for single card)
        _checkCMD = SPI.transfer(_CMD_NOOP);
        _checkDATA = SPI.transfer(_NOOP);
        if ((_checkCMD == CMD) && (_checkDATA == DATA)) {
            PIN_MAP[_CS].gpio_peripheral->BSRR = PIN_MAP[_CS].gpio_pin; //set CS high
            if (_total == j) {
                return true; //match to number of cards
            } else return false; //got reply but different number
        }
    }
    PIN_MAP[_CS].gpio_peripheral->BSRR = PIN_MAP[_CS].gpio_pin; //set CS high
    return false; //didnt get return data

}

boolean SPI_Motor::sendToCard(uint8_t CMD, uint8_t DATA) {
    int j;
    uint8_t _NOOP = 0x00;
    PIN_MAP[_CS].gpio_peripheral->BRR = PIN_MAP[_CS].gpio_pin; //set CS low
    _checkCMD = SPI.transfer(CMD); //send control byte
    _checkDATA = SPI.transfer(DATA); //send data byte
    for (j = 1; j < _card; j++) { //send no-op + blank data to put in correct card (not required for single card)
        SPI.transfer(_CMD_NOOP);
        SPI.transfer(_NOOP);
    }
    PIN_MAP[_CS].gpio_peripheral->BSRR = PIN_MAP[_CS].gpio_pin; //set CS High so the data gets latched in the card
    PIN_MAP[_CS].gpio_peripheral->BRR = PIN_MAP[_CS].gpio_pin; //set CS low, now we clear out the data so it doesn't effect other cards.
    for (j = _card; j <= _total; j++) { //send no-op + blank data to clear data past last card (not required for single card)
        _checkCMD = SPI.transfer(_CMD_NOOP);
        _checkDATA = SPI.transfer(_NOOP);
    }
    PIN_MAP[_CS].gpio_peripheral->BSRR = PIN_MAP[_CS].gpio_pin; //set CS high
    if ((_checkCMD == CMD) && (_checkDATA == DATA)) {
        return true; //data returned and matched to number of cards
    }
    return false;

}