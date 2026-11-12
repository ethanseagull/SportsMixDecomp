#ifndef MSW_CSTAGEMAN_HPP_
#define MSW_CSTAGEMAN_HPP_

#include "bcs/CListNodeBase.hpp"
#include "bcs/CPool.hpp"
#include "bcs/CUnivTask.hpp"
#include "bcs/FVEC3.hpp"
#include "game/msw/CStgSeSlot.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    struct BCS_Color;
    class CRegex;
    class CScnObj;
}

// The goal of a basket court (BasketF's msw::CGoalObjBasket derives from it):
// what GetGoal returns. A class in the global namespace.
class CGoalObj;

namespace msw {

    class CArn;
    class CStageMdl;
    class CStageMdlListNode;
    class CStageNet;
    class CStageScroll;

    namespace STAGE {
        struct MDL_GEN_RULE;
    }

    // (names guessed) The camera data of the stage's scenes, the member of
    // CStageMan at 0x8C. Size 0x30, vptr at 0x2C. Only the slot that
    // CStageMan::SceneCameraLoad calls (vtable offset 0x1C) is known; the
    // ones before it are placeholders that keep it there.
    class CStageSceneCamera {
        public:
            CStageSceneCamera();

        private:
            unsigned char m_Unk00[0x2C]; // 0x00

        public:
            virtual void Unk08();
            virtual void Unk0C();
            virtual void Unk10();
            virtual void Unk14();
            virtual void Unk18(); // called by the constructor
            virtual void Load(long, long);
    };

    // (names guessed) The stage's environment data, made by
    // CStageMan::Initialize_. Size 0x30.
    struct STAGE_ENV_COLOR {
        unsigned long m_Unk00;            // 0x00
        unsigned long m_LightMapOnColor;  // 0x04
        unsigned long m_LightMapOffColor; // 0x08
    };

    struct CStageEnv {
        void *m_Unk00;                   // 0x00
        void *m_Unk04;                   // 0x04
        const STAGE_ENV_COLOR *m_pColor; // 0x08
        unsigned char m_Unk0C[0x24];     // 0x0C
    };

    // The stage of a game: court dimensions, goals, net, models, fog,
    // gimmicks and the stage's sounds. A DOL class, size 0x1FC:
    // bcs::CUnivTask at 0x00, CStgSeSlot at 0x34 (vptr at 0x3C), own members
    // from 0x40. Each game module derives its stages from it.
    //
    // The order of the virtual functions is the order of the slots in
    // __vt__Q23msw13CStageManBoss (BossF). Bodies given here are the ones
    // the modules emit as weak functions; the functions without a body are
    // DOL functions. Return types are not part of the mangled names: those
    // of the functions with a body are read from the body, the ones marked
    // "guessed" and those of the DOL functions are guesses (void where
    // nothing better was known).
    class CStageMan : public bcs::CUnivTask, public CStgSeSlot {
        public:
            enum FLOW_TYPE {
                FLOW_TYPE_0
            };

            enum GOAL_SE_TYPE {
                GOAL_SE_TYPE_0
            };

            enum GOAL_IN_TYPE {
                GOAL_IN_TYPE_0
            };

            enum LDATA_TYPE {
                LDATA_TYPE_0
            };

            // Values of m_nGimmickStep.
            enum {
                GIMMICK_STOP,
                GIMMICK_RUNNING,
                GIMMICK_PAUSE
            };

            class CMdlPtrPoolNode;
            class CMdlGroup; // (name guessed) 0x28 bytes, see GetMdlGroup

            typedef bcs::CPool<CStageMdl *, CMdlPtrPoolNode, bcs::pool::NoLock> CMdlPtrPool;

            CStageMan(bcs::CTask *pParent);
            virtual ~CStageMan();
            virtual void Main();
            virtual void Disp();

            virtual void Disp3D(long) {}
            virtual long GetArrangeGroupNum() const { return m_nArrangeGroupNum; }
            virtual void SceneEntry();
            virtual void SceneReentry();
            virtual void SceneEntry(long scene);
            virtual void SceneLeave(long scene);
            virtual void SceneLeaveAll();
            virtual int IsSceneEntry() const { return m_bSceneEntry; }
            virtual unsigned long GetSyncCheckSum() const { return CalcSum(); }

            // The edges of an area; a null pointer skips that edge.
            virtual void GetCourtRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetCourtLeft();
                }
                if (pRight != 0) {
                    *pRight = GetCourtRight();
                }
                if (pFront != 0) {
                    *pFront = GetCourtFront();
                }
                if (pBack != 0) {
                    *pBack = GetCourtBack();
                }
            }
            virtual float GetCourtLeft() const;
            virtual float GetCourtRight() const;
            virtual float GetCourtFront() const;
            virtual float GetCourtBack() const;
            virtual float GetCourtRadius() const;
            virtual float GetCourtCenter() const { return 0.0f; }
            virtual void GetTeamCourtRect(long team, float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetTeamCourtLeft(team);
                }
                if (pRight != 0) {
                    *pRight = GetTeamCourtRight(team);
                }
                if (pFront != 0) {
                    *pFront = GetTeamCourtFront(team);
                }
                if (pBack != 0) {
                    *pBack = GetTeamCourtBack(team);
                }
            }
            virtual float GetTeamCourtLeft(long team) const;
            virtual float GetTeamCourtRight(long team) const;
            virtual float GetTeamCourtFront(long team) const;
            virtual float GetTeamCourtBack(long team) const;
            virtual void GetTeamCourtFrontZoneRect(long team, float *pLeft, float *pRight, float *pFront,
                                                   float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetTeamCourtFrontZoneLeft(team);
                }
                if (pRight != 0) {
                    *pRight = GetTeamCourtFrontZoneRight(team);
                }
                if (pFront != 0) {
                    *pFront = GetTeamCourtFrontZoneFront(team);
                }
                if (pBack != 0) {
                    *pBack = GetTeamCourtFrontZoneBack(team);
                }
            }
            virtual float GetTeamCourtFrontZoneLeft(long team) const { return GetTeamCourtLeft(team); }
            virtual float GetTeamCourtFrontZoneRight(long team) const { return GetTeamCourtRight(team); }
            virtual float GetTeamCourtFrontZoneFront(long team) const;
            virtual float GetTeamCourtFrontZoneBack(long team) const;
            virtual void GetCourtInnerAreaRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetCourtInnerAreaLeft();
                }
                if (pRight != 0) {
                    *pRight = GetCourtInnerAreaRight();
                }
                if (pFront != 0) {
                    *pFront = GetCourtInnerAreaFront();
                }
                if (pBack != 0) {
                    *pBack = GetCourtInnerAreaBack();
                }
            }
            virtual float GetCourtInnerAreaLeft() const { return GetCourtLeft(); }
            virtual float GetCourtInnerAreaRight() const { return GetCourtRight(); }
            virtual float GetCourtInnerAreaFront() const { return GetCourtFront(); }
            virtual float GetCourtInnerAreaBack() const { return GetCourtBack(); }
            virtual void GetTeamCourtInnerAreaRect(long team, float *pLeft, float *pRight, float *pFront,
                                                   float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetTeamCourtInnerAreaLeft(team);
                }
                if (pRight != 0) {
                    *pRight = GetTeamCourtInnerAreaRight(team);
                }
                if (pFront != 0) {
                    *pFront = GetTeamCourtInnerAreaFront(team);
                }
                if (pBack != 0) {
                    *pBack = GetTeamCourtInnerAreaBack(team);
                }
            }
            virtual float GetTeamCourtInnerAreaLeft(long team) const { return GetTeamCourtLeft(team); }
            virtual float GetTeamCourtInnerAreaRight(long team) const { return GetTeamCourtRight(team); }
            virtual float GetTeamCourtInnerAreaFront(long team) const { return GetTeamCourtFront(team); }
            virtual float GetTeamCourtInnerAreaBack(long team) const { return GetTeamCourtBack(team); }
            virtual void GetGroundRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetGroundLeft();
                }
                if (pRight != 0) {
                    *pRight = GetGroundRight();
                }
                if (pFront != 0) {
                    *pFront = GetGroundFront();
                }
                if (pBack != 0) {
                    *pBack = GetGroundBack();
                }
            }
            virtual float GetGroundLeft() const;
            virtual float GetGroundRight() const;
            virtual float GetGroundFront() const;
            virtual float GetGroundBack() const;
            virtual void GetTeamGroundRect(long team, float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetTeamGroundLeft(team);
                }
                if (pRight != 0) {
                    *pRight = GetTeamGroundRight(team);
                }
                if (pFront != 0) {
                    *pFront = GetTeamGroundFront(team);
                }
                if (pBack != 0) {
                    *pBack = GetTeamGroundBack(team);
                }
            }
            virtual float GetTeamGroundLeft(long team) const;
            virtual float GetTeamGroundRight(long team) const;
            virtual float GetTeamGroundFront(long team) const;
            virtual float GetTeamGroundBack(long team) const;
            virtual void GetOuterFieldRect(float *pLeft, float *pRight, float *pFront, float *pBack) const
            {
                if (pLeft != 0) {
                    *pLeft = GetOuterFieldLeft();
                }
                if (pRight != 0) {
                    *pRight = GetOuterFieldRight();
                }
                if (pFront != 0) {
                    *pFront = GetOuterFieldFront();
                }
                if (pBack != 0) {
                    *pBack = GetOuterFieldBack();
                }
            }
            virtual float GetOuterFieldLeft() const;
            virtual float GetOuterFieldRight() const;
            virtual float GetOuterFieldFront() const;
            virtual float GetOuterFieldBack() const;
            virtual float GetOuterFieldRadius() const;
            virtual void GetTeamOuterFieldRect(long team, float *pLeft, float *pRight, float *pFront, float *pBack,
                                               long n) const
            {
                if (pLeft != 0) {
                    *pLeft = GetTeamOuterFieldLeft(team, n);
                }
                if (pRight != 0) {
                    *pRight = GetTeamOuterFieldRight(team, n);
                }
                if (pFront != 0) {
                    *pFront = GetTeamOuterFieldFront(team, n);
                }
                if (pBack != 0) {
                    *pBack = GetTeamOuterFieldBack(team, n);
                }
            }
            virtual float GetTeamOuterFieldLeft(long team, long n) const;
            virtual float GetTeamOuterFieldRight(long team, long n) const;
            virtual float GetTeamOuterFieldFront(long team, long n) const;
            virtual float GetTeamOuterFieldBack(long team, long n) const;
            virtual long GetTeamIdxByPos(const bcs::FVEC3 &rPos) const;
            virtual unsigned long GetAreaFlagByPos(const bcs::FVEC3 &rPos, long) const;
            virtual long GetFloorAttribute(const bcs::FVEC3 &rPos, FLOW_TYPE type) const;
            virtual void GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &, FLOW_TYPE) const
            {
                rFlow.Set(0.0f, 0.0f, 0.0f);
            }
            virtual float GetFloorFlowRot(const bcs::FVEC3 &, FLOW_TYPE) const { return 0.0f; }
            virtual float GetFloorFlowRate(const bcs::FVEC3 &, FLOW_TYPE) const { return 0.0f; }
            virtual void CalcFloorFlowPos(bcs::FVEC3 &rOut, const bcs::FVEC3 &rPos, FLOW_TYPE) const { rOut = rPos; }
            virtual float CalcFloorFlowRot(const bcs::FVEC3 &, FLOW_TYPE) const { return 0.0f; }
            virtual long GetHitMaterial(const bcs::FVEC3 &rPos) const;
            virtual float GetTeamCourtOffsetX(long team) const { return m_aTeamCourtOffset[team].x; }
            virtual float GetTeamCourtOffsetZ(long team) const { return m_aTeamCourtOffset[team].z; }
            virtual bcs::FVEC3 GetTeamCourtOffset(long team) const { return m_aTeamCourtOffset[team]; }
            virtual CGoalObj *GetGoal(long) { return 0; }
            virtual const CGoalObj *GetGoal(long) const { return 0; }
            virtual const bcs::FVEC3 &GetGoalPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalRot(long goal) const;
            virtual const bcs::FVEC3 &GetBoardPos(long goal) const;
            virtual const bcs::FVEC3 &GetGoalVec(long goal) const;
            virtual float GetGoalWidth(long) const { return 0.0f; }
            virtual float GetGoalX1Length(long) const { return 0.0f; }
            virtual float GetGoalRRRadius(long) const { return 0.0f; }
            virtual float GetGoalDepth(long) const { return 0.0f; }
            virtual long GetGoalSE(GOAL_SE_TYPE) const { return -1; }
            virtual const bcs::FVEC3 &Get3PointAreaPos(long goal) const { return GetGoalPos(goal); }
            // (return type guessed: zero)
            virtual bool Is3PointArea(long, const bcs::FVEC3 &, float) const { return false; }
            // The by-value return goes through bcs::FVEC3's copy constructor:
            // three float copies, as in the original.
            virtual bcs::FVEC3 GetGoalAssumedPos(long goal, float) const { return GetGoalPos(goal); }
            virtual void GoalCrush(long goal);
            virtual void GoalRepairAll();
            virtual void GoalIn(long goal);
            virtual float GetNetWidth() const { return GetBaseNetWidth_(); }
            virtual float GetNetHeightTop() const { return GetBaseNetHeightTop_(); }
            virtual float GetNetHeightBottom() const { return GetBaseNetHeightBottom_(); }
            virtual const bcs::FVEC3 &GetNetPos() const { return m_vZero; }
            virtual void SetNetAnm(const bcs::FVEC3 &, const bcs::FVEC3 &);
            virtual void ResetNetAnm();
            // Whether every panel bit of `panel` is enabled.
            virtual bool IsQPanelEnabled(long panel) const { return GetQPanelEnabled(panel) == panel; }
            virtual long GetQPanelEnabled(long panel) const { return panel & GetQPanelEnabled_(); }
            // (return types of the next eight guessed: the first five return
            // zero, the last three return one)
            virtual long GetPanelType(const bcs::FVEC3 &) const { return 0; }
            virtual long HitPanel(const bcs::FVEC3 &, int) { return 0; }
            virtual long GetPanelBonusType() const { return 0; }
            virtual long GetPanelBonusRate() const { return 0; }
            virtual bool GetPassToPos(bcs::FVEC3 *, long, long) const { return false; }
            virtual bool IsNoBorder() const { return true; }
            virtual bool CanbeComeback() const { return true; }
            virtual bool CanbeJump(const bcs::FVEC3 &) const { return true; }
            virtual void SendMessage(long, long, long, long);
            virtual void EffectStart(long, long);
            virtual void EffectStartPart(long, long, long);
            virtual void EffectStop(long, long);
            virtual void EffectErase(long, long);
            virtual CStageScroll *GetScroll() { return 0; }
            virtual const CStageScroll *GetScroll() const { return 0; }
            virtual float GetWorldRotX() const { return 0.0f; }
            virtual float GetWorldRotY() const { return 0.0f; }
            virtual float GetWorldRotZ() const { return 0.0f; }
            virtual void SetBaseColor(const bcs::BCS_Color &rColor);
            virtual void SetColorScale(const bcs::BCS_Color &rColor);
            virtual void FogBlendIn(long fog, long, float frame, int);
            virtual void FogBlendOut(long fog, long, float frame, int);
            virtual void FogBlendStart(long fog, long, float, float);
            virtual void FogBlendIn(long fog, float frame, int n) { FogBlendIn(fog, 0, frame, n); }
            virtual void FogBlendOut(long fog, float frame, int n) { FogBlendOut(fog, 0, frame, n); }
            virtual void FogBlendStart(long fog, float f1, float f2) { FogBlendStart(fog, 0, f1, f2); }
            virtual void FogBlendPause();
            virtual void FogBlendResume();
            virtual void FogBlendCancel();
            virtual void FogBlendReset();
            virtual void FogBlendUpdate(float);
            virtual void FogBlendPauseOne(long fog);
            virtual void FogBlendResumeOne(long fog);
            virtual void FogBlendCancelOne(long fog);
            virtual void FogBlendResetOne(long fog);
            virtual void FogBlendUpdateOne(long fog, float);
            // Returned through a hidden pointer (read from the DOL body).
            virtual bcs::BCS_Color QueryBgFadeColor_() const;
            virtual void Main_(float);
            virtual long GetCurLoadDataIdx_(LDATA_TYPE type) const;
            virtual void CreateMdl(nw4r::g3d::ResFile, CArn &, long);
            virtual void DoCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &, long);
            virtual CStageMdl *NewMdl_(CStageMan *, long, bcs::CRegex &);
            virtual void AddMdl(CStageMdl *);
            virtual void GroupAddMdl(long group, CMdlPtrPool::Handle &);
            virtual CMdlGroup *GetMdlGroup(long group);
            virtual const CMdlGroup *GetMdlGroup(long group) const;
            virtual unsigned long CalcSum() const;
            virtual void SetDefaultNodeTag(unsigned long tag) { m_nDefaultNodeTag = tag; }
            virtual unsigned long GetDefaultNodeTag() const { return m_nDefaultNodeTag; }
            virtual void SetDispByTag(long, int, unsigned long, unsigned long);
            virtual const STAGE::MDL_GEN_RULE *GetMdlGenRule_() const;
            virtual const STAGE::MDL_GEN_RULE *FindMdlGenRule_(bcs::CRegex &, const char *) const;
            // GetBaseCourtWidth_ and GetBaseCourtDepth_ have bodies in the
            // DOL but no exported name: every module's stage overrides them.
            virtual float GetBaseCourtWidth_() const;
            virtual float GetBaseCourtDepth_() const;
            virtual float GetBaseCourtFrontZoneDepth_() const;
            // Inline: the stages of BasketF and VolleyF, which do not
            // override it, emit it.
            virtual float GetBaseCourtRadius_() const { return 0.0f; }
            virtual float GetBaseGroundWidth_() const;
            virtual float GetBaseGroundDepth_() const;
            virtual float GetBaseOuterWidth_() const { return 0.0f; }
            virtual float GetBaseOuterDepth_() const { return 0.0f; }
            virtual float GetBaseOuterFront_() const { return 0.0f; }
            virtual float GetBaseOuterRadius_() const { return 0.0f; }
            virtual float GetBaseNetWidth_() const { return 0.0f; }
            virtual float GetBaseNetHeightTop_() const { return 0.0f; }
            virtual float GetBaseNetHeightBottom_() const { return 0.0f; }
            virtual void SetTeamCourtOffsetX(long team, float x) { m_aTeamCourtOffset[team].x = x; }
            virtual void SetTeamCourtOffsetZ(long team, float z) { m_aTeamCourtOffset[team].z = z; }
            virtual void SetTeamCourtOffset(long team, const bcs::FVEC3 &rOffset)
            {
                m_aTeamCourtOffset[team] = rOffset;
            }
            virtual void SetTeamCourtOffset(long team, float x, float z)
            {
                SetTeamCourtOffsetX(team, x);
                SetTeamCourtOffsetZ(team, z);
            }
            virtual CStageNet *GetNet() { return 0; }
            virtual const CStageNet *GetNet() const { return 0; }
            // One bit per panel that GetQPanelEnabled can report.
            virtual long GetQPanelEnabled_() const { return 0xFFFF; }
            // (return type of the next two guessed: one word)
            virtual unsigned long GetLightMapOnColor() const { return m_pEnv->m_pColor->m_LightMapOnColor; }
            virtual unsigned long GetLightMapOffColor() const { return m_pEnv->m_pColor->m_LightMapOffColor; }
            virtual void SceneCameraLoad(long scene, long n) { m_SceneCamera.Load(scene, n); }
            virtual long SE_GetSlotNum() const { return 4; }
            virtual void PlayAnim_AudienceFlash(long, float);
            virtual void UpdateAnim_AudienceFlash(float);
            virtual void InitAnim_AudienceFlag();

            // Hooks for the derived stages. The two OnPreCreate* say whether
            // to go on and create the thing.
            virtual bool OnPreCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &, long)
            {
                return true;
            }
            virtual void OnPostCreateMdl(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, CMdlPtrPool::Handle &, CArn &,
                                         long)
            {
            }
            virtual bool OnPreCreateObj(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, nw4r::g3d::ResFile, CArn &, long)
            {
                return true;
            }
            virtual void OnPostCreateObj(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, bcs::CScnObj *,
                                         nw4r::g3d::ResFile, CArn &, long)
            {
            }
            virtual void OnPostAppendNode(const STAGE::MDL_GEN_RULE *, bcs::CRegex &, CStageMdlListNode *,
                                          nw4r::g3d::ResFile &, CArn &, long)
            {
            }
            virtual void OnSceneEntry() {}
            virtual void OnPreSceneEntry(long) {}
            virtual void OnPostSceneEntry(long) {}
            virtual void OnPreSceneLeave(long) {}
            virtual void OnPostSceneLeave(long) {}
            virtual void OnSceneReentry() {}
            virtual void OnPreInitialize() {}
            virtual void OnPostInitialize() {}
            virtual void OnGameStart() {}
            virtual void OnGameEnd() {}
            virtual void OnGoalIn(long, GOAL_IN_TYPE) {}
            virtual void OnGameInit() {}
            virtual void OnPeriodInit() {}
            virtual void OnTurnInit() {}
            virtual void OnTurnStart() {}
            virtual void OnTurnEnd() {}
            virtual void OnEventStart() {}
            virtual void OnEventEnd() {}
            virtual void OnEntranceEventStart() {}
            virtual void OnEntranceEventEnd() {}
            virtual void OnVictoryEventStart() {}
            virtual void OnVictoryEventEnd() {}
            virtual void OnChangeCourtEventStart() {}
            virtual void OnChangeCourtEventEnd() {}
            virtual void OnArtsEventStart() {}
            virtual void OnArtsEventEnd() {}
            virtual void OnArtsShootStart() {}
            virtual void OnArtsShootEnd() {}
            virtual void OnActiveCameraChange(long, long) {}
            // (return type guessed: zero; CalcSum's contribution of the derived stage)
            virtual unsigned long OnCalcSum() const { return 0; }
            virtual void OnGimmickReset() {}
            virtual void OnGimmickStart() {}
            virtual void OnGimmickStop() {}
            virtual void OnGimmickCancel() {}
            virtual void OnGimmickPause() {}
            virtual void OnGimmickResume() {}
            virtual bool IsGimmickStop() const { return m_nGimmickStep == GIMMICK_STOP; }
            virtual bool IsGimmickRunning() const { return m_nGimmickStep == GIMMICK_RUNNING; }
            virtual bool IsGimmickPause() const { return m_nGimmickStep == GIMMICK_PAUSE; }
            virtual long GetGimmickStep() const { return m_nGimmickStep; }
            // (return type guessed: a null pointer here)
            virtual const void *GetPrmTbl() const { return 0; }
            virtual long GetEntranceEventGmkStartCamNo() const;
            virtual long GetEntranceEventGmkEndCamNo() const;
            virtual void OnEntranceEventChgCamera(long) {}
            virtual void OnEntranceEventGmkStart() {}
            virtual void OnEntranceEventGmkEnd() {}
            virtual void SwitchDemoMdlAll(int);

            void SetStageNo(long stageNo);
            // (name guessed) Inline in the modules.
            long GetStageNo() const { return m_nStageNo; }
            // Loads and builds the stage chosen with SetStageNo.
            void Initialize_();
            // (added for MenuF game/menu/CSelectMenu.cpp; inline, emitted
            // there; name from the symbol)
            void Init() { Initialize_(); }

            static bool IsValidInstance();
            static CStageMan *GetInstance();
            // The default model factory of an MDL_GEN_RULE: the stage
            // instance's NewMdl_(pStage, n, rRegex).
            static CStageMdl *NewMdl(CStageMan *pStage, long n, bcs::CRegex &rRegex);

            static bcs::FVEC3 m_vZero;

        protected:
            // (names guessed) The number of models in the model pool, and
            // the model of its idx-th node in use, or null. Inline; HockeyF's
            // CStageHockey05::Main_ walks the models with them.
            long GetMdlNum() { return GetMdlUsedList().GetCount(); }
            CStageMdl *GetMdlByIdx(long idx);

        private:
            // The list of nodes in use of m_MdlPtrPool (bcs::CPool::m_UsedList,
            // at 0x34 in the pool).
            bcs::CListBase &GetMdlUsedList() { return *reinterpret_cast<bcs::CListBase *>(m_MdlPtrPool + 0x34); }

            unsigned char m_Unk40[0x10];        // 0x40, an object the constructor builds
            long m_nStageNo;                    // 0x50
            unsigned char m_Unk54[0x28];        // 0x54, a pool or list
            unsigned long m_Unk7C[4];           // 0x7C
            CStageSceneCamera m_SceneCamera;    // 0x8C
            unsigned long m_UnkBC;              // 0xBC
            bcs::FVEC3 m_aTeamCourtOffset[2];   // 0xC0
            CStageEnv *m_pEnv;                  // 0xD8
            long m_UnkDC;                       // 0xDC
            long m_UnkE0;                       // 0xE0
            int m_bSceneEntry;                  // 0xE4
            unsigned long m_nDefaultNodeTag;    // 0xE8
            unsigned char m_MdlPtrPool[0x60];   // 0xEC, the CMdlPtrPool (0x400 nodes of 0x2C bytes)
            long m_nArrangeGroupNum;            // 0x14C
            CMdlGroup *m_pMdlGroup;             // 0x150, m_nArrangeGroupNum groups
            unsigned long m_Unk154;             // 0x154
            long m_nGimmickStep;                // 0x158
            unsigned char m_Unk15C[0xA0];       // 0x15C
    };

    // A node of CStageMan's model pool, 0x2C bytes (see m_MdlPtrPool). Only
    // what bcs::CPool::Handle reads is mapped (BasketF CStageBasket13).
    class CStageMan::CMdlPtrPoolNode {
        public:
            unsigned char m_Unk00[0x10]; // 0x00
            unsigned long m_nHandleId;   // 0x10, the id of the handle that refers to this node
            CStageMdl *m_Data;           // 0x14
            unsigned char m_Unk18[0x14]; // 0x18
    };

    inline CStageMdl *CStageMan::GetMdlByIdx(long idx)
    {
        bcs::CListBase &rList = GetMdlUsedList();
        if (idx < 0 || idx >= GetMdlNum()) {
            return 0;
        }
        long i = 0;
        for (bcs::CListNodeBase *pNode = rList.GetFirst(); pNode != rList.GetEnd(); bcs::CListNodeBase::Step(pNode)) {
            if (i == idx) {
                return reinterpret_cast<CMdlPtrPoolNode *>(pNode)->m_Data;
            }
            i++;
        }
        return 0;
    }

    namespace STAGE {

        // (member names guessed) One entry of a stage's model table, size
        // 0x14: models whose node name matches the regular expression
        // m_pPattern are made by m_pNewMdl. A table ends with an entry whose
        // pattern is null; CStageMan::FindMdlGenRule_ then reads the second
        // word as the next table to search (the base class's table, which a
        // derived stage stores there the first time it is asked for its own),
        // or stops when it is null.
        struct MDL_GEN_RULE {
            const char *m_pPattern;                                      // 0x00
            long m_nId;                                                  // 0x04, the next table in an end entry
            long m_Unk08;                                                // 0x08 (1 for the stage 02 splash models)
            unsigned long m_Unk0C;                                       // 0x0C
            CStageMdl *(*m_pNewMdl)(CStageMan *, long, bcs::CRegex &); // 0x10
        };
    }
}

namespace msw {

    // (layout guessed from the loops of CStageMan::SetDispByTag and HockeyF
    // CStageHockey01::SwitchAudienceMdl_) A group of the stage's models, 0x28
    // bytes: a list whose nodes carry a CStageMdl at 0x10.
    class CStageMan::CMdlGroup {
        public:
            class CNode : public bcs::CListNodeBase {
                public:
                    CStageMdl *GetMdl() const { return m_pMdl; }

                private:
                    CStageMdl *m_pMdl; // 0x10
            };

            bcs::CListBase &GetList() { return m_List; }

        private:
            bcs::CListBase m_List;      // 0x00, 0x28 bytes
    };
}

#endif /* !MSW_CSTAGEMAN_HPP_ */
