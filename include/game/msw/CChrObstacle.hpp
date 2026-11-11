#ifndef MSW_CCHROBSTACLE_HPP_
#define MSW_CCHROBSTACLE_HPP_

#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

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
            virtual void Disp();
            virtual void SendMessage(long, long, long, long);
            virtual float GetAttackRadius() const;
            virtual float GetAttackRange() const;
            virtual int IsAttackEnd() const;

        protected:
            unsigned char m_Unk34[0x44 - 0x34];
            bcs::FVEC3 m_Pos;            // 0x44
            unsigned char m_Unk50[0x4];
            float m_fRotY;               // 0x54
            unsigned char m_Unk58[0x4];
            float m_fRadius;             // 0x5C
            unsigned char m_Unk60[0x94 - 0x60];
    };
}

#endif /* !MSW_CCHROBSTACLE_HPP_ */
