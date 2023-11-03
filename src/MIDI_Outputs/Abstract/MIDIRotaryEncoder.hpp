#pragma once

#include "MIDI_Outputs/Abstract/MIDIAddressable.hpp"
#include <AH/STL/utility> // std::forward
#include <Def/Def.hpp>
#include <Def/TypeTraits.hpp>
#include <MIDI_Outputs/Abstract/EncoderState.hpp>
#include <MIDI_Outputs/Abstract/MIDIOutputElement.hpp>
#include <MIDI_Outputs/Abstract/MIDIAddressable.hpp>

#ifdef ARDUINO
#include <Submodules/Encoder/AHEncoder.hpp>
#else
#include <Encoder.h> // Mock
#endif

AH_DIAGNOSTIC_WERROR()

BEGIN_CS_NAMESPACE

/**
 * @brief   An abstract class for rotary encoders that send MIDI events.
 */
template <class Enc, class Sender>
class GenericMIDIRotaryEncoder : public MIDIOutputElement, public MIDIAddressable {
  public:
    /**
     * @brief   Construct a new MIDIRotaryEncoder.
     *
     * @todo    Documentation
     */
    GenericMIDIRotaryEncoder(Enc &&encoder, MIDIAddress address,
                             int16_t speedMultiply, uint8_t pulsesPerStep,
                             const Sender &sender)
        : MIDIAddressable(address),
          encoder(std::forward<Enc>(encoder)),
          encstate(speedMultiply, pulsesPerStep), sender(sender) {}

    void begin() override { begin_if_possible(encoder); }

    void update() override {
        auto encval = encoder.read();
        if (int16_t delta = encstate.update(encval)) {
            sender.send(delta, address);
        }
    }

    void setSpeedMultiply(int16_t speedMultiply) {
        encstate.setSpeedMultiply(speedMultiply);
    }
    int16_t getSpeedMultiply() const { return encstate.getSpeedMultiply(); }

    int16_t resetPositionOffset() {
        auto encval = encoder.read();
        return encstate.update(encval);
    }

  private:
    Enc encoder;
    EncoderState<decltype(encoder.read())> encstate;

  public:
    Sender sender;
};

template <class Sender>
using MIDIRotaryEncoder = GenericMIDIRotaryEncoder<AHEncoder, Sender>;

template <class Sender>
using BorrowedMIDIRotaryEncoder = GenericMIDIRotaryEncoder<AHEncoder &, Sender>;

END_CS_NAMESPACE

AH_DIAGNOSTIC_POP()
