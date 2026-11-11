#ifndef MSW_CITEMDODGEDD_HPP_
#define MSW_CITEMDODGEDD_HPP_

#include "game/msw/CItemManDodge.hpp"
#include "game/msw/CItemDodge.hpp"
#include "game/msw/CBombExplosion.hpp"
#include "bcs/CSndTask.hpp"
#include "bcs/CTaskMan.hpp"

namespace msw {

    // The item manager of the "DD" dodgeball game (name of the mode not
    // known): items thrown in it are the DD bombs, one class per kind 7
    // to 11. Size of CItemManDodge.
    class CItemManDodgeDD : public CItemManDodge {
        public:
            CItemManDodgeDD(bcs::CTask *pParent) : CItemManDodge(pParent) {}
            virtual ~CItemManDodgeDD() {}

            virtual CItem *CreateThrowItem(long kind, const bcs::FVEC3 &pos);
    };

    // The DD bombs: a dodgeball bomb that counts down after it has been
    // thrown and blows up when the countdown is over, or at once when it
    // hits (the kinds differ in when). Size 0x1D0 (CItemManDodgeDD's
    // allocation). ExParamUpdate_ is pure here (null in the vtable).
    class CItemDodgeBombDD_Base : public CItemDodgeBomb {
        public:
            CItemDodgeBombDD_Base(CItemMan *pMan)
                : CItemDodgeBomb(pMan), m_bTimer(0), m_bHit(0), m_hSe(0), m_fExplodeWait(0.0f)
            {
            }
            virtual ~CItemDodgeBombDD_Base();

            virtual void Create(const bcs::FVEC3 &pos);
            virtual int DoParabolaThrow(const bcs::FVEC3 &, float, float);
            virtual float GetBombExplodeRestFrameDD() const { return m_fExplodeRestFrame; }
            virtual void CallbackBodyPush_(CCollBodyUnit *, CCollBodyUnit *, bcs::FVEC3 *);
            virtual bool CallbackBodyDamage_(CCollBodyUnit *, const CCollAtkUnit *, const bcs::FVEC3 &);
            virtual void ExParamUpdate_() = 0;
            virtual long GetParamThrowType() const { return 1; }
            virtual float GetParamThrowSearchDirRangeMax() const { return 0.0f; }
            virtual float GetParamThrowSearchDistRangeMax() const { return 0.0f; }
            virtual void DoThrowInit(const bcs::FVEC3 &, int);
            virtual void CtorStateThrow();
            virtual void LoopStateThrow();
            virtual void StopEffect_();

            virtual void DoExplode_();

        protected:
            // (name guessed) Stops the fuse sound, if there is one.
            void StopSe_()
            {
                if (m_hSe != 0) {
                    bcs::CSndTask *pSe = static_cast<bcs::CSndTask *>(bcs::CTaskMan::GetTask(m_hSe));
                    if (pSe != 0) {
                        pSe->Stop(0.0f);
                    }
                    m_hSe = 0;
                }
            }

            long m_bTimer;        // 0x1C0, (names guessed) 1 for the time bomb: counts down from the throw
            long m_bHit;          // 0x1C4, the time bomb has been hit by an attack of kind 0x15
            unsigned long m_hSe;  // 0x1C8, a sound task
            float m_fExplodeWait; // 0x1CC, frames the blast waits for after the countdown; size 0x1D0
    };

    class CItemDodgeBombDD_Small : public CItemDodgeBombDD_Base {
        public:
            CItemDodgeBombDD_Small(CItemMan *pMan) : CItemDodgeBombDD_Base(pMan) {}
            virtual ~CItemDodgeBombDD_Small() {}

            virtual long GetItemType() const { return 7; }
            virtual void ExParamUpdate_();
    };

    class CItemDodgeBombDD_Nml : public CItemDodgeBombDD_Base {
        public:
            CItemDodgeBombDD_Nml(CItemMan *pMan) : CItemDodgeBombDD_Base(pMan) {}
            virtual ~CItemDodgeBombDD_Nml() {}

            virtual long GetItemType() const { return 8; }
            virtual void ExParamUpdate_();
    };

    class CItemDodgeBombDD_Mega : public CItemDodgeBombDD_Base {
        public:
            CItemDodgeBombDD_Mega(CItemMan *pMan) : CItemDodgeBombDD_Base(pMan) {}
            virtual ~CItemDodgeBombDD_Mega() {}

            virtual long GetItemType() const { return 9; }
            virtual void ExParamUpdate_();
    };

    class CItemDodgeBombDD_Time : public CItemDodgeBombDD_Base {
        public:
            CItemDodgeBombDD_Time(CItemMan *pMan) : CItemDodgeBombDD_Base(pMan) { m_bTimer = 1; }
            virtual ~CItemDodgeBombDD_Time() {}

            virtual long GetItemType() const { return 10; }
            virtual void ExParamUpdate_();
    };

    class CItemDodgeBombDD_LockOn : public CItemDodgeBombDD_Base {
        public:
            CItemDodgeBombDD_LockOn(CItemMan *pMan) : CItemDodgeBombDD_Base(pMan) {}
            virtual ~CItemDodgeBombDD_LockOn() {}

            virtual long GetItemType() const { return 11; }
            virtual void ExParamUpdate_();
    };
}

// The blast of a DD bomb: its size, effect, sound and camera quake depend
// on the kind of the bomb. Outside namespace msw in the original
// (__vt__16CBombExplosionDD). Size 0x80.
class CBombExplosionDD : public msw::CBombExplosion {
    public:
        // The kind comes second: DoExplode_ reads the other arguments
        // before it calls GetItemType, and that before GetTempParent.
        CBombExplosionDD(bcs::CTask *pParent, long nItemType, const bcs::FVEC3 &rPos, unsigned long arg,
                         float fRadius, float f2, float f3, long l1, long l2, unsigned long ul)
            : msw::CBombExplosion(pParent, rPos, arg, fRadius, f2, f3, l1, l2, ul), m_nItemType(nItemType)
        {
        }
        virtual ~CBombExplosionDD() {}

        virtual void CreateEffect_(const bcs::FVEC3 &rPos);

    protected:
        long m_nItemType; // 0x7C, the kind of the bomb (7 to 11)
};

#endif /* !MSW_CITEMDODGEDD_HPP_ */
