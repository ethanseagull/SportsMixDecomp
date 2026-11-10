#ifndef REVOLUTION_RFL_H_
#define REVOLUTION_RFL_H_

// The Mii library's types, only as far as the game's signatures need them.
// Written from the mangled names and the code that uses them; the member
// names are guesses.

// Where a Mii's data comes from (passed and stored as a word).
enum RFLDataSource {
    RFLDataSource_Official = 0
};

// Size 8. Identifies one Mii; copied as two words.
struct RFLCreateID {
    unsigned char data[8];
};

#endif /* !REVOLUTION_RFL_H_ */
