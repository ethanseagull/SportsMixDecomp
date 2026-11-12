#ifndef BCS_CACCINPUTMAN_HPP_
#define BCS_CACCINPUTMAN_HPP_

namespace bcs {

    // (name guessed; no symbol) Keeps four pointers (one per pad) to objects
    // of 0x2FE8 bytes, in an array in the DOL's .bss. Lives in the DOL. Only
    // the member used so far.
    class CAccInputMan {
        public:
            // (name guessed) Creates the object of every pad that has none.
            // The body is the DOL's fn_800AE9C8.
            static void CreateInstances();

            // (added for game/bcs/CWifiPad.cpp; names guessed) Read the
            // object of pad nPad; -1 (0 for the powers) when it has none.
            // The object runs five stroke detectors (0 to 4);
            // GetStrokeRawN is detector N's last result, GetStrokeN the
            // same with "none" (6) turned into -1. Detector 0 reports a
            // vertical swing of the Remote as 4 and a horizontal one as 5.
            static long GetStrokeRaw0(long nPad);
            static long GetStrokeRaw1(long nPad);
            static long GetStrokeRaw2(long nPad);
            static long GetStrokeRaw3(long nPad);
            static long GetStrokeRaw4(long nPad);
            static long GetStroke0(long nPad);
            static long GetStroke1(long nPad);
            static long GetStroke2(long nPad);
            static long GetStroke3(long nPad);
            static long GetStroke4(long nPad);
            // Clears the detectors of pad nPad; fParam is stored with them.
            static void ResetAcc(long nPad, float fParam);
            static float GetScratchPower(long nPad);
            static float GetNunchukPower(long nPad);
    };
}

#endif /* !BCS_CACCINPUTMAN_HPP_ */
