#ifndef MSW_CCOVACUUMFORCE_HPP_
#define MSW_CCOVACUUMFORCE_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CBodyCollision.hpp"
#include "game/msw/CChrObstacle.hpp"

namespace msw {
    namespace CC_CMN {
        namespace LUIGI {

            // (DOL class; meaning guessed from the names) The pull of Luigi's
            // vacuum in his special: while enabled it draws the characters of
            // the other team towards a point in front of him, harder the
            // closer they are and the nearer to his facing, and knocks back
            // those that stay in it too long. 0x104 bytes. Defined in
            // game/msw/CC_CMN/LUIGI/CcOvacuumForce.cpp; member names guessed.
            class CcOvacuumForce : public CChrObstacle {
                public:
                    enum {
                        // characters CChrMan can hold (size of m_afPullFrame)
                        CHR_NUM = 8
                    };

                    CcOvacuumForce(bcs::CTask *pParent, long nPlayer, long nTeam);
                    virtual ~CcOvacuumForce();
                    virtual void Main();

                    // bEnable switches the pull on or off; the body
                    // collision is on only while both are set. Switched
                    // off, the pull goes on for fKeepFrame frames.
                    void SetForceEnable(int bEnable, int bCollision, float fKeepFrame);
                    void SetForceBasePos(const bcs::FVEC3 &pos);
                    void SetForceYRot(float fRotY);
                    // Frames in the pull after which a character is knocked
                    // back (0: never).
                    void SetForceLimit(float fLimit);
                    // Pull speed at the edge and at the centre (per second),
                    // distance of full and of no pull, and angle off the
                    // facing (degrees) of full and of no pull.
                    void SetForceParam(float fSpeedMin, float fSpeedMax, float fDistNear, float fDistFar,
                                       float fAngleNear, float fAngleFar);
                    void SetForceDmgEffectFrm(float fFrame);

                private:
                    void Force_(float fFrame);

                    int m_bEnable;                  // 0x94
                    float m_fKeepFrame;             // 0x98
                    long m_nPlayer;                 // 0x9C, Luigi's player number (not pulled)
                    long m_nTeam;                   // 0xA0, his team (not pulled)
                    CBodyCollision m_BodyColl;      // 0xA4
                    float m_afPullFrame[CHR_NUM];   // 0xB0, frames each character has been pulled
                    float m_fLimit;                 // 0xD0
                    float m_fSpeedMin;              // 0xD4, per frame
                    float m_fSpeedMax;              // 0xD8, per frame
                    float m_fDistNear;              // 0xDC
                    float m_fDistFar;               // 0xE0
                    float m_fAngleNear;             // 0xE4, radians
                    float m_fAngleFar;              // 0xE8, radians
                    // 0xEC-0x100: the arguments of CChrCore::SetHookDamage
                    // for the knock-back (meanings as unknown as there).
                    unsigned long m_nDmgFlag;       // 0xEC
                    float m_fDmgEffectFrm;          // 0xF0
                    long m_nDmgArgF4;               // 0xF4
                    float m_fDmgArgF8;              // 0xF8
                    long m_nDmgArgFC;               // 0xFC
                    long m_nDmgArg100;              // 0x100
            };
        }
    }
}

#endif /* !MSW_CCOVACUUMFORCE_HPP_ */
