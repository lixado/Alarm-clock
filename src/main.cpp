#include <Arduino.h>
#include "functions.hpp"
#include "string.h"

//Variable names for the switches and leds
const uint8_t led1Pin = 4;
const uint8_t led2Pin = 5;
const uint8_t switch1Pin = 2;
const uint8_t switch2Pin = 3;

//Clocktime
uint8_t hours = 12;
uint8_t minutes = 0;
uint8_t seconds = 0;

//Alarm time
uint8_t Alarmhours = 12;
uint8_t Alarmminutes = 0;
uint8_t Alarmseconds = 0;

//mode
uint8_t timemode = 0;
uint8_t alarmmode = 0;

int buttonState = 0;      // current state of the button
int buttonState2 = 0;     // current state of the button 2
int lastButtonState = 0;  // previous state of the button
int lastButtonState2 = 0; // previous state of the button 2
int startPressed = 0;     // the time button was pressed
int endPressed = 0;       // the time button was released
int timeHold = 0;         // the time button is hold
int timeReleased = 0;     // the time button is released
int choice = 3;           // int used to react to alarm 1 = silence, 0 = delay, 3 = nothing

//maybe use buttonstate insted of buttonxpressed?

bool button1pressed = false; //state of button, used for alarm
bool button2pressed = false; //state of button, used for alarm

//alarm on or off
bool alarm = true;

//turn alarm on or off
uint8_t alarm_on_off = 0; //must be 5 to turn it on
uint8_t seconds_at_press;

void setup()
{
  Serial.begin(9600);

  setUpTimer();
  setUpButtons();

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);

  pinMode(2, INPUT);
  pinMode(3, INPUT);

  startLCD();
  alarm_on_or_off(alarm);

  //get Alarm from mem and set it
  Alarm_from_mem_t ints_to_returned = readfromEEPROM();

  Alarmhours = ints_to_returned.Alarmhours;
  Alarmminutes = ints_to_returned.Alarmminutes;
}

void loop()
{

  //choice to react to alarm
  if (alarm)
  {

    choice = Alarm(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds, button1pressed, button2pressed);

    if (choice == 1)
    {

      //testing purposes print function
      Serial.print("silence");

      digitalWrite(led2Pin, LOW);
      //need to re-read so that the Alarm doesnt get stuck with + 5 * x minutes
      //get Alarm from mem and set it
      Alarm_from_mem_t ints_to_returned = readfromEEPROM();

      Alarmhours = ints_to_returned.Alarmhours;
      Alarmminutes = ints_to_returned.Alarmminutes;

      //reset choice variable
      choice = 3;

      //reset state of buttons
      button1pressed = false;
      button2pressed = false;
    }
    else if (choice == 0)
    {
      digitalWrite(led2Pin, HIGH);
      //testing purposes print function
      Serial.print("delay by 5 mins");

      //might have problems with saving to non-volatile
      //increases alarm minutes by 5
      Alarmminutes += 5;

      //reset choice variable
      choice = 3;

      //reset state of buttons
      button1pressed = false;
      button2pressed = false;
    }
  }

  //read the state of buttons
  buttonState = digitalRead(switch1Pin);
  buttonState2 = digitalRead(switch2Pin);

  // button state changed
  if (buttonState != lastButtonState)
  {

    // the button was just pressed
    if (buttonState == HIGH)
    {
      startPressed = millis();
      timeReleased = startPressed - endPressed; //how much time button was pressed
    }
    else // the button was just released
    {
      endPressed = millis();
      timeHold = endPressed - startPressed;

      if (timeHold >= 2000)
      {

        if (timemode == 1)
        {
          //reset state of buttons
          button1pressed = false;
          button2pressed = false;
          timemode = 0; //leave clock mode
          hours--;
        }
        else if (!alarmmode && timemode == 0)
        {
          timemode = 1; //enter clock mode
        }
      }
    }
  }

  //last state of button 1 used to check how long button pressed
  lastButtonState = buttonState;

  //Alarm
  // button state changed
  if (buttonState2 != lastButtonState2)
  {
    // the button was just pressed
    if (buttonState2 == HIGH)
    {
      startPressed = millis();
      timeReleased = startPressed - endPressed;
    }
    else // the button was just released
    {
      endPressed = millis();
      timeHold = endPressed - startPressed; //how much time button was pressed

      if (timeHold >= 2000) //runs if button pressed nore then 4 seconds
      {

        if (alarmmode == 1)
        {        
          //reset state of buttons
          button1pressed = false;
          button2pressed = false;

          alarmmode = 0; //leave alarm mode
          Alarmminutes--;

          //write to mem
          writetoEEPROM(Alarmhours, Alarmminutes);

          digitalWrite(led1Pin, HIGH);
        }
        else if (alarmmode == 0 && !timemode)
        {
          alarmmode = 1; // enter alarm mode
          digitalWrite(led1Pin, LOW);
        }
      }
    }
  }

  //last state of button 2, used to check for time pressed
  lastButtonState2 = buttonState2;
}

//if timer ticks
ISR(TIMER1_COMPA_vect)
{
  //increase seconds
  seconds++;

  if (seconds >= 60) //reset seconds if equals 60 and increase minutes
  {
    seconds = 0;
    minutes++;
  }

  if (minutes >= 60) //reset minutes if equals 60 and increase hours
  {
    minutes = 0;
    hours++;
  }

  if (hours >= 24) //reset hours if equals 24
  {
    hours = 0;
  }

  //used to control alarm on or off
  if (seconds % 2 == 0 && alarmmode == 0 && timemode == 0 && alarm_on_off > 0)
  {
    alarm_on_off--;
  }

  //update the clock
  updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
}

ISR(INT0_vect) // button 1
{
  set_turn_sound_off();
  //if button 1 pressed, used for alarm
  button1pressed = true;

  //light and print function for testing purposes
  Serial.println("Button 1");

  if (timemode == 1) //if in clock mode and control clock
  {

    hours++;
    if (hours >= 24) //Reset hours and seconds
    {
      hours = 0;
      seconds = 0;
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }

    else //Reset seconds
    {
      seconds = 0;
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
  }

  //alarm
  if (alarmmode == 1) //if in alarm mode
  {

    Alarmhours++;
    if (Alarmhours >= 24) //Reset Alarm hours and Alarm seconds
    {
      Alarmhours = 0;
      Alarmseconds = 0; //Alarm seconds resets each time the button is clicked
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }

    else //Resets Alarm seconds
    {
      Alarmseconds = 0; //Alarm seconds resets each time the button is clicked
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
  }
}

ISR(INT1_vect) // button 2
{
  set_turn_sound_off();
  //if button 2 pressed, used for Alarm
  button2pressed = true;

  if (!alarmmode && !timemode)
  {
    alarm_on_off++;
  }

  if (alarm_on_off >= 5 && !timemode && !alarmmode)
  {

    if (alarm)
    {
      alarm = false;
      alarm_on_off = 0;
    }
    else
    {
      alarm = true;
      alarm_on_off = 0;
    }

    alarm_on_or_off(alarm);
  }

  //light and print function for testing purposes
  Serial.println("Button 2");

  if (timemode == 1) // if clock mode on control clock
  {
    minutes++;         //increase minutes by 1
    if (minutes >= 60) //reset minutes if bigger then 60
    {
      minutes = 0;
      seconds = 0; //reset seconds
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
    else
    {
      seconds = 0; //reset seconds
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
  }

  //Alarm

  if (alarmmode == 1) //if in alarm mode
  {
    Alarmminutes++;         //increase alarm minutes by 1
    if (Alarmminutes >= 60) //reset Alarm minutes if bigger then 60
    {
      Alarmminutes = 0;
      Alarmseconds = 0; //reset seconds
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
    else
    {
      Alarmseconds = 0; //reset seconds
      updateClock(hours, minutes, seconds, Alarmhours, Alarmminutes, Alarmseconds);
    }
  }
}