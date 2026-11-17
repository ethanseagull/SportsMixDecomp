#ifndef MSW_CCHANGEFIELDDEMO_HPP_
#define MSW_CCHANGEFIELDDEMO_HPP_

#include "game/msw/CEventModel.hpp"
#include "game/msw/CEventMoveAction.hpp"
#include "game/msw/CEventScene.hpp"

namespace msw {

    // What a character of the change-field demo plays while it walks to its
    // new place: the run, walk or idle motion by its speed. 0x14 bytes, as
    // CEventMoveAction. (DodgeF game/dodge/CChangeFieldDemo.cpp)
    class CEvMoveActFieldDemo : public CEventMoveAction {
        public:
            CEvMoveActFieldDemo(CEventModel *pModel);
            virtual ~CEvMoveActFieldDemo();

            virtual void UpdateFrame(const bcs::FVEC3 &pos, const bcs::FVEC3 &rot, float fFrame);
    };

    // The dodgeball change-field demo (CEventDemoDodge's demo 6, 0xF8
    // bytes): characters going out of the field to the outside and back in,
    // shown one change at a time (up to four), each with a telop naming the
    // characters. An event scene of resource 0x60. Member names guessed.
    class CChangeFieldDemo : public CEventScene {
        public:
            // One change: a team going out of or into the field (0x10 bytes).
            struct FIELD_WORK {
                signed char nTeam;      // 0x00
                signed char nOut;       // 0x01: 1 going out, 0 coming in
                signed char nPlayer[2]; // 0x02: the players named in the telop, -1 for none
                signed char nChr[4];    // 0x04: the characters that move, -1 for none
                float fStart;           // 0x08: frame of the scene the change starts at
                float fEnd;             // 0x0C: and ends at
            };

            enum { FIELD_MAX = 4 };

            // pField: one byte per character, 1 going out, 0 coming in, -1
            // staying. (the last argument is not used)
            CChangeFieldDemo(bcs::CTask *pParent, char *pField, long nCount);
            virtual ~CChangeFieldDemo();

            virtual nw4r::g3d::ResFile GetRes();
            virtual int EventFirst();
            virtual void EventStart();
            virtual void EventKill() { Delete(); }
            virtual long EventPlay(CUT_WORK &rCut);
            virtual long EventCansel(CUT_WORK &rCut, long nRet);
            virtual void CheckEventSkip();
            virtual int IsEventSkip();
            virtual void EventTimer(short nId, short nField, unsigned long nIdx);
            virtual void EventTimerReset(short nId, short, unsigned long);
            virtual void EventFlags(unsigned long, unsigned long);

        private:
            // Fill change nIdx with the characters of team nTeam that come
            // in (go out); the mask of the characters used.
            unsigned long _SetIn(long nIdx, long nTeam, char *pField);
            unsigned long _SetOut(long nIdx, long nTeam, char *pField);
            // _SetOut (_SetIn) for the next change, counted when it is used.
            unsigned short _AddOut(long nTeam, char *pField);
            unsigned short _AddIn(long nTeam, char *pField);
            // Puts character nIdx of change nField on its node path; the
            // character, -1 when it has none.
            long _SetMoveCommon(long nField, long nIdx);
            void _SetMoveWalk(long nField, long nIdx, long nMot);

            // Plays the sequence pData on character nChr.
            void _PlayMotion(long nChr, const CEventModel::MOT_DATA *pData);

            long m_nCutIdx[FIELD_MAX];          // 0xA4: first cut of each change
            FIELD_WORK m_Field[FIELD_MAX];      // 0xB4
            signed char m_nFieldNum;            // 0xF4
            signed char m_bStage5;              // 0xF5: the game is on stage 5
            signed char m_bButton;              // 0xF6: the skip button window is up
            unsigned char m_nSkipBit;           // 0xF7: skip bit asked for by the button
    }; // size 0xF8
}

#endif /* !MSW_CCHANGEFIELDDEMO_HPP_ */
