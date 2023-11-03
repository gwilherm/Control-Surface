#pragma once

#include <AH/Hardware/Button.hpp>
#include <Def/Def.hpp>
#include <MIDI_Outputs/Abstract/MIDIOutputElement.hpp>
#include <MIDI_Outputs/Abstract/MIDIAddressable.hpp>

BEGIN_CS_NAMESPACE

/**
 * @brief   A class for latching buttons and switches that send MIDI events.
 *
 * The button is debounced.
 *
 * @see     Button
 */
template <class Sender>
class MIDIButtonLatching : public MIDIOutputElement, public MIDIAddressable {
  protected:
    /**
     * @brief   Construct a new MIDIButtonLatching.
     *
     * @param   pin
     *          The digital input pin with the button connected.
     *          The internal pull-up resistor will be enabled.
     * @param   address
     *          The MIDI address to send to.
     * @param   sender
     *          The MIDI sender to use.
     */
    MIDIButtonLatching(pin_t pin, MIDIAddress address, const Sender &sender)
        : MIDIAddressable(address), button(pin), sender(sender) {}

  public:
    void begin() override { button.begin(); }
    void update() override {
        AH::Button::State state = button.update();
        if (state == AH::Button::Falling || state == AH::Button::Rising) {
            sender.sendOn(address);
            sender.sendOff(address);
        }
    }

    AH::Button::State getButtonState() const { return button.getState(); }

  private:
    AH::Button button;

  public:
    Sender sender;
};

END_CS_NAMESPACE