#include <LiquidCrystal_I2C.h>
#include <MIDI.h>
#include <ShiftRegister74HC595.h>
#include "Const.h"

ShiftRegister74HC595<2> shiftOutput_1(6,8,7); // 74HC595 for pistons
ShiftRegister74HC595<7> shiftOutput_2(11,9,10); // 74HC595 for stops
LiquidCrystal_I2C lcd(0x27, 20, 4);

MIDI_CREATE_DEFAULT_INSTANCE(); 

void handleNoteOn(byte channel, byte pitch, byte velocity)
{
    if(channel == outputChannel)
    {   
        if(pitch >= 0 && pitch < 11)
          shiftOutput_1.set( pitch, HIGH);
        else     
          shiftOutput_2.set( pitch - firstStopPitch, HIGH );
    }
}

void handleNoteOff(byte channel, byte pitch, byte velocity)
{
    if(channel == outputChannel)
    {
        if(pitch >= 0 && pitch < 11)
          shiftOutput_1.set( pitch, LOW);
        else     
          shiftOutput_2.set( pitch - firstStopPitch, LOW );
    }
}

void readMessageToLCD()
{
    int type = MIDI.getType();
    int data1 = MIDI.getData1();

    if((type == midi::SystemExclusive) && (data1 < 80))
    {
        byte* sysExMessage = MIDI.getSysExArray();

        String line1 = "";
        String line2 = "";

        if((sysExMessage[1] == HAUPTWERK_ID) && (data1 > 24))
        {
            for(int i = 6; i < (22); ++i)
            {
                byte c = sysExMessage[i];
                line1 = line1 + char(c);
            }

            for(int i = 27; i < (37); ++i)
            {
                byte c = sysExMessage[i];
                line2 = line2 + char(c);
            }

            LCDUpdate(line1, line2);
        }
    }
}

void LCDUpdate(String line1, String line2)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(line1);
    lcd.setCursor(0, 1);
    lcd.print(line2);
}

void setup()
{
    MIDI.begin(MIDI_CHANNEL_OMNI);
    MIDI.turnThruOff();
    MIDI.setHandleNoteOn(handleNoteOn);
    MIDI.setHandleNoteOff(handleNoteOff);

    shiftOutput_1.setAllLow();
    shiftOutput_2.setAllLow();   

    lcd.begin();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Initialize...");
}

void loop() 
{     
    while(MIDI.read())
    {
        readMessageToLCD();
    }
}
