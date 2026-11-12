#ifndef MSW_CGAMEGUIDEBASKET_HPP_
#define MSW_CGAMEGUIDEBASKET_HPP_

#include "bcs/CTask.hpp"

namespace msw {

    class CBall;
    class CChrCore;

    // The basketball game's guide (BasketF): watches play and puts up the
    // guide telops of the DOL's CGameGuideMan for the shot (types 8 and 9),
    // the jump block (1) and the jump attack on a dunk (3). Size 0x70
    // (operator new in CGameGuideManBasket::Create). Member names are
    // guesses.
    class CGameGuideBasket : public bcs::CTask {
        public:
            CGameGuideBasket(bcs::CTask *pParent);
            virtual ~CGameGuideBasket();
            virtual void Main();

            void UpDateGuid();
            void DispContinue();
            // Look for a chance of the guide of that name and put it up.
            int JudgeJumpAttack();
            int JudgeJumpAttack__(const CChrCore *pChr, const CChrCore *pHold);
            int JudgeJumpBlock();
            int JudgeJumpBlock_(const CChrCore *pChr, const CBall *pBall);
            void StartGuideTelop(long nType, const CChrCore *pChr);
            // pChr did what guide nType shows; after enough of them the
            // guide is not shown to that player any more.
            void AddSuccessCount(const CChrCore *pChr, long nType);

            // Takes the telop down and forgets its character.
            inline void Reset();
            // Reset, and also clears the timers and the done bits.
            inline void Clear();
            // Not zero while a guide waits for its result (bit 0 of
            // m_nFlags).
            int IsWait() const { return m_nFlags & 1; }
            // Not zero when guide nType is not for pChr: no character, not
            // a pad player, or the player has done it often enough already.
            inline int IsGuideDone(const CChrCore *pChr, long nType) const;
            // Not zero when pChr can be shown a guide at all.
            inline int IsGuideChr(const CChrCore *pChr) const;
            // Not zero while the shot guide (type 8) fits pChr.
            inline int IsShootGuide(const CChrCore *pChr) const;
            // Not zero while the jump shot guide (type 9) fits pChr.
            inline int IsJumpShootGuide(const CChrCore *pChr) const;
            // Puts up the jump shot guide when it fits pChr; not zero then.
            inline int StartJumpShootGuide(const CChrCore *pChr);

            static CGameGuideBasket *g_pInstance;

            const CChrCore *m_pChr;          // 0x34, the character the telop is for
            unsigned long m_nFlags;          // 0x38, bit 0: a guide is waiting for its result
            long m_nType;                    // 0x3C, the telop up now, -1 for none
            long m_nLastType;                // 0x40, the telop that was up last
            long m_nPadNo;                   // 0x44, the pad of m_pChr
            long m_nUnk48;                   // 0x48 (only set to -1)
            float m_fUnk4C;                  // 0x4C, no guide while above zero
            float m_fChrChangeWait;          // 0x50, no guide while above zero
            float m_fUnk54;                  // 0x54, no guide while above zero
            float m_fTelopTime;              // 0x58, how long the telop has been up
            float m_fHoldTime;               // 0x5C, how long the ball has been held
            unsigned long m_nDoneBits[4];    // 0x60, per pad: bit n set when guide n is done
    };
}

#endif /* !MSW_CGAMEGUIDEBASKET_HPP_ */
