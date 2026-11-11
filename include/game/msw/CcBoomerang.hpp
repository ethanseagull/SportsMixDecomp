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

            // (DOL class; meaning guessed from the names) The banana
            // boomerang Diddy Kong throws in his special shot. 0x108 bytes
            // (BasketF allocates its CC08_BASKET::CcBoomerangBasket, which
            // adds no member, with that size).
            //
            // INCOMPLETE: the virtual functions are read from
            // __vt__Q33msw11CC08_BASKET17CcBoomerangBasket (BasketF); the
            // ones the class adds come after CChrOptMdlObj's in that vtable.
            // Return types and parameter meanings guessed. Members mapped
            // only where the modules write them.
            class CcBoomerang : public CChrObstacle, public CChrOptMdlObj {
                public:
                    // nPlayer and nTeam: the thrower's player and team numbers.
                    CcBoomerang(bcs::CTask *pParent, long nPlayer, long nTeam);
                    virtual ~CcBoomerang();
                    virtual void Main();
                    virtual long SendMessage(long, long, long, long);

                    // Puts the boomerang at rPos turned by rRot, flying
                    // towards the yaw fRotY.
                    virtual void Create(nw4r::g3d::ResFile file, const CC_DATA::ARTS_OPT_CREATE_INFO *pInfo,
                                        const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, float fRotY);
                    virtual void MainFunc_();
                    virtual void OnAttackHit(const CCollAtkUnit *pAtk, const CCollBodyUnit *pBody,
                                             const bcs::FVEC3 &rPos, unsigned long);
                    // The per-game tuning (m_fUnkCC, m_afParam).
                    virtual void SetParam_();
                    // Inline: the weak copy is emitted by the first file that
                    // makes a vtable of the class (BasketF:
                    // game/basket/CChrCoreBasketDiddy.cpp).
                    virtual void SetCollParam_() {}
                    virtual void PlayLoopSe_();
                    virtual void StopLoopSe_();

                protected:
                    // The effect that trails the boomerang.
                    void SetTailEffect_(long nEff);

                    unsigned char m_UnkB0[0xCC - 0xB0];
                    float m_fUnkCC;              // 0xCC (meaning not known; Basket: 180)
                    unsigned char m_UnkD0[0xD8 - 0xD0];
                    // 0xD8-0x104: tuning values (meanings not known).
                    float m_afParam[12];
            };
        }
    }
}

#endif /* !MSW_CCBOOMERANG_HPP_ */
