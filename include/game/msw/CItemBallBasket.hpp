#ifndef MSW_CITEMBALLBASKET_HPP_
#define MSW_CITEMBALLBASKET_HPP_

#include "game/msw/CItemBall.hpp"
#include "bcs/CMdlObj.hpp"

namespace msw {

    namespace BALL_BASKET {
        struct SHOT_PARAM;
    }

    // Base of the item balls of the basket game (BasketF). Adds the shot
    // parameters of the item, its display scale and the bomb's blast centre
    // (slots 0x98 to 0xA8, after CItemBall's). The item kinds are in
    // CItemBallBasketKinds.hpp; the out-of-line members are defined in
    // game/basket/CItemBallBasketBomb.cpp, which also emits the vtable.
    //
    // The functions defined in the class are inline in the original: their
    // weak copies sit in earlier BasketF units (CBallBasket.cpp,
    // unit_A3AF0), where the linker kept the first one.
    class CItemBallBasket : public CItemBall {
        public:
            // Inline: every item kind's constructor has it. The model starts
            // at this class's display scale (1).
            CItemBallBasket(CBall *pBall, long type) : CItemBall(pBall, type)
            {
                bcs::CMdlObj *pMdl = m_pMdl;
                if (pMdl != 0) {
                    float scale = CItemBallBasket::GetDispScale();
                    pMdl->m_vScale.x = scale;
                    pMdl->m_vScale.y = scale;
                    pMdl->m_vScale.z = scale;
                }
            }

            virtual ~CItemBallBasket();
            virtual bool DoTrg(long trg);
            virtual bool IsDisableRebound() const { return true; }
            virtual float GetScale() const { return 1.0f; }
            virtual long GetJumpShootTailEffect() const { return -1; }
            virtual void MainFunc_();
            virtual bool TrgFreeMode_();

            // New virtual functions (slots 0x98 to 0xA8).
            // The scale the item model is shown at (name from the symbol).
            virtual float GetDispScale() const { return 1.0f; }
            // Fills in how a shot with this item flies.
            virtual void GetShotParam_(BALL_BASKET::SHOT_PARAM &rParam) const {}
            virtual void SetExplodeCenterPos(const bcs::FVEC3 &rPos) {}
            virtual bcs::FVEC3 GetExplodeCenterPos() const { return bcs::FVEC3(0.0f, 0.0f, 0.0f); }
            // The radius of the blast (0: no blast).
            virtual float GetExplodeR() const { return 0.0f; }

            // The strength of the blast of an item of this kind.
            float GetPower() const;
            // The curve given to a shot with an item of `type` (the red shell
            // and the banana use it).
            static float GetCurveValue(long type);

        protected:
            // (name guessed) Bit 0 of m_nTrgFlag: TrgDestroy_ has run.
            int IsDestroyed() const { return m_nTrgFlag & 1; }
    };

    // Declared first and defined inline below the class (guess, kept from the
    // HockeyF version): every kind's destructor calls CItemBall's directly.
    inline CItemBallBasket::~CItemBallBasket() {}
}

#endif /* !MSW_CITEMBALLBASKET_HPP_ */
