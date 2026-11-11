#ifndef MSW_CCOLLMANHOCKEY_HPP_
#define MSW_CCOLLMANHOCKEY_HPP_

#include "game/msw/CCollMan.hpp"

namespace msw {

    // The collision manager of the hockey game (HockeyF), size 0xFC: CCollMan
    // (0x7C) and four goal guardians (the boxes that keep the puck out of a
    // goal while a keeper stands in it; names guessed). The party game uses
    // another class, local to game/hockey/CCollManHockey.cpp.
    class CCollManHockey : public CCollMan {
        public:
            // One guardian, 0x20 bytes; set through the four virtuals below.
            struct GOAL_GUARDIAN {
                int m_bEnable;      // 0x00
                float m_fRotY;      // 0x04, radians, kept in [0, 2pi]
                bcs::FVEC3 m_Pos;   // 0x08
                bcs::FVEC3 m_Size;  // 0x14
            };

            enum { GOAL_GUARDIAN_NUM = 4 };

            CCollManHockey(bcs::CTask *pParent) : CCollMan(pParent)
            {
                for (int i = 0; i < GOAL_GUARDIAN_NUM; i++) {
                    m_aGuardian[i].m_bEnable = 0;
                    m_aGuardian[i].m_Pos.Set(0.0f, 0.0f, 0.0f);
                    m_aGuardian[i].m_Size.Set(0.0f, 0.0f, 0.0f);
                    m_aGuardian[i].m_fRotY = 0.0f;
                }
            }
            virtual ~CCollManHockey();
            virtual void Main();
            virtual void Disp();

            virtual int DetectMapHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                                      const bcs::FVEC3 &rTo, float fRadius, long nKind, unsigned long flags);
            virtual int DetectBallGoalIn_(long &rGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, const bcs::FVEC3 &rFrom,
                                          const bcs::FVEC3 &rTo, float fRadius);
            virtual int DetectBallGoalOut_(long &rGoal, bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal,
                                           const bcs::FVEC3 &rFrom, const bcs::FVEC3 &rTo, float fRadius);
            virtual void EnableGoalGuardian_(long n, int bEnable);
            virtual void SetGoalGuardianPos_(long n, const bcs::FVEC3 &rPos);
            virtual void SetGoalGuardianRotY_(long n, float fRotY);
            virtual void SetGoalGuardianSize_(long n, const bcs::FVEC3 &rSize);

            // Returns the instance, creating it if there is none yet.
            static CCollMan *CreateInstance(bcs::CTask *pParent);

        protected:
            int DetectGoalHit_(bcs::FVEC3 &rPos, bcs::FVEC3 &rNormal, float &rTime, const bcs::FVEC3 &rFrom,
                               const bcs::FVEC3 &rTo, float fRadius);

            GOAL_GUARDIAN m_aGuardian[GOAL_GUARDIAN_NUM]; // 0x7C
    };
}

#endif /* !MSW_CCOLLMANHOCKEY_HPP_ */
