#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <Arduino.h>

typedef struct
{
  uint8_t Alarmhours;
  uint8_t Alarmminutes;

}Alarm_from_mem_t;

void set_turn_sound_off();
int setPiezo(int frequency);
void printToLCD(char string[20], char second_string[20], bool alarm);
void setUpTimer();
void updateClock(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t Alarmhours, uint8_t Alarmminutes, uint8_t Alarmseconds);
void setUpButtons();
int Alarm(uint8_t hours, uint8_t minutes, uint8_t seconds, uint8_t Alarmhours, uint8_t Alarmminutes, uint8_t Alarmseconds, bool button1pressed, bool button2pressed);
void writetoEEPROM(uint8_t Alarmhours, uint8_t Alarmminutes);
void startLCD();
void updateClockHours(uint8_t hours);
void updateClockMinutes(uint8_t minutes);
void updateClockSeconds(uint8_t seconds);
void updateAlarmHours(uint8_t Alarmhours);
void updateAlarmMinutes(uint8_t Alarmminutes);
void updateAlarmSeconds(uint8_t Alarmseconds);
void alarm_on_or_off(bool alarm);
Alarm_from_mem_t readfromEEPROM();


#endif