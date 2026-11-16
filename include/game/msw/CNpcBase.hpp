#ifndef MSW_CNPCBASE_HPP_
#define MSW_CNPCBASE_HPP_

#include "bcs/CAnmObjMotBase.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"

namespace bcs {
    class CAnmMotion;
    class CMdlObj;
    class CMdlRef;
    class CMdlShadow;
}

namespace msw {

    // INCOMPLETE: a non-player character on the court (the boss Packun,
    // ...). A DOL class and a task (its constructor
    // __ct__Q23msw8CNpcBaseFPQ23msw7CChrManl calls bcs::CTask's). Size
    // 0x188: the constructor's last store is to 0x184, and
    // msw::CNpcBossPackun's state queue starts at 0x188. Only part of its
    // virtual functions (below) and of its members are declared, so it must
    // not be instantiated or derived from with a vtable emitted yet.
    class CAtkCollision;
    class CBall;
    class CBodyCollision;
    class CChrMan;
    class CCollAtkUnit;
    class CCollBodyUnit;

    // The base is bcs::CUnivTask (same layout as bcs::CTask): the derived
    // vtables hold bcs::CUnivTask::GetFrameRateF at 0x14
    // (__vt__Q23msw19CNpcGhostBasketBase, BasketF).
    class CNpcBase : public bcs::CUnivTask {
        public:
            // (DOL; added for BossF game/boss/CNpcBehemothFiga.cpp) nType is
            // the kind of NPC (CNpcBehemothBoss passes 0x13).
            CNpcBase(CChrMan *pMan, long nType);
            virtual ~CNpcBase();

            // Virtual functions 0x18 to 0x50, in the order and with the names
            // of the slots of __vt__Q23msw16CNpcStringBasket (BasketF), for
            // BasketF's CBallBasketFruit, which calls CanbeCatch. Return
            // types guessed. (the class's destructor and later slots are not
            // declared yet)
            // DOL; overrides of bcs::CTask's (slots 0x0C and 0x10 of the
            // BossF vtables of the classes derived from it).
            virtual void Main();
            virtual void Disp();

            virtual void Create();                                     // 0x18
            virtual long GetNpcType() const;                           // 0x1C
            virtual void Reset();                                      // 0x20
            virtual long SendMessage(long, long, long, long, long);    // 0x24
            // (inline: VolleyF's game/volley/CNpcGhostVolley.cpp and the
            // other modules' NPC files each have a weak copy returning 0)
            virtual long GetThinkLevel() const { return 0; }           // 0x28
            virtual void AnimPlay(long, int, float, float);            // 0x2C
            virtual void AnimStop();                                   // 0x30
            virtual void AnimSetSpeed(float);                          // 0x34
            virtual void AnimSetFrame(float);                          // 0x38
            virtual void AttachBall(CBall *);                          // 0x3C
            virtual void AttachBall(CBall *, long, float, unsigned long); // 0x40
            virtual void DetachBall();                                 // 0x44
            // (inline: BossF CNpcBehemothFiga.cpp and the BasketF, DodgeF,
            // HockeyF and VolleyF files of the NPCs that keep them carry weak
            // copies)
            virtual int IsHaveBall() const { return 0; }               // 0x48
            virtual int IsHoldBall() const { return IsHaveBall(); }    // 0x4C
            virtual int CanbeCatch() const { return 0; }               // 0x50
            // 0x54 to 0x70 (added for BasketF game/basket/CNpcGhostBasketBase.cpp;
            // names from __vt__Q23msw19CNpcGhostBasketBase, return types
            // guessed except where noted).
            // (returns the body's radius: BossF's CComDrvBoss adds it to a
            // character's size; the two points are the body capsule's ends)
            virtual float GetBodyCollInfo(bcs::FVEC3 &, bcs::FVEC3 &);    // 0x54
            virtual void MainFunc();                                    // 0x58
            // (inline: BasketF CBallManBasketHungryBossPackun.cpp, BossF
            // CNpcBehemothFiga.cpp and DodgeF CGameDodgeDodgeOrDie.cpp each
            // have a weak copy, an empty body)
            virtual void DispFunc() {}                                  // 0x5C
            virtual void CallbackMotTrg_(const bcs::CAnmObjMotBase::MOT_TRG *); // 0x60
            // (the ghost's override returns 1 after taking the hit, 0 when it
            // ignores it)
            virtual unsigned long CallbackBodyDamage_(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                      const bcs::FVEC3 &rPos); // 0x64
            virtual void CallbackBodyPush_(CCollBodyUnit *pBody, CCollBodyUnit *pOther, bcs::FVEC3 *pPush); // 0x68
            virtual void CallbackBallCatch_(CCollBodyUnit *pBody, CBall *pBall); // 0x6C
            virtual void CallbackAttackHit_(const CCollAtkUnit *, const CCollBodyUnit *, const bcs::FVEC3 &,
                                            unsigned long); // 0x70

            // Added for BasketF game/basket/CNpcGhostBasketBase.cpp (all in
            // the DOL; return types and parameter names guessed).
            long AnimGetMotID();
            float AnimGetFrame();
            float AnimGetFrameNum();
            int IsAnimEnd();
            int IsAnimLoop();
            // (DOL; added for VolleyF game/volley/CNpcGhostVolley.cpp) The
            // frame count of motion nMot of the model's resource.
            float AnimGetResFrameNum(long nMot);
            // (DOL; added for VolleyF game/volley/CNpcGhostVolley.cpp) Erases
            // every effect the NPC made.
            void Eff_EraseAll();
            // (DOL; added for VolleyF game/volley/CNpcGhostVolley.cpp, return
            // type guessed) Stored as the spike's outline parameter.
            long PlayGetOutline_(unsigned long) const;
            // Plays sound nId at pPos (null: at the NPC).
            void PlaySePos(long nId, const bcs::FVEC3 *pPos, unsigned long, long, float, float, float);
            // The ball the NPC carries; null when none.
            CBall *GetAttachBall() const;
            // Turns the yaw towards fRotY by fRate of the difference.
            void RotateYRate(float fRotY, float fRate);
            // The yaw (radians) from the NPC towards rPos, and from rPos
            // towards the NPC, in the XZ plane.
            float GetRotY_ToPosXZ(const bcs::FVEC3 &rPos) const;
            float GetRotY_FromPosXZ(const bcs::FVEC3 &rPos) const;
            void GetCompPos(bcs::FVEC3 &rPos) const;
            void SetScale(const bcs::FVEC3 &rScale);
            // (DOL; added for BasketF game/basket/CBallManBasketHungryBossPackun.cpp,
            // names from the symbols) The offset the model is drawn at from
            // the NPC's position.
            void SetOffsetPos(const bcs::FVEC3 &rOfs);
            bcs::FVEC3 &GetOffsetPos();

            // The functions registered with the body collision; pArg is the
            // NPC, and each calls the virtual function of the same name with
            // a trailing underscore.
            static void CallbackBodyPush(CCollBodyUnit *pBody, CCollBodyUnit *pOther, void *pArg, bcs::FVEC3 *pPush);
            static unsigned long CallbackBodyDamage(CCollBodyUnit *pBody, const CCollAtkUnit *pAtk,
                                                    const bcs::FVEC3 &rPos, void *pArg);
            static void CallbackBallCatch(CCollBodyUnit *pBody, CBall *pBall, void *pArg);
            // (DOL; added for BossF game/boss/CNpcBehemothFiga.cpp) The
            // motion trigger callback; pArg is the NPC (CallbackMotTrg_).
            static void CallbackMotTrg(const bcs::CAnmObjMotBase::MOT_TRG *pTrg, void *pArg);

            // (names guessed) Read by CStageBasketHungryBossPackun::Main_ in
            // BasketF to make the goal follow the boss.
            const bcs::FVEC3 &GetPos() const { return m_vPos; }
            const bcs::FVEC3 &GetRot() const { return m_vRot; }
            // (DOL; added for BossF game/boss/CChrCoreBoss.cpp) The index of
            // the model node called pName (negative when there is none), and
            // the world position of node nNode.
            long GetNodeID(const char *pName) const;
            void GetNodePos(long nNode, bcs::FVEC3 &rPos) const;
            // (names guessed) Inline in msw::CStgGmk_Trolley_Mdl (DOL),
            // which places its heyho; the yaw is brought into [0, 2 pi).
            void SetPos(const bcs::FVEC3 &rPos) { m_vPos = rPos; }
            void SetRotY(float rot)
            {
                while (rot > 6.2831855f) {
                    rot -= 6.2831855f;
                }
                while (rot < 0.0f) {
                    rot += 6.2831855f;
                }
                m_vRot.y = rot;
            }

            // (DOL; added for BossF game/boss/CNpcBehemothFiga.cpp, names from
            // the symbols, return types and parameter meanings guessed) The
            // attack collision with nNum units (m_pAtkColl) and the body
            // collision with nNum units (m_pBodyColl), and the settings of
            // attack unit nIdx.
            void CollAtkCreate(long nNum);
            void CollAtkSetAtkType(long nIdx, unsigned long nType);
            void CollAtkSetCollType(long nIdx, long nType);
            void CollAtkSetHitSe(long nIdx, long nSe);
            void CollBodyCreate(long nNum);
            // Effects in the NPC's effect slots, and its sounds.
            void Eff_CreateAttachAuto(long nSlot, long nEffect, unsigned long nFlag);
            void Eff_CreateSimple(long nEffect, const bcs::FVEC3 &rPos, unsigned long, unsigned long);
            void Eff_CreateSimple(long nEffect, const bcs::FVEC3 &rPos, float fScale, unsigned long, unsigned long);
            void Eff_Stop(long nSlot);
            void PlaySe(long nId, unsigned long, long, float, float, float);
            void SE_PlayPos(long nSlot, long nId, const bcs::FVEC3 *pPos, unsigned long, long, float, float, float);
            void SE_Stop(long nSlot, float fFadeFrames);
            void SE_StopAll(float fFadeFrames);

        protected:
            unsigned char m_Unk34[0x40 - 0x34];
            // 0x40 (name guessed; BossF CNpcBehemothBoss::UpdateLookAt_
            // scales its turn rate by it).
            float m_fUnk40;
            unsigned char m_Unk44[0x48 - 0x44];
            // 0x48 (name guessed): the model (what NewMdlObj_ made).
            bcs::CMdlObj *m_pMdl;
            // 0x4C, 0x50 (names guessed, BossF CNpcBehemothFiga.cpp): the
            // model's shadow and its reflection.
            bcs::CMdlShadow *m_pShadow;
            bcs::CMdlRef *m_pRef;
            // 0x54 (names guessed, BossF CNpcBehemothFiga.cpp): the model's
            // motion and the attack collision units.
            bcs::CAnmMotion *m_pAnmMot;
            CAtkCollision *m_pAtkColl;
            // 0x5C (name guessed): the body collision units.
            CBodyCollision *m_pBodyColl;
            bcs::FVEC3 m_vPos;   // 0x60
            bcs::FVEC3 m_vRot;   // 0x6C, written by RotateYRate
            bcs::FVEC3 m_vScale; // 0x78, written by SetScale
            unsigned char m_Unk84[0x188 - 0x84];
    };
}

#endif /* !MSW_CNPCBASE_HPP_ */
