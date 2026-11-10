#ifndef MSW_CITEMBOMB_HPP_
#define MSW_CITEMBOMB_HPP_

#include "game/msw/CItem.hpp"

namespace msw {

    // The bomb (item type 3). A DOL class of size 0x1C0. The overrides and
    // new virtuals are read from __vt__Q23msw13CItemBossBomb (BossF); return
    // types are guesses except for the inline bodies, which the modules emit.
    class CItemBomb : public CItem {
        public:
            CItemBomb(CItemMan *pMan);
            virtual ~CItemBomb();

            virtual void Create(const bcs::FVEC3 &rPos);
            virtual long GetItemType() const { return 3; }
            virtual float GetBodyCollRadius() const;
            virtual float GetAtkCollRadius() const;
            virtual bool CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual float GetParamHeight() const;
            virtual float GetParamAttachHeight() const;
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateDrop();
            virtual void LoopStateDrop();
            virtual void CtorStateAttach();
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();

            virtual void GetExplosionParam(float &, float &, float &, long &, long &, unsigned long &);
            virtual void StartEffect_();
            virtual void StopEffect_();
            virtual void DoExplode_();

        protected:
            unsigned char m_Unk1AC[0x1C0 - 0x1AC];
    };
}

#endif /* !MSW_CITEMBOMB_HPP_ */
