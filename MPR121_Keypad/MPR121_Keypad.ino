#include <Wire.h>
#include "mpr121.h"

#define MPR121_ADDR    0x5A

#define PHONE_DIGITS   10  // 10 digits in a phone number

// Match key inputs with electrode numbers
#define STAR    0
#define SEVEN   1
#define FOUR    2
#define ONE     3
#define ZERO    4
#define EIGHT   5
#define FIVE    6
#define TWO     7
#define POUND   8
#define NINE    9
#define SIX     10
#define THREE   11

int irqpin = 4;

byte x = 0;
uint16_t touchstatus;
char phoneNumber[PHONE_DIGITS];


void setup(){
    pinMode(irqpin, INPUT);
    digitalWrite(irqpin, HIGH);

    Wire.begin();        // join i2c bus (address optional for master)
    Serial.begin(9600);  // start serial for output

    mpr121QuickConfig();
}

void loop(){
   getPhoneNumber();
  
    Serial.print("\nDialing... \n");
    for (int i=0; i<PHONE_DIGITS; i++)
        Serial.print(phoneNumber[i]);
}

void getPhoneNumber()
{
    int i = 0;
    int touchNumber;
    
    Serial.println("Please Enter a number...");
    
    while(i<PHONE_DIGITS){
        while(checkInterrupt())
            ;
        touchNumber = 0;
        
        touchstatus = mpr121Read(0x01) << 8;
        touchstatus |= mpr121Read(0x00);
        
        for (int j=0; j<12; j++)  // Check how many electrodes were pressed
        {
            if ((touchstatus & (1<<j)))
                touchNumber++;
        }
        
        if (touchNumber == 1)
        {
            if (touchstatus & (1<<STAR))
                phoneNumber[i] = '*';
            else if (touchstatus & (1<<SEVEN))
                phoneNumber[i] = '7';
            else if (touchstatus & (1<<FOUR))
                phoneNumber[i] = '4';
            else if (touchstatus & (1<<ONE))
                phoneNumber[i] = '1';
            else if (touchstatus & (1<<ZERO))
                phoneNumber[i] = '0';
            else if (touchstatus & (1<<EIGHT))
                phoneNumber[i] = '8';
            else if (touchstatus & (1<<FIVE))
                phoneNumber[i] = '5';
            else if (touchstatus & (1<<TWO))
                phoneNumber[i] = '2';
            else if (touchstatus & (1<<POUND))
                phoneNumber[i] = '#';
            else if (touchstatus & (1<<NINE))
                phoneNumber[i] = '9';
            else if (touchstatus & (1<<SIX))
                phoneNumber[i] = '6';
            else if (touchstatus & (1<<THREE))
                phoneNumber[i] = '3';
            
            Serial.print(phoneNumber[i]);
            i++;
        }
        else if (touchNumber == 0)
            ;
        else
            Serial.println("Only touch ONE button!");
    }
}


byte mpr121Read(byte reg){
    byte data;
    Wire.beginTransmission(MPR121_ADDR);
    Wire.write((byte)reg);
    Wire.endTransmission(false);

    Wire.requestFrom(MPR121_ADDR, 1, true);
    while(int i = Wire.available()){
        for(int j=0; j<i; j++)
            data = Wire.read();
    }
    return data;
}

void mpr121Write(byte reg, byte data){
    Wire.beginTransmission(MPR121_ADDR);
    Wire.write((byte)reg);
    Wire.write((byte)data);
    Wire.endTransmission();
}

void mpr121QuickConfig(void){
    // Section A
    // This group controls filtering when data is > baseline.
    mpr121Write(MHD_R, 0x01);
    mpr121Write(NHD_R, 0x01);
    mpr121Write(NCL_R, 0x00);
    mpr121Write(FDL_R, 0x00);
  
    // Section B
    // This group controls filtering when data is < baseline.
    mpr121Write(MHD_F, 0x01);
    mpr121Write(NHD_F, 0x01);
    mpr121Write(NCL_F, 0xFF);
    mpr121Write(FDL_F, 0x02);
    
    // Section C
    // This group sets touch and release thresholds for each electrode
    mpr121Write(ELE0_T, TOU_THRESH);
    mpr121Write(ELE0_R, REL_THRESH);
    mpr121Write(ELE1_T, TOU_THRESH);
    mpr121Write(ELE1_R, REL_THRESH);
    mpr121Write(ELE2_T, TOU_THRESH);
    mpr121Write(ELE2_R, REL_THRESH);
    mpr121Write(ELE3_T, TOU_THRESH);
    mpr121Write(ELE3_R, REL_THRESH);
    mpr121Write(ELE4_T, TOU_THRESH);
    mpr121Write(ELE4_R, REL_THRESH);
    mpr121Write(ELE5_T, TOU_THRESH);
    mpr121Write(ELE5_R, REL_THRESH);
    mpr121Write(ELE6_T, TOU_THRESH);
    mpr121Write(ELE6_R, REL_THRESH);
    mpr121Write(ELE7_T, TOU_THRESH);
    mpr121Write(ELE7_R, REL_THRESH);
    mpr121Write(ELE8_T, TOU_THRESH);
    mpr121Write(ELE8_R, REL_THRESH);
    mpr121Write(ELE9_T, TOU_THRESH);
    mpr121Write(ELE9_R, REL_THRESH);
    mpr121Write(ELE10_T, TOU_THRESH);
    mpr121Write(ELE10_R, REL_THRESH);
    mpr121Write(ELE11_T, TOU_THRESH);
    mpr121Write(ELE11_R, REL_THRESH);
    
    // Section D
    // Set the Filter Configuration
    // Set ESI2
    mpr121Write(FIL_CFG, 0x04);
    
    // Section E
    // Electrode Configuration
    // Enable 6 Electrodes and set to run mode
    // Set ELE_CFG to 0x00 to return to standby mode
    mpr121Write(ELE_CFG, 0x0C);	// Enables all 12 Electrodes
    //mpr121Write(ELE_CFG, 0x06);		// Enable first 6 electrodes
    
    // Section F
    // Enable Auto Config and auto Reconfig
    /*mpr121Write(ATO_CFG0, 0x0B);
    mpr121Write(ATO_CFGU, 0xC9);	// USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   mpr121Write(ATO_CFGL, 0x82);	// LSL = 0.65*USL = 0x82 @3.3V
    mpr121Write(ATO_CFGT, 0xB5);*/	// Target = 0.9*USL = 0xB5 @3.3V
}

byte checkInterrupt(void){
    if(digitalRead(irqpin))
        return 1;
    return 0;
}
