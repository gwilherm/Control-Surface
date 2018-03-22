#ifndef SELECTOR_H_
#define SELECTOR_H_

#include <string.h>

#include "../Helpers/Linked_List.h"
#include "../ExtendedInputOutput/ExtendedInputOutput.h"

#ifdef __AVR__
#include "../Helpers/initializer_list.h"
#else
#include <initializer_list>
#endif

using namespace ExtIO;

class Selector
{  
public:
  enum buttonType
  {
    TOGGLE,
    MOMENTARY
  };

  /*
  Different  Select modes:

  - One toggle switch (latching switch)

      When the switch is in the 'off' position, Setting 1 is selected
      When the switch is in the 'on' position, Setting 2 is selected

      Selector(, switch pin, Selector::TOGGLE);


  - One toggle switch (latching switch) and one LED

      When the switch is in the 'off' position, Setting 1 is selected and the LED is off
      When the switch is in the 'on' position, Setting 2 is selected and the LED is on

          Note: this mode is pretty useless, you can just connect the LED to the switch directly,
                without wasting a digital output pin on it.

      Selector(, switch pin, led pin, Selector::TOGGLE);


  - One momentary switch (push button)

      Pressing the button switches the Setting:
      When starting the program, Setting 1 is selected,
      When the button is pressed, Setting 2 is selected,
      When the button is pressed again, Setting 1 is selected, 
      and so on.

      Selector(, button pin);
      Selector(, button pin, Selector::MOMENTARY);


  - One momentary switch (push button) and one LED

      Pressing the button switches the Setting and toggles the LED:
      When starting the program, Setting 1 is selected and the LED is off,
      When the button is pressed, Setting 2 is selected and the LED turns on,
      When the button is pressed again, Setting 1 is selected and the LED is turned off,
      and so on.

      Selector(, button pin, led pin);
      Selector(, button pin, led pin, Selector::MOMENTARY);


  - Multiple momentary switches (push buttons)

      Pressing one of the buttons selects the respective output:
      When starting the program, Setting 1 is selected,
      When the second button is pressed, Setting 2 is selected,
      When the n-th button is pressed, Setting n is selected.

      Selector(, { button 1 pin, button 2 pin, ... , button n pin } );


  - Multiple momentary switches (push buttons) and multiple LEDs

      Pressing one of the buttons selects the respective output and enables the respective LED:
      When starting the program, Setting 1 is selected and LED 1 is on,
      When the second button is pressed, Setting 2 is selected, LED 1 turns off and LED 2 turns on,
      When the n-th button is pressed, Setting n is selected, LED n turns on, and all other LEDs are off.

      Selector(, { button 1 pin, button 2 pin, ... , button n pin }, { led 1 pin, led 2 pin, ... , led n pin } );

      
  - Two momentary switches (push buttons)

      Pressing the first button increments the Setting number,
      pressing the second button decrements the Setting number: 
      When starting the program, Setting 1 is selected,
      When the first button is pressed, Setting 2 is selected, 
      When the first button is pressed again, Setting 3 is selected,
      When the last Setting is selected, and the first button is pressed again,
      Setting 1 is selected.
      When the second button is pressed, the last Setting (n) is selected,
      When the second button is pressed again, Setting (n-1) is selected,
      and so on.

      Selector(, { button increment pin, button decrement pin }, number of Settings);


  - Two momentary switches (push buttons) and multiple LEDs

      Pressing the first button increments the Setting number and turns on the respective LED,
      pressing the second button decrements the Setting number and turns on the respective LED: 
      When starting the program, Setting 1 is selected and LED 1 is on,
      When the first button is pressed, Setting 2 is selected, LED 1 turns off and LED 2 turns on,
      When the first button is pressed again, Setting 3 is selected, LED 2 turns off and LED 3 turns on.
      When the last Setting is selected, and the first button is pressed,
      Setting 1 is selected, the last LED turns off and LED 1 turns on.
      When the second button is pressed, the last Setting (n) is selected, LED 1 turns off and LED n turns on,
      When the second button is pressed again, Setting (n-1) is selected, LED n turns off and LED n-1 turns on,
      and so on.

      Selector(, { button increment pin, button decrement pin }, { led 1 pin, led 2 pin, ... , led n pin });


  - One momentary switch (push button)

      Pressing the button increments the Setting number,
      When starting the program, Setting 1 is selected,
      When the button is pressed, Setting 2 is selected, 
      When the button is pressed again, Setting 3 is selected,
      When the last Setting is selected, and the button is pressed again,
      Setting 1 is selected.

      Selector(, { button increment pin }, number of Settings);


  - One momentary switch (push button) and multiple LEDs

      Pressing the button increments the Setting number and turns on the respective LED,
      When starting the program, Setting 1 is selected and LED 1 is on,
      When the button is pressed, Setting 2 is selected, LED 1 turns off and LED 2 turns on,
      When the button is pressed again, Setting 3 is selected, LED 2 turns off and LED 3 turns on.
      When the last Setting is selected, and the button is pressed,
      Setting 1 is selected, the last LED turns off and LED 1 turns on.
      
      Selector(, { button increment pin }, { led 1 pin, led 2 pin, ... , led n pin });


    Note: a switch is 'off' or 'released' when it doesn't conduct. The digital value 
    on the input will therefore be HIGH (because of the pull-up resistor)
  */

  Selector(pin_t switchPin, buttonType buttonType = MOMENTARY) // One switch or button, no LEDs
      : switchPin(switchPin)
  {
    if (buttonType == TOGGLE)
    {
      mode = SINGLE_SWITCH;
    }
    else
    {
      dbButton1.pin = switchPin;
      mode = SINGLE_BUTTON;
    }
    pinMode(switchPin, INPUT_PULLUP);
    LinkedList::append(this, first, last);
  }
  Selector(pin_t switchPin, pin_t ledPin, buttonType buttonType = MOMENTARY) // One switch or button, one LED
      : switchPin(switchPin), ledPin(ledPin)
  {
    if (buttonType == TOGGLE)
    {
      mode = SINGLE_SWITCH_LED;
    }
    else
    {
      dbButton1.pin = switchPin;
      mode = SINGLE_BUTTON_LED;
    }
    pinMode(switchPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    LinkedList::append(this, first, last);
  }
  template <size_t N>
  Selector(const pin_t (&switchPins)[N]) // Multiple buttons, no LEDs
      : switchPins(switchPins), nb_settings(N)
  {
    mode = MULTIPLE_BUTTONS;
    for (uint8_t i = 0; i < nb_settings; i++)
      pinMode(this->switchPins[i], INPUT_PULLUP);
    LinkedList::append(this, first, last);
  }
  Selector(std::initializer_list<pin_t> switchPins) // Multiple buttons, no LEDs
      : nb_settings(switchPins.size())
  {
    switchPinsStorage = (pin_t *)malloc(sizeof(pin_t) * switchPins.size());
    memcpy(switchPinsStorage, switchPins.begin(), sizeof(pin_t) * switchPins.size());
    this->switchPins = switchPinsStorage;
    mode = MULTIPLE_BUTTONS;
    for (uint8_t i = 0; i < nb_settings; i++)
      pinMode(this->switchPins[i], INPUT_PULLUP);
    LinkedList::append(this, first, last);
  }
  template <size_t M, size_t N>
  Selector(const pin_t (&switchPins)[M], const pin_t (&ledPins)[N]) // One or multiple buttons, multiple LEDs
      : switchPins(switchPins), ledPins(ledPins), nb_settings(N)
  {
    if (M == 1)
    {
      dbButton1.pin = this->switchPins[0];
      mode = INCREMENT_LEDS;
      pinMode(dbButton1.pin, INPUT_PULLUP);
    }
    else if (M == 2)
    {
      dbButton1.pin = this->switchPins[0];
      dbButton2.pin = this->switchPins[1];
      mode = INCREMENT_DECREMENT_LEDS;
      pinMode(dbButton1.pin, INPUT_PULLUP);
      pinMode(dbButton2.pin, INPUT_PULLUP);
    }
    else
    {
      mode = MULTIPLE_BUTTONS_LEDS;
      nb_settings = N < M ? N : M; // min(N, M)
      for (uint8_t i = 0; i < nb_settings; i++)
      {
        pinMode(switchPins[i], INPUT_PULLUP);
      }
    }
    for (uint8_t i = 0; i < nb_settings; i++)
    {
      pinMode(this->ledPins[i], OUTPUT);
    }
    digitalWrite(ledPins[0], HIGH);
    LinkedList::append(this, first, last);
  }
  Selector(std::initializer_list<pin_t> switchPins, std::initializer_list<pin_t> ledPins) // One or multiple buttons, multiple LEDs
      : nb_settings(ledPins.size())
  {

    ledPinsStorage = (pin_t *)malloc(sizeof(pin_t) * ledPins.size());
    memcpy(ledPinsStorage, ledPins.begin(), sizeof(pin_t) * ledPins.size());
    this->ledPins = ledPinsStorage;

    if (switchPins.size() == 1)
    {
      dbButton1.pin = ((pin_t *)switchPins.begin())[0];
      mode = INCREMENT_LEDS;
      pinMode(dbButton1.pin, INPUT_PULLUP);
    }
    else if (switchPins.size() == 2)
    {
      dbButton1.pin = ((pin_t *)switchPins.begin())[0];
      dbButton2.pin = ((pin_t *)switchPins.begin())[1];
      mode = INCREMENT_DECREMENT_LEDS;
      pinMode(dbButton1.pin, INPUT_PULLUP);
      pinMode(dbButton2.pin, INPUT_PULLUP);
    }
    else
    {
      mode = MULTIPLE_BUTTONS_LEDS;
      switchPinsStorage = (pin_t *)malloc(sizeof(pin_t) * switchPins.size());
      memcpy(switchPinsStorage, switchPins.begin(), sizeof(pin_t) * switchPins.size());
      this->switchPins = switchPinsStorage;
      nb_settings = switchPins.size() < ledPins.size() ? switchPins.size() : ledPins.size(); // min(ledPins.size(), switchPins.size())
      for (uint8_t i = 0; i < nb_settings; i++)
      {
        pinMode(this->switchPins[i], INPUT_PULLUP);
      }
    }
    for (uint8_t i = 0; i < nb_settings; i++)
    {
      pinMode(this->ledPins[i], OUTPUT);
    }
    digitalWrite(this->ledPins[0], HIGH);
    LinkedList::append(this, first, last);
  }
  Selector(const pin_t (&switchPins)[2], pin_t nb_settings) // Two buttons (+1, -1), no LEDs
      : switchPins(switchPins), nb_settings(nb_settings)
  {
    dbButton1.pin = switchPins[0];
    dbButton2.pin = switchPins[1];
    mode = INCREMENT_DECREMENT;
    pinMode(dbButton1.pin, INPUT_PULLUP);
    pinMode(dbButton2.pin, INPUT_PULLUP);
    LinkedList::append(this, first, last);
  }
  Selector(const pin_t (&switchPins)[1], pin_t nb_settings) // One button (+1), no LEDs
      : switchPins(switchPins), nb_settings(nb_settings)
  {
    dbButton1.pin = switchPins[0];
    mode = INCREMENT;
    pinMode(dbButton1.pin, INPUT_PULLUP);
    LinkedList::append(this, first, last);
  }
  Selector(std::initializer_list<pin_t> switchPins, pin_t nb_settings) // One or two buttons (+1, (-1)), no LEDs
      : nb_settings(nb_settings)
  {
    if (switchPins.size() == 1)
    {
      dbButton1.pin = ((pin_t *)switchPins.begin())[0];
      mode = INCREMENT;
      pinMode(dbButton1.pin, INPUT_PULLUP);
    }
    else if (switchPins.size() == 2)
    {
      dbButton1.pin = ((pin_t *)switchPins.begin())[0];
      dbButton2.pin = ((pin_t *)switchPins.begin())[1];
      mode = INCREMENT_DECREMENT;
      pinMode(dbButton1.pin, INPUT_PULLUP);
      pinMode(dbButton2.pin, INPUT_PULLUP);
    }
    LinkedList::append(this, first, last);
  }

  ~Selector()
  {
    free(ledPinsStorage);
    free(switchPinsStorage);
    LinkedList::remove(this, first, last);
  }

  void refresh();

  uint8_t getSetting();
  void setSetting(uint8_t newSetting);

#ifdef DEBUG
  const char *getMode();
#endif

  Selector *getNext()
  {
    return next;
  }
  static Selector *getFirst()
  {
    return first;
  }

protected:
  uint8_t Setting = 0;
  virtual void refreshImpl(uint8_t newSetting) {}

private:
  pin_t switchPin, ledPin;
  const pin_t *switchPins, *ledPins;
  pin_t *switchPinsStorage = nullptr, *ledPinsStorage = nullptr;
  uint8_t nb_settings;

  unsigned long prevBounceTime = 0;

  struct debouncedButton
  {
    pin_t pin;
    bool prevState = HIGH;
  } dbButton1, dbButton2;

  const unsigned long debounceTime = 25;

  const static int8_t falling = LOW - HIGH;
  const static int8_t rising = HIGH - LOW;

  enum SelectorMode
  {
    SINGLE_BUTTON,
    SINGLE_BUTTON_LED,
    SINGLE_SWITCH,
    SINGLE_SWITCH_LED,
    MULTIPLE_BUTTONS,
    MULTIPLE_BUTTONS_LEDS,
    INCREMENT_DECREMENT,
    INCREMENT_DECREMENT_LEDS,
    INCREMENT,
    INCREMENT_LEDS
  } mode;

  void refreshLEDs(uint8_t newSetting);
  bool debounceButton(debouncedButton &button);

  Selector *next = nullptr, *previous = nullptr;
  static Selector *last;
  static Selector *first;
  template <class Node>
  friend void LinkedList::append(Node *, Node *&, Node *&);
  template <class Node>
  friend void LinkedList::moveDown(Node *, Node *&, Node *&);
  template <class Node>
  friend void LinkedList::remove(Node *, Node *&, Node *&);
};

#endif // SELECTOR_H_