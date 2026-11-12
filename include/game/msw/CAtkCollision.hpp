#ifndef MSW_CATKCOLLISION_HPP_
#define MSW_CATKCOLLISION_HPP_

#include "bcs/FVEC3.hpp"
#include "game/msw/CCollAtkUnit.hpp"

namespace msw {
    class CCollBodyUnit;
    class CCollUnit;

    // A set of attack collision units, 0xC bytes (msw::CItem embeds one at
    // 0x170). A DOL class (vtable lbl_8046EA5C, only the destructor). Every
    // Set*All applies one value to all units; the per-unit functions ignore
    // an index out of range. Names without a symbol are guesses.
    class CAtkCollision {
        public:
            // Added for the DOL's msw::CStgGmk_WaterPillar_Act.
            CAtkCollision();
            CAtkCollision(long nNum, long kind, long type, unsigned long arg);
            virtual ~CAtkCollision();
            // Frees the units.
            void Destroy();
            void SetCollKind(long index, long kind);
            void SetCollTypeAll(long type);
            bool IsEnable(long index) const;
            void SetDisable(long index);
            void SetDisableAll();
            void SetPos(long index, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2);
            void SetSize(long index, float x, float y, float z);
            void ResetTargetFlag(long index, unsigned long nFlag);
            void ResetTargetFlagAll(unsigned long nFlag);
            // (name guessed) The byte at 0x15 of every unit.
            void SetUnk15All(long n);
            void SetTeamNoAll(long nTeamNo);
            void SetTeamIdx(long index, long nTeam);
            void SetTeamIdxAll(long nTeam);
            void SetChrIdx(long index, long nChr);
            void SetChrIdxAll(long nChr);
            void SetMyNoHitFlagAll(unsigned long nFlag);
            void SetNoHitFlag(long index, unsigned long nFlag);
            void SetTmpNoHitFlag(long index, unsigned long nFlag);
            void SetTmpNoHitFlagAll(unsigned long nFlag);
            // (names guessed) The direction of unit `index` in radians,
            // stored wrapped into [0, 2 pi); setting it also sets the bit
            // IsAtkHit reads, ResetAtkHit clears it.
            void SetAtkRotY(long index, float rot);
            void ResetAtkHit(long index);
            bool IsAtkHit(long index) const;
            void SetHitEffectID(long index, long nEffect);
            void SetHitSeID(long index, long nSe);
            // (names guessed) The float at 0x98 and the shorts at 0x9C and
            // 0x9E of every unit.
            void SetUnk98All(float f);
            void SetUnk9CAll(long n9C, long n9E);

            // The radius of unit `index`.
            float GetRadius(long index) const;
            // (name guessed; the DOL's fn_80109170) The direction SetAtkRotY
            // stored (the float at 0xA0 of the 0xAC-byte unit); 0 for an
            // index out of range.
            float GetAtkRotY(long index) const;

            // Sets the attack power of every unit.
            void SetAtkPowerAll(float power);

            // Called when a unit hits a body; pArg is what was registered.
            typedef CCollAtkUnit::AttackHitCallback AttackHitCallback;
            void SetAttackHitCallback(long index, AttackHitCallback pFunc, void *pArg);
            void SetAttackHitCallbackAll(AttackHitCallback pFunc, void *pArg);

            // (added for BasketF CChrCoreBasketKuroma; parameter meanings
            // guessed) Makes nNum units; sets on every unit the attack type,
            // the target flags, the flags of what it does not hit, the
            // radius, the position, the effect played on a hit and the
            // owner's index; turns every unit on.
            void Create(long nNum, long, long, unsigned long);
            void SetAtkTypeAll(unsigned long nType);
            void SetTargetFlagAll(unsigned long nFlag);
            void SetNoHitFlagAll(unsigned long nFlag);
            void SetRadiusAll(float fRadius);
            void SetPosAll(const bcs::FVEC3 &rPos);
            void SetHitEffectIDAll(long nEffect);
            void SetIndexAll(long nIndex);
            void SetEnableAll();
            // (added for game/basket/CChrCoreBasketDonkey.cpp; meanings
            // guessed) The size of every unit, and the frames of the effect
            // played on a hit.
            void SetSizeAll(float, float, float);
            void SetHitEffectFrameAll(float fFrame);

            // Per unit (added for game/msw/CStgGmk_Trolley.cpp; DOL
            // functions, argument meanings guessed). SetPos2 is the other
            // end of a capsule.
            void SetCollType(long index, long type);
            void SetTargetFlag(long index, unsigned long nFlag);
            void SetAtkType(long index, unsigned long nType);
            void SetEnable(long index);
            void SetRadius(long index, float fRadius);
            void SetPos(long index, const bcs::FVEC3 &rPos);
            void SetPos2(long index, const bcs::FVEC3 &rPos);
            // (DOL fn_8010934C, next to SetHitEffectFrameAll; name guessed;
            // added for the DOL's pinball ball)
            void SetHitEffectFrame(long index, float fFrame);
            void SetSize(long index, const bcs::FVEC3 &rSize);

            // (added for BasketF's CcSiromaMagicCircle; meanings guessed)
            // The camera shake and the sound of a hit; both ends of every
            // unit's capsule; unit `index`.
            void SetHitCamVibIDAll(long nId);
            void SetHitSeIDAll(long nSe);
            void SetPosAll(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2);
            CCollUnit *GetColl(long index) const;
            // (added for BasketF game/basket/CcWarioSmokeMgr.cpp; DOL) The
            // size of every unit.
            void SetSizeAll(const bcs::FVEC3 &rSize);

        private:
            // CCollUnit::SetPos, which the original inlines here (one file
            // with the collision units, see CDefCollision).
            static inline void SetUnitPos_(CCollAtkUnit &rUnit, const bcs::FVEC3 &rPos);
            static inline void SetUnitPos_(CCollAtkUnit &rUnit, const bcs::FVEC3 &rPos, const bcs::FVEC3 &rPos2);
            // CCollAtkUnit's setters of the hit callback and of the shorts
            // at 0x9C, likewise.
            static inline void SetUnitAttackHitCallback_(CCollAtkUnit &rUnit, AttackHitCallback pFunc, void *pArg);
            static inline void SetUnitUnk9C_(CCollAtkUnit &rUnit, long n9C, long n9E);

            // 0x00: vtable
            CCollAtkUnit *m_pColl;   // 0x04, array of m_nNum
            unsigned long m_nNum;    // 0x08
    };
}

#endif /* !MSW_CATKCOLLISION_HPP_ */
