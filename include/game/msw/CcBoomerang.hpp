#ifndef MSW_CCBOOMERANG_HPP_
#define MSW_CCBOOMERANG_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CC_DATA.hpp"
#include "game/msw/CChrObstacle.hpp"
#include "game/msw/CChrOptMdlObj.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    class CCollAtkUnit;
    class CCollBodyUnit;

    namespace CC_CMN {
        namespace DIDDY {

            // (DOL class; meaning guessed from the names) Diddy Kong's
            // boomerang. 0x108 bytes (VolleyF's CC08_VOLLEY::CcBoomerangVolley,
            // which adds no member, is allocated with that size).
            //
            // INCOMPLETE: the bases and virtual functions are read from
            // __vt__Q33msw11CC08_VOLLEY17CcBoomerangVolley (VolleyF):
            // CChrOptMdlObj at 0x94; the virtuals the class adds come after
            // CChrOptMdlObj's in that vtable. Members mapped only where the
            // modules write them; names and return types guessed.
            class CcBoomerang : public CChrObstacle, public CChrOptMdlObj {
                public:
                    // nPlayer, nTeam: the thrower's player and team numbers.
                    CcBoomerang(bcs::CTask *pParent, long nPlayer, long nTeam);
                    virtual ~CcBoomerang();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);

                    // Starts the boomerang at rPos, facing rRot, flying out
                    // along the yaw fRotY.
                    virtual void Create(nw4r::g3d::ResFile res, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                        const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY); // 0x88
                    virtual void MainFunc_();                                                         // 0x8C
                    virtual void OnAttackHit(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &,
                                             unsigned long);                                          // 0x90
                    // Sets the per-game tuning values (m_fUnkCC, m_afParam).
                    virtual void SetParam_();                                                         // 0x94
                    virtual void SetCollParam_() {}                                                   // 0x98
                    virtual void PlayLoopSe_();                                                       // 0x9C
                    virtual void StopLoopSe_();                                                       // 0xA0

                    // The effect that trails the boomerang.
                    void SetTailEffect_(long nEff);

                protected:
                    unsigned char m_UnkB0[0xCC - 0xB0];
                    float m_fUnkCC;              // 0xCC
                    unsigned char m_UnkD0[0xD8 - 0xD0];
                    float m_afParam[12];         // 0xD8 (meanings not known)
            };
        }
    }
}

#endif /* !MSW_CCBOOMERANG_HPP_ */
