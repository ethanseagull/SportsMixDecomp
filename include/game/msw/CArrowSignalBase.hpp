#ifndef MSW_CARROWSIGNALBASE_HPP_
#define MSW_CARROWSIGNALBASE_HPP_

#include "bcs/CTask.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    // The arrow model shown over a character to point him out. A DOL class
    // deriving from bcs::CTask; the game modules derive their own
    // (CArrowSignalBasket in BasketF), which supply Main. Fields from 0x34
    // read from the DOL constructor, destructor and CreateMdl_ and from
    // CArrowSignalBasket; size not known (no allocation site in the DOL).
    // Names are guesses.
    class CArrowSignalBase : public bcs::CTask {
        public:
            CArrowSignalBase(bcs::CTask *pParent, long nChrIdx, long nTeam);
            virtual ~CArrowSignalBase();
            // Empty in the DOL (fn_800F16D8).
            virtual void Main();

        protected:
            // Loads the arrow model of m_nChrIdx's character, coloured for
            // m_nTeam.
            void CreateMdl_(float, float);

            bcs::CMdlObj *m_pMdl; // 0x34, released by the destructor
            long m_nChrIdx;       // 0x38, passed to CChrMan::GetChrCore
            long m_nTeam;         // 0x3C, passed to CGame::GetTeamColorIdx
            long m_nState;        // 0x40
            long m_nCount;        // 0x44, counted up by the modules
            float m_Unk48;        // 0x48
            float m_Unk4C;        // 0x4C
            float m_Unk50;        // 0x50
            float m_Unk54;        // 0x54
            float m_Unk58;        // 0x58
            float m_Unk5C;        // 0x5C
    };
}

#endif /* !MSW_CARROWSIGNALBASE_HPP_ */
