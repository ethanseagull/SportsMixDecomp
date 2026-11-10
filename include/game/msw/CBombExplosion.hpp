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
    class CBombExplosion : public bcs::CUnivTask {
        public:
            CBombExplosion(bcs::CTask *pParent, const bcs::FVEC3 &rPos, unsigned long, float fRadius, float, float,
                           long, long, unsigned long);
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
            CCollAtkUnit *m_pAtkUnit; // 0x34, created by Create_
            unsigned char m_Unk38[0x50 - 0x38];
            float m_fRadius;          // 0x50, (name guessed) the size of the blast
            unsigned char m_Unk54[0x7C - 0x54];
    };
}

#endif /* !MSW_CBOMBEXPLOSION_HPP_ */
