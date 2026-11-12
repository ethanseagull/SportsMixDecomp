#ifndef MSW_CCHROBSTACLE_HPP_
#define MSW_CCHROBSTACLE_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CAtkCollision.hpp"

namespace msw {

    // (DOL class; meaning guessed from the name and CChrMan::ObstacleSearch)
    // A task that gets in the characters' way: Toad's bouncing mushrooms
    // derive from it. 0x94 bytes (CC_CMN::KINOPIO::CcBoundMushroom's second
    // base starts there).
    //
    // INCOMPLETE: the virtual functions are those of
    // __vt__Q33msw11CC09_VOLLEY22CcKinopioBoundMushroom (VolleyF) up to its
    // first secondary vtable; return types and parameter meanings guessed.
    // Members mapped only where the derived classes in the modules read them.
    class CChrObstacle : public bcs::CUnivTask {
        public:
            CChrObstacle(bcs::CTask *pParent, long, long);
            virtual ~CChrObstacle();
            // Disp, GetAttackRadius, GetAttackRange and IsAttackEnd are
            // inline too: VolleyF's game/volley/CcDaisyFlowerGarden.cpp emits
            // the weak copies (in reverse order) and CChrCoreVolleyDonkey,
            // later in the module, refers to them without a copy of its own.
            virtual void Disp() {}
            // Inline: each module has one weak copy (li r3, 0), emitted by
            // the first file that makes the vtable of a class that does not
            // override it (BasketF: game/basket/CChrCoreBasketSaboten.cpp).
            virtual long SendMessage(long, long, long, long) { return 0; }
            // (meanings guessed) Both read the radius of the first attack
            // unit.
            virtual float GetAttackRadius() const { return m_AtkColl.GetRadius(0); }
            virtual float GetAttackRange() const { return m_AtkColl.GetRadius(0); }
            virtual int IsAttackEnd() const { return 0; }

            // (added for BossF game/boss/CComDrvBoss.cpp, name guessed) The
            // kind of obstacle; the Behemoth's meteors are 0x12.
            long GetObstacleType() const { return m_nObstacleType; }

        protected:
            unsigned char m_Unk34[0x3C - 0x34];
            long m_nObstacleType;        // 0x3C, see GetObstacleType
            unsigned char m_Unk40[0x44 - 0x40];
            bcs::FVEC3 m_Pos;            // 0x44
            unsigned char m_Unk50[0x4];
            float m_fRotY;               // 0x54
            unsigned char m_Unk58[0x4];
            float m_fRadius;             // 0x5C
            unsigned char m_Unk60[0x88 - 0x60];
            // 0x88 (name guessed): read by GetAttackRadius/GetAttackRange.
            CAtkCollision m_AtkColl;
    };
}

#endif /* !MSW_CCHROBSTACLE_HPP_ */
