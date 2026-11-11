#ifndef MSW_CCHRMANBASKET_HPP_
#define MSW_CCHRMANBASKET_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CChrMan.hpp"

namespace msw {

    class CNpcBossPackun;

    // INCOMPLETE: the character manager of the basketball game (BasketF).
    // Allocated with 0xE8 and 0xEC bytes (the second by a derived class);
    // only some members are mapped (the rest is padding up to 0xE8), so it
    // must not be instantiated yet.
    class CChrManBasket : public CChrMan {
        public:
            // The boss Packun whose index (the NPC's virtual at 0x80) is
            // nIdx; null when there is none.
            CNpcBossPackun *GetBossPackun_(long nIdx) const;
            // Makes the boss Packun nIdx at rPos (the Hungry Boss Packun
            // stages call it for the boss that carries goal 0).
            CNpcBossPackun *CreateBossPackun_(const bcs::FVEC3 &rPos, long nIdx);

            // The instance as this class (guessed inline: the callers call
            // CChrMan::GetInstance and use the result directly).
            static CChrManBasket *GetInstance() { return static_cast<CChrManBasket *>(CChrMan::GetInstance()); }

            // Added for game/basket/CChrDrvPadBasket.cpp (names guessed).
            // Blocks a change of character by team nTeam's pad for a while
            // (sets m_fChgInputDisableFrame[nTeam]).
            void OnChgCharacterInputDiableFrame_(long nTeam);
            // Starts a combination play of pChr's team; zero when none
            // starts.
            int PlayCombination_(const CChrCore *pChr);
            // Not zero when team nTeam may change its character by pad.
            // (guessed inline: the drivers read the member after calling
            // CChrMan::GetInstance)
            static int IsChgCharacterInputEnable(long nTeam)
            {
                return GetInstance()->m_fChgInputDisableFrame[nTeam] <= 0.0f;
            }

        protected:
            unsigned char m_Unk8C[0xD4 - 0x8C];
            // 0xD4 (name guessed): per team, the frames left before the pad
            // may change character again (OnChgCharacterInputDiableFrame_
            // writes it).
            float m_fChgInputDisableFrame[2];
            unsigned char m_UnkDC[0xE8 - 0xDC];
    };
}

#endif /* !MSW_CCHRMANBASKET_HPP_ */
