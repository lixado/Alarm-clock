#include "functions.hpp"
#include <Arduino.h>
#include "pitches.hpp"
#include <LiquidCrystal.h>
#include <EEPROM.h>

//var used for alarm
uint8_t alarmon = 0;
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
bool turn_sound_off = false;

void set_turn_sound_off()
{
    turn_sound_off = true;
}


//set music with a frequency
int setPiezo()
{
    //Some code is used from this website: https://www.arduino.cc/en/Tutorial/toneMelody

    //The melody that will be played, see "pitches.hpp" for the notes. "Pitches.hpp" sets up note-variables as frequencies that will be played.
    int melody[] = {
        NOTE_FS4, 0, NOTE_CS5, NOTE_AS4, NOTE_AS4, 0, NOTE_GS4, NOTE_FS4,
        NOTE_FS4, NOTE_B4, NOTE_AS4, NOTE_AS4, NOTE_GS4, NOTE_GS4,
        NOTE_FS4, 0, NOTE_FS4, NOTE_CS5, NOTE_AS4, NOTE_AS4,
        NOTE_GS4, NOTE_GS4, NOTE_FS4, NOTE_FS4, NOTE_DS4, NOTE_CS4, NOTE_DS4, -1};
    //How long the tone should play for: 2 is half note, 4 is quarter note, etc.
    int durations[] = {4, 4, 4, 4, 6, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 2, 4, -1};

    int currentNote = 0;
    while (durations[currentNote] != -1)
    {
        if (turn_sound_off) 
        {
            return NULL;
        }

        int duration = 1000 / durations[currentNote];
        tone(13, melody[currentNote], duration);
        int pause = duration * 1.30;
        delay(pause);
        noTone(13);
        currentNote++;

        if (turn_sound_off) 
        {
            return NULL;
        }

    }
}

void startLCD()
{
    char time[8] = "Time :";
    char alarm[8] = "Alarm:";

    lcd.clear();
    lcd.begin(16,2);
    lcd.setCursor(0, 0);
    lcd.write(time);
    lcd.setCursor(0, 1);
    lcd.write(alarm);
    lcd.setCursor(9,0);
    lcd.write(":");
    lcd.setCursor(12,0);
    lcd.write(":");
    lcd.setCursor(9,1);
    lcd.write(":");
    lcd.setCursor(12,1);
    lcd.write(":");
}

void updateClockHours(uint8_t hours)
{
    char hoursStr[3] = { 0 };
    if (hours <= 9) 
    {
        snprintf(hoursStr, sizeof(hoursStr), "0%i", hours);        
    }
    else
    {
        snprintf(hoursStr, sizeof(hoursStr), "%i", hours);           
    }

    lcd.setCursor(7, 0);
    lcd.write(hoursStr);
}

void updateClockMinutes(uint8_t minutes)
{
    char minutesStr[3] = { 0 };
    if (minutes <= 9) 
    {
        snprintf(minutesStr, sizeof(minutesStr), "0%i", minutes);        
    }
    else
    {
        snprintf(minutesStr, sizeof(minutesStr), "%i", minutes);           
    }
    
    
    lcd.setCursor(10, 0);
    lcd.write(minutesStr);
}

void updateClockSeconds(uint8_t seconds)
{
    char secondsStr[3] = { 0 };
    if (seconds <= 9) 
    {
        snprintf(secondsStr, sizeof(secondsStr), "0%i", seconds);        
    }
    else
    {
        snprintf(secondsStr, sizeof(secondsStr), "%i", seconds);           
    }
    
    
    lcd.setCursor(13, 0);
    lcd.write(secondsStr);
}

void updateAlarmHours(uint8_t hours)
{
    char hoursStr[3] = { 0 };
    if (hours <= 9) 
    {
        snprintf(hoursStr, sizeof(hoursStr), "0%i", hours);        
    }
    else
    {
        snprintf(hoursStr, sizeof(hoursStr), "%i", hours);           
    }
    
    
    lcd.setCursor(7, 1);
    lcd.write(hoursStr);
}

void updateAlarmMinutes(uint8_t minutes)
{
    char minutesStr[3] = { 0 };
    if (minutes <= 9) 
    {
        snprintf(minutesStr, sizeof(minutesStr), "0%i", minutes);        
    }
    else
    {
        snprintf(minutesStr, sizeof(minutesStr), "%i", minutes);           
    }
    
    
    lcd.setCursor(10, 1);
    lcd.write(minutesStr);
}

void updateAlarmSeconds(uint8_t seconds)
{
    char secondsStr[3] = { 0 };
    if (seconds <= 9) 
    {
        snprintf(secondsStr, sizeof(secondsStr), "0%i", seconds);        
    }
    else
    {
        snprintf(secondsStr, sizeof(secondsStr), "%i", seconds);           
    }
    
    
    lcd.setCursor(13, 1);
    lcd.write(secondsStr);
}

void alarm_on_or_off(bool alarm)
{
    if (alarm) 
    {
        lcd.setCursor(15, 1);
        lcd.write("*");
    }
    else
    {
        lcd.setCursor(15, 1);
        lcd.write(" ");
    }
    
    
}

//fucntion to strings to send to screen
void updateClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t Alarmhours, uint8_t Alarmminutes, uint8_t Alarmseconds)
{
    updateClockHours(hours);
    updateClockMinutes(minutes);
    updateClockSeconds(seconds);
    updateAlarmHours(Alarmhours);
    updateAlarmMinutes(Alarmminutes);
    updateAlarmSeconds(Alarmseconds);
}

void setUpTimer()
{

    noInterrupts();
    //useless?
    //analog input
    EICRA |= (1 << ISC01);
    EICRA &= ~(1 << ISC00);
    EIMSK |= (1 << INT0); // Enable external interrupt 0 (pin 2)

    //manual delay
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    TCCR1B |= (1 << CS12);
    TCCR1B |= (1 << WGM12);
    OCR1A = 16000000 / 256 / 1; // / by antal ganger per sekund som vi vil få en interrupt
    TIMSK1 |= (1 << OCIE1A);
    interrupts();
}

void setUpButtons()
{
    noInterrupts();

    EICRA = 0;

    EICRA |= (1 << ISC00);
    EICRA |= (1 << ISC01); // Configure interrupt to happen on falling edge
    
    EICRA |= (1 << ISC10); // Configure interrupt to happen on falling edge
    EICRA |= (1 << ISC11);

    EIMSK |= (1 << INT0); // Enable external interrupt 2, 3
    EIMSK |= (1 << INT1);

    interrupts();
}

//return true = silence, false = delay by 5 mins
int Alarm(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t Alarmhours, uint8_t Alarmminutes, uint8_t Alarmseconds, bool button1pressed, bool button2pressed)
{
    if (hours == Alarmhours && minutes == Alarmminutes && seconds == Alarmseconds)
    {
        alarmon = 1;
        turn_sound_off = false;
    }

    if (alarmon == 1)
    {

        //play sound
        setPiezo();
        delay(1000);

        //choose
        if (button1pressed)
        {
            alarmon = 0;
            return 1;
        }
        else if (button2pressed)
        {
            alarmon = 0;
            Serial.println("button 2 ");
            return 0;
        }
    }
    return 3;
}

//function to write to non-volatile mem
void writetoEEPROM(uint8_t Alarmhours, uint8_t Alarmminutes)
{
    //Save Alarmhours
    EEPROM.update(1, Alarmhours);

    //Save Alarmminutes
    EEPROM.update(0, Alarmminutes);
}

//function that reads from non-volatile mem and returns ints in a struct
Alarm_from_mem_t readfromEEPROM()
{
    Alarm_from_mem_t ints_to_return;

    ints_to_return.Alarmhours = EEPROM.read(1);

    ints_to_return.Alarmminutes = EEPROM.read(0);

    return ints_to_return;
}



