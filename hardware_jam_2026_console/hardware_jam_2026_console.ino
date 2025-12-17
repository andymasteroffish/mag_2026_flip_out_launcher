// ///////////////////////////////////////////////////////////////////////////////
// // MAG Alt Control Jam 2026
// // Console code
// // Andy Wallace
// ///////////////////////////////////////////////////////////////////////////////

#include <Keyboard.h>

#define NUM_INPUTS 9

// "debouncing" is when we add a threshold to signal changes to mitigate the natural fluxuations in electricity that often happen with small boards
// we make sure the signal stays solid for a small amount instead of changing the key on the very first pulse
#define DEBOUNCE_TIME 50  //measured in millis

// info for each input
// toggle switches and buttons both complete a circuit
// so this structure works for both!
struct ButtonInfo{
  int key;   //the keyboard key that will be presed / released
  int pin;    //the input pin to connect to

  int prev_live_state;      //state last time we checked
  int prev_stable_state;    //state last time we declared it stable

  //when this is more than DEBOUNCE_TIME millis ago, we declare the state stable
  unsigned long last_unstable_time; 
};

//tracking our inputs
ButtonInfo inputs[NUM_INPUTS];

//pausing input
bool disable_keyboard_input;

//timing out
uint32_t last_input_time;
uint32_t idle_time_for_reset_key = 10 * 60 * 1000;    //measured in millis
//todo: might be nice to have a toggle switch to kill this

void setup() {
  //Serial.begin(9600);     //remove when not testing

  //which pins are we using
  inputs[0].pin = 0;
  inputs[1].pin = 1;
  inputs[2].pin = 2;
  inputs[3].pin = 3;
  inputs[4].pin = 4;
  //big button
  inputs[5].pin = 6;
  //reset panel
  inputs[6].pin = 8;
  inputs[7].pin = 9;
  inputs[8].pin = 10;

  //which keys are we using
  inputs[0].key = 'q';
  inputs[1].key = 'w';
  inputs[2].key = 'e';
  inputs[3].key = 'r';
  inputs[4].key = 't';
  inputs[5].key = ' ';  //the start button

  inputs[6].key = KEY_LEFT;
  inputs[7].key = KEY_RIGHT;
  inputs[8].key = 'k';

  //setup inputs
  for(int i=0; i<NUM_INPUTS; i++){
    inputs[i].prev_live_state = LOW;
    inputs[i].prev_live_state = inputs[i].prev_live_state;
    inputs[i].last_unstable_time = millis();
    pinMode(inputs[i].pin, INPUT_PULLUP);
  }

  disable_keyboard_input = false;
  last_input_time = millis();

}

void loop() {
  //check on our circuits
  for(int i=0; i<NUM_INPUTS; i++){
    //read the pin
    int cur_state = digitalRead( inputs[i]. pin );

    //if this is different from last check, we're unstable
    if (cur_state != inputs[i].prev_live_state){
      //reset the timer
      inputs[i].last_unstable_time = millis();
      last_input_time = millis();
    }

    //if we've been stable for a while, check if the value has changed
    if( millis()-inputs[i].last_unstable_time > DEBOUNCE_TIME ){

      //check if this is a falling edge
      if (cur_state == LOW && inputs[i].prev_stable_state == HIGH){
        if(!disable_keyboard_input){
          Keyboard.press(inputs[i].key);
        }
      }
      //or a rising edge
      if (cur_state == HIGH && inputs[i].prev_stable_state == LOW){
        if(!disable_keyboard_input){
          Keyboard.release(inputs[i].key);
        }
      }

      //then store it
      inputs[i].prev_stable_state = cur_state;
    }

    //always store our live state
    inputs[i].prev_live_state = cur_state;
  }

  //check how long we've been running with no input
  //Serial.println( millis() - last_input_time );
  if (millis() - last_input_time > idle_time_for_reset_key){
    last_input_time = millis();
    Keyboard.press(KEY_K);
    delay(10);
    Keyboard.release(KEY_K);
  }

  //tiny delay to prevent this fomr running too fast
  delay(5);

}

