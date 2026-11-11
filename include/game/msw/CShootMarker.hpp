#ifndef MSW_CSHOOTMARKER_HPP_
#define MSW_CSHOOTMARKER_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    // The marker of where a character shoots (HockeyF
    // game/hockey/CShootMarker.cpp), size 0x60, one per team: a target on
    // the ice at the point the shooter aims at, turned towards the goal,
    // and a second model under it.
    class CShootMarker : public bcs::CTask {
        public:
            CShootMarker(bcs::CTask *pParent, long nTeam);
            virtual ~CShootMarker();
            virtual void Main();
            virtual void MainFunc(float rate);
            virtual void UpdateFunc(float rate);

            // (name guessed) Set to 1 by CChrManHockey::ResetShootMarker.
            void SetReset() { m_bReset = 1; }

        protected:
            void CreateObj_(long nTeam);
            int AppearCheck();

            unsigned char m_Unk34[4];
            bcs::CMdlObj *m_pMdl;       // 0x38, the target
            bcs::CMdlObj *m_pMdlGround; // 0x3C, drawn under it with no depth write
            long m_nTeam;               // 0x40
            bcs::FVEC3 m_vPos;          // 0x44, the point aimed at
            float m_fRotY;              // 0x50, towards the goal
            float m_Unk54;              // 0x54
            long m_bReset;              // 0x58 (name guessed)
            long m_nChr;                // 0x5C, the character the marker follows; -1 for none
    };
}

#endif /* !MSW_CSHOOTMARKER_HPP_ */
