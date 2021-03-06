#ifndef MEDIA_WILDMIDI_H
#define MEDIA_WILDMIDI_H

#include "Midi.h"

#ifdef HAVE_WILDMIDI

/* Implementation for opening supported MIDI-like files through WildMidi. */
class WildMidiDevice : public MidiDevice {
    WildMidiDevice(const std::string &soundfont);
public:
    virtual ~WildMidiDevice();

    virtual MidiSongPtr open(const std::string &name) override;

    static void init(const std::string &soundfont);
};

#endif /* HAVE_WILDMIDI */

#endif /* MEDIA_WILDMIDI_H */
