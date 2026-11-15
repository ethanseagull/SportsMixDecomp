#ifndef MSW_CBOMBEXPLOSION_HPP_
#define MSW_CBOMBEXPLOSION_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {

    class CCollAtkUnit;

    // The blast of a bomb: a task that holds an attack collision for a while.
    // A DOL class of size 0x7C; BossF derives CBombExplosionBoss from it.
    //
    // The virtual functions are in the order of the slots of
    // __vt__Q23msw18CBombExplosionBoss. GetAtkIndex is inline (the modules
    // emit it). Return types are read from how Create_ in the DOL uses the
    // results; argument meanings are not known.
    //
    // INCOMPLETE: only the members module code uses are named.
    class CBombExplosion : public bcs::CUnivTaskEx {
        public:
            // fDownFrame, fUnk98, nUnk9C, nUnk9E and nNoHitFlag are handed to
            // the attack unit by Create_ (CCollAtkUnit's fields of those
            // offsets); hMdl is passed to Create_ (names guessed).
            CBombExplosion(bcs::CTask *pParent, const bcs::FVEC3 &rPos, unsigned long hMdl, float fDownFrame,
                           float fRadius, float fUnk98, long nUnk9C, long nUnk9E, unsigned long nNoHitFlag);
            virtual ~CBombExplosion();
            virtual void Main();

            virtual void SetAttackPower(float power);
            virtual unsigned long GetAtkType() const;
            virtual long GetAtkIndex() const { return -1; }
            virtual unsigned long GetNoHitFlag() const;
            // Shows the blast; called first by Create_.
            virtual void CreateEffect_(const bcs::FVEC3 &rPos);
            // Creates the attack collision unit at rPos.
            virtual void Create_(const bcs::FVEC3 &rPos, unsigned long);

        protected:
            // The states of Main (names guessed).
            enum {
                STATE_CREATE = 0, // creates the attack unit
                STATE_GROW = 1,   // the blast grows to its full size
                STATE_HOLD = 2,   // full size until m_fUnk70 runs out
                STATE_END = 99
            };

            CCollAtkUnit *m_pAtkUnit; // 0x34, created by Create_
            float m_fTimer;           // 0x38, (name guessed) frames since the blast was created
            bcs::FVEC3 m_Pos;         // 0x3C, where the blast is
            unsigned long m_hMdl;     // 0x48, (name guessed) passed to Create_
            long m_nState;            // 0x4C, STATE_*
            float m_fRadius;          // 0x50, (name guessed) the size of the blast
            float m_fDownFrame;       // 0x54, (names guessed) the attack unit's fields of these offsets
            float m_fUnk98;           // 0x58
            long m_nUnk9C;            // 0x5C
            long m_nUnk9E;            // 0x60
            unsigned long m_nNoHitFlag; // 0x64, or'ed with GetNoHitFlag()
            // 0x68, the attack unit's power (SetAttackPower; CCollAtkUnit
            // 0x88). DodgeF's CreateEffect_ sets 25, 15 or 35.
            float m_fUnk68;
            float m_fGrowTime;        // 0x6C, (name guessed) frames to grow to full size
            // 0x70, (name guessed: the frames the blast stays at full size)
            // DodgeF's CreateEffect_ sets 6, 4 or 10.
            float m_fUnk70;

        public:
            // 0x74, 0x78 (names guessed): a character index (-1: none) told
            // about every character the blast hits, and whether the first
            // hit of another character is still to be reported. Public: the
            // file's attack-hit callback (not a member) reads them.
            long m_nOwnerChr;
            long m_bReportHit;
    };
}

#endif /* !MSW_CBOMBEXPLOSION_HPP_ */
