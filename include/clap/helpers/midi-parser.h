/*
 * Simple MIDI parser implementation.
 * I used the following reference:
 * http://www.sonicspot.com/guide/midifiles.html
 */

#ifndef CLAP_HELPERS_MIDI_PARSER_H
# define CLAP_HELPERS_MIDI_PARSER_H

# ifdef __cplusplus
extern "C" {
# endif

# include <stdint.h>
# include <string.h>

# include "../clap.h"

enum clap_midi_parser_status
{
  CLAP_MIDI_PARSER_EOB         = -2,
  CLAP_MIDI_PARSER_ERROR       = -1,
  CLAP_MIDI_PARSER_INIT        = 0,
  CLAP_MIDI_PARSER_HEADER      = 1,
  CLAP_MIDI_PARSER_TRACK       = 2,
  CLAP_MIDI_PARSER_TRACK_MIDI  = 3,
  CLAP_MIDI_PARSER_TRACK_META  = 4,
  CLAP_MIDI_PARSER_TRACK_SYSEX = 5,
};

enum clap_midi_file_format
{
  CLAP_MIDI_FILE_FORMAT_SINGLE_TRACK = 0,
  CLAP_MIDI_FILE_FORMAT_MULTIPLE_TRACKS = 1,
  CLAP_MIDI_FILE_FORMAT_MULTIPLE_SONGS = 2,
};

static const char *
clap_midi_file_format_name(int fmt)
{
  switch (fmt) {
  case CLAP_MIDI_FILE_FORMAT_SINGLE_TRACK: return "single track";
  case CLAP_MIDI_FILE_FORMAT_MULTIPLE_TRACKS: return "multiple tracks";
  case CLAP_MIDI_FILE_FORMAT_MULTIPLE_SONGS: return "multiple songs";

  default: return "(unknown)";
  }
}

struct clap_midi_header
{
  int32_t size;
  uint16_t format;
  int16_t tracks_count;
  int16_t time_division;
};

struct clap_midi_track
{
  int32_t size;
};

enum clap_midi_status
{
  CLAP_MIDI_STATUS_NOTE_OFF   = 0x8,
  CLAP_MIDI_STATUS_NOTE_ON    = 0x9,
  CLAP_MIDI_STATUS_NOTE_AT    = 0xA, // after touch
  CLAP_MIDI_STATUS_CC         = 0xB, // control change
  CLAP_MIDI_STATUS_PGM_CHANGE = 0xC,
  CLAP_MIDI_STATUS_CHANNEL_AT = 0xD, // after touch
  CLAP_MIDI_STATUS_PITCH_BEND = 0xE,
};

static const char *
clap_midi_status_name(int status)
{
  switch (status) {
  case CLAP_MIDI_STATUS_NOTE_OFF: return "Note Off";
  case CLAP_MIDI_STATUS_NOTE_ON: return "Note On";
  case CLAP_MIDI_STATUS_NOTE_AT: return "Note Aftertouch";
  case CLAP_MIDI_STATUS_CC: return "CC";
  case CLAP_MIDI_STATUS_PGM_CHANGE: return "Program Change";
  case CLAP_MIDI_STATUS_CHANNEL_AT: return "Channel Aftertouch";
  case CLAP_MIDI_STATUS_PITCH_BEND: return "Pitch Bend";

  default: return "(unknown)";
  }
}

enum clap_midi_meta
{
  CLAP_MIDI_META_SEQ_NUM         = 0x00,
  CLAP_MIDI_META_TEXT            = 0x01,
  CLAP_MIDI_META_COPYRIGHT       = 0x02,
  CLAP_MIDI_META_TRACK_NAME      = 0x03,
  CLAP_MIDI_META_INSTRUMENT_NAME = 0x04,
  CLAP_MIDI_META_LYRICS          = 0x05,
  CLAP_MIDI_META_MAKER           = 0x06,
  CLAP_MIDI_META_CUE_POINT       = 0x07,
  CLAP_MIDI_META_CHANNEL_PREFIX  = 0x20,
  CLAP_MIDI_META_END_OF_TRACK    = 0x2F,
  CLAP_MIDI_META_SET_TEMPO       = 0x51,
  CLAP_MIDI_META_SMPTE_OFFSET    = 0x54,
  CLAP_MIDI_META_TIME_SIGNATURE  = 0x58,
  CLAP_MIDI_META_KEY_SIGNATURE   = 0x59,
  CLAP_MIDI_META_SEQ_SPECIFIC    = 0x7F,
};

static const char *
clap_midi_meta_name(int type)
{
  switch (type) {
  case CLAP_MIDI_META_SEQ_NUM: return "Sequence Number";
  case CLAP_MIDI_META_TEXT: return "Text";
  case CLAP_MIDI_META_COPYRIGHT: return "Copyright";
  case CLAP_MIDI_META_TRACK_NAME: return "Track Name";
  case CLAP_MIDI_META_INSTRUMENT_NAME: return "Instrument Name";
  case CLAP_MIDI_META_LYRICS: return "Lyrics";
  case CLAP_MIDI_META_MAKER: return "Maker";
  case CLAP_MIDI_META_CUE_POINT: return "Cue Point";
  case CLAP_MIDI_META_CHANNEL_PREFIX: return "Channel Prefix";
  case CLAP_MIDI_META_END_OF_TRACK: return "End of Track";
  case CLAP_MIDI_META_SET_TEMPO: return "Set Tempo";
  case CLAP_MIDI_META_SMPTE_OFFSET: return "SMPTE Offset";
  case CLAP_MIDI_META_TIME_SIGNATURE: return "Time Signature";
  case CLAP_MIDI_META_KEY_SIGNATURE: return "Key Signature";
  case CLAP_MIDI_META_SEQ_SPECIFIC: return "Sequencer Specific";

  default: return "(unknown)";
  }
}


struct clap_midi_midi_event
{
  unsigned status : 4;
  unsigned channel : 4;
  uint8_t  param1;
  uint8_t  param2;
};

struct clap_midi_meta_event
{
  uint8_t        type;
  int32_t        length;
  const uint8_t *bytes;  // reference to the input buffer
};

struct clap_midi_sysex_event
{
  uint8_t        sysex;
  uint8_t        type;
  int32_t        length;
  const uint8_t *bytes;  // reference to the input buffer
};

struct clap_midi_parser
{
  enum clap_midi_parser_status state;

  /* input buffer */
  const uint8_t *in;
  int32_t        size;

  /* result */
  int64_t                      vtime;
  struct clap_midi_header      header;
  struct clap_midi_track       track;
  struct clap_midi_midi_event  midi;
  struct clap_midi_meta_event  meta;
  struct clap_midi_sysex_event sysex;
};

static inline enum clap_midi_parser_status
clap_midi_parse(struct clap_midi_parser *parser);

/* Converts a midi buffer in the state track, into a clap_event.
 * If the midi data can't be converted into clap's events, it is then
 * converted as a clap_midi_event. */
static inline void
clap_midi_convert(const uint8_t     *in,
                  int32_t            size,
                  struct clap_event *event);

# include "midi-parser.c"

# ifdef __cplusplus
}
# endif

#endif /* !CLAP_MIDI_PARSER_H */
