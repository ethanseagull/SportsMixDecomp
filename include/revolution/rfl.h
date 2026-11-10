#ifndef REVOLUTION_RFL_H_
#define REVOLUTION_RFL_H_

// The Mii library (RFL). Only what the game's code passes around so far,
// written from the mangled names and the code that uses them; member names
// are guesses.

#ifdef __cplusplus
extern "C" {
#endif

// Where a Mii's data comes from (passed and stored as a word).
typedef enum RFLDataSource {
    RFLDataSource_Official = 0
} RFLDataSource;

// Size 8. Identifies one Mii; copied as two words.
typedef struct RFLCreateID {
    unsigned char data[8];
} RFLCreateID;

// A Mii as stored, 0x4C bytes. Its contents are not used by the game's own
// code, so they are not spelled out here.
typedef struct RFLStoreData {
    unsigned char data[0x4C];
} RFLStoreData;

#ifdef __cplusplus
}
#endif

#endif /* !REVOLUTION_RFL_H_ */
