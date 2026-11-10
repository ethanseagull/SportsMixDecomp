#ifndef REVOLUTION_RFL_H_
#define REVOLUTION_RFL_H_

// The Mii library (RFL). Only what the game's code passes around so far.

#ifdef __cplusplus
extern "C" {
#endif

// A Mii as stored, 0x4C bytes. Its contents are not used by the game's own
// code, so they are not spelled out here.
typedef struct RFLStoreData {
    unsigned char data[0x4C];
} RFLStoreData;

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_RFL_H_ */
