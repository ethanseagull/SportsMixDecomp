#ifndef MSW_CSTGMDL_HPP_
#define MSW_CSTGMDL_HPP_

#include "bcs/CMdlRef.hpp"
#include "bcs/CMdlShadow.hpp"
#include "bcs/CScnObj.hpp"
#include "bcs/FMTX34.hpp"
#include "bcs/New.hpp"
#include "game/msw/CLoadData.hpp"
#include "game/msw/CStageAnm.hpp"
#include "revolution/mtx.h"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CMdlObj;
}

namespace msw {

    class CArn;
    class CStgObj;

    // A model of a stage object. A DOL class, size 0x12C, vptr at 0x128
    // (constructor __ct__Q23msw7CStgMdlFPQ23msw7CStgObj): the data members
    // come before the first virtual function.
    //
    // The order of the virtual functions is that of
    // __vt__Q23msw29CStgGmk_WaterWorks_Hockey_Mdl (HockeyF). Return types
    // and parameter names are guesses unless a caller shows them.
    class CStgMdl {
        public:
            // What a model is made from: the names of its model, shadow
            // model, reference model and animations in a resource file.
            // 0x20 bytes; the field names are guesses from the one table
            // read so far (HockeyF game/hockey/CStgGmk_WaterWorks_Hockey.cpp)
            // and from the creators in the DOL, which read the first three
            // names and the animation table.
            struct MDL_PARAM {
                const char *pMdlName;           // 0x00
                const char *pShadowName;        // 0x04
                const char *pRefName;           // 0x08
                unsigned long unk0C;            // 0x0C
                const char *const *ppAnmSuffix; // 0x10, appended to pAnmName, ends with a null entry
                const char *pAnmName;           // 0x14
                unsigned long unk18;            // 0x18
                // 0x1C, a table of how the animations are played (HockeyF
                // CGoalMdlHockey passes its CGoalMdl::ANM_PARAM table)
                const void *pAnmParam;
            };

            // Builds the parts of a model for Create(const CCreator &). An
            // interface with the vptr at 0x00; its vtable has no destructor.
            // The DOL functions in the vtables of its derived classes have
            // no exported names: the first one makes the model (it looks up
            // MDL_PARAM::pMdlName), the next ones the shadow, the reference
            // model and the animations (names guessed). CCreator's own four
            // are inline in the DOL (weak copies returning 0, 0x8014BDE4 to
            // 0x8014BDFC); they are declared without bodies here so that no
            // unit emits its vtable.
            class CCreator {
                public:
                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;
                    virtual bool CreateShadowMdl(bcs::CMdlShadow *pShadow) const;
                    virtual bool CreateRefMdl(bcs::CMdlRef *pRef) const;
                    virtual bool CreateAnm(CStageAnm *pAnm, bcs::CMdlObj *pMdl) const;
            };

            // The creator of a gimmick model: the DOL vtable
            // __vt__Q33msw7CStgMdl22CGenericGimmickCreator and three words.
            // Declaring its first virtual function without a body keeps the
            // compiler from emitting the vtable.
            class CGenericGimmickCreator : public CCreator {
                public:
                    CGenericGimmickCreator(nw4r::g3d::ResFile res, const MDL_PARAM *pParam)
                        : m_Res(res), m_pParam(pParam), m_pName(0)
                    {
                    }
                    // (added for msw::CStgGenericGmkMdl, which passes a name)
                    CGenericGimmickCreator(nw4r::g3d::ResFile res, const MDL_PARAM *pParam, const char *pName)
                        : m_Res(res), m_pParam(pParam), m_pName(pName)
                    {
                    }

                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;

                private:
                    nw4r::g3d::ResFile m_Res;  // 0x04
                    const MDL_PARAM *m_pParam; // 0x08
                    const char *m_pName;       // 0x0C (name and meaning guessed; msw::CStgGenericGmkMdl sets it)
            };

            // (names guessed) The creator of a model found in an archive:
            // the DOL vtable __vt__Q33msw7CStgMdl15CGenericCreator, a
            // pointer to the resource file and the archive entry. Made on
            // the stack by VolleyF's CStageVolley10.
            class CGenericCreator : public CCreator {
                public:
                    CGenericCreator(const nw4r::g3d::ResFile &rRes, CArn *pArn, long nIdx)
                        : m_pRes(&rRes), m_pArn(pArn), m_nIdx(nIdx)
                    {
                    }

                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;

                private:
                    const nw4r::g3d::ResFile *m_pRes; // 0x04
                    CArn *m_pArn;                     // 0x08
                    long m_nIdx;                      // 0x0C
            };

            // The same with animations (MVA), made on the stack by the
            // modules: the DOL vtable
            // __vt__Q33msw7CStgMdl25CGenericMVAGimmickCreator, one slot
            // more. Its vptr is stored after the base's members, so the
            // members belong to the base.
            class CGenericMVAGimmickCreator : public CGenericGimmickCreator {
                public:
                    CGenericMVAGimmickCreator(nw4r::g3d::ResFile res, const MDL_PARAM *pParam)
                        : CGenericGimmickCreator(res, pParam)
                    {
                    }

                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;
                    // The extra slot (0x18, the last). Inline: the DOL's
                    // game/msw/CStgGmk_Case.cpp emits the weak copy the
                    // vtable uses, right after its Create.
                    virtual ~CGenericMVAGimmickCreator() {}
            };

            CStgMdl(CStgObj *pParent);

            // A creator that makes nothing (a DOL object in .sbss, 4 bytes:
            // only the vptr). Create() passes it.
            static CCreator NullCreator;

            // The arguments of a queued SetAnm (PushAnm), 0x18 bytes.
            struct ANM_ARG {
                unsigned long anm;   // 0x00
                unsigned long arg;   // 0x04
                float speed;         // 0x08
                float frame;         // 0x0C
                float blend;         // 0x10
                unsigned long flag;  // 0x14, passed to IsAnmEndAll before the next one starts
            };

            // How many ANM_ARGs the queue holds (a ring).
            enum { ANM_ARG_NUM = 8 };

        private:
            // (DOL 0x80172104 and 0x80172468, names guessed) Run over the
            // model's materials when Create makes it; not written yet.
            void SetupReflectMat_(bcs::CMdlObj *pMdl);
            void SetupMat_(bcs::CMdlObj *pMdl);

        private:
            CStgObj *m_pParent;          // 0x00
            unsigned long m_nIndex;      // 0x04
            bcs::CMdlObj *m_pMdlObj;     // 0x08, also the scene object
            bcs::CMdlShadow *m_pShadow;  // 0x0C
            bcs::CMdlRef *m_pRef;        // 0x10
            CStageAnm *m_pAnm;           // 0x14
            long m_nAnmArgHead;          // 0x18, the queue of PushAnm (names guessed)
            long m_nAnmArgTail;          // 0x1C
            ANM_ARG m_AnmArg[ANM_ARG_NUM]; // 0x20
            unsigned short m_nLayer;     // 0xE0
            unsigned char m_UnkE2[0x2];  // 0xE2
            unsigned long m_UnkE4;       // 0xE4, 0 after construction (use not known)
            float m_fFadeFrame;          // 0xE8, the fade's frame (FadeIn/FadeOut start it at -delay)
            float m_fFadeLength;         // 0xEC
            long m_nFade;                // 0xF0, 1 fading in, -1 fading out, 0 none
            int m_bSceneEntry;           // 0xF4
            bcs::FMTX34 m_Mtx;           // 0xF8

        public:
            virtual ~CStgMdl();
            // Inline: the modules that emit msw::CStgMdlNull's vtable (the
            // only one that does not override it) have weak copies, which
            // pass NullCreator to Create(const CCreator &).
            virtual void Create() { Create(NullCreator); }
            // Create from pParam in the resource file res (CLoadData).
            virtual void Create(long res, const MDL_PARAM *pParam)
            {
                Create(CGenericGimmickCreator(CLoadData::GetRes(res), pParam));
            }
            virtual void Create(const CCreator &rCreator);
            virtual void DeleteMdl();
            virtual void SetIndex(unsigned long idx) { m_nIndex = idx; }
            virtual unsigned long GetIndex() const { return m_nIndex; }
            virtual void Main(float rate);
            virtual void Commit();
            virtual CStgObj *GetParent() { return m_pParent; }
            virtual const CStgObj *GetParent() const { return m_pParent; }
            virtual void SceneEntry();
            virtual void SceneLeave();
            virtual int IsSceneEntry() const { return m_bSceneEntry; }
            virtual void SetMtx(const bcs::FMTX34 &rMtx) { PSMTXCopy(rMtx.m, m_Mtx.m); }
            virtual const bcs::FMTX34 &GetMtx() const { return m_Mtx; }
            virtual unsigned short GetLayer() const { return m_nLayer; }
            virtual void SetLayer(unsigned short layer) { m_nLayer = layer; }
            virtual void SetDrawPriority(long opa, long xlu);
            virtual bcs::CScnObj *GetScnObj() { return m_pMdlObj; }
            virtual const bcs::CScnObj *GetScnObj() const { return m_pMdlObj; }
            virtual bcs::CMdlObj *GetMdlObj() { return m_pMdlObj; }
            virtual const bcs::CMdlObj *GetMdlObj() const { return m_pMdlObj; }
            virtual bcs::CMdlShadow *GetMdlShadow() { return m_pShadow; }
            virtual const bcs::CMdlShadow *GetMdlShadow() const { return m_pShadow; }
            virtual bcs::CMdlRef *GetMdlRef() { return m_pRef; }
            virtual const bcs::CMdlRef *GetMdlRef() const { return m_pRef; }
            virtual void SetVisible(int bVisible);
            virtual void AttachToMdl(CStgMdl *pMdl, long node, bcs::CScnObj::AttachMode mode);
            virtual void DetachFromMdl();
            // Fades the model in (or out) over time frames, after delay
            // frames.
            virtual void FadeIn(float time, float delay);
            virtual void FadeOut(float time, float delay);
            virtual void FadeUpdate(float rate);
            virtual bool GetNodeMtx(bcs::FMTX34 &rMtx, long node) const;
            virtual bool GetNodeMtx(bcs::FMTX34 &rMtx, const char *pNode) const;
            virtual bool GetNodeWorldMtx(bcs::FMTX34 &rMtx, long node) const;
            virtual bool GetNodeWorldMtx(bcs::FMTX34 &rMtx, const char *pNode) const;
            virtual void SetAnm(unsigned long anm, unsigned long, float speed, float frame, float blend,
                                unsigned long flag);
            virtual void PushAnm(unsigned long anm, unsigned long, float speed, float frame, float blend,
                                 unsigned long flag);
            virtual void PopAnmAll();
            virtual void SetAnmEx(unsigned long anm, unsigned long);
            virtual void CancelAnm();
            virtual void ResetAnm();
            virtual float GetAnmFrame(CStageAnm::ANM_IDX idx) const
            {
                return m_pAnm ? m_pAnm->GetFrame(idx) : 0.0f;
            }
            // Sets the frame of every animation.
            virtual void SetAnmFrame(float frame)
            {
                if (m_pAnm) {
                    m_pAnm->SetFrame(-1, frame);
                }
            }
            virtual float GetAnmTotalFrame(CStageAnm::ANM_IDX idx) const
            {
                return m_pAnm ? m_pAnm->GetTotalFrame(idx) : 0.0f;
            }
            virtual void SetAnmSpeed(float speed);
            virtual bool IsAnmEnd(unsigned long);
            virtual bool IsAnmEndAll(unsigned long);
            virtual bool IsAnmLooped(unsigned long);
            virtual bool IsAnmLoopedAll(unsigned long);
            virtual bool IsAnmLoop(unsigned long);
            virtual bool IsAnmLoopAll(unsigned long);
            virtual bool IsAnmExist(unsigned long anm, CStageAnm::ANM_IDX idx) const
            {
                const CStageAnm *pAnm = GetAnmObj();
                if (pAnm && pAnm->IsAnmExist(anm, idx)) {
                    return true;
                }
                return false;
            }
            virtual unsigned long GetCurAnmId(CStageAnm::ANM_IDX idx) const
            {
                const CStageAnm *pAnm = GetAnmObj();
                return pAnm ? pAnm->GetCurAnmId(idx) : 0;
            }
            virtual CStageAnm *GetAnmObj() { return m_pAnm; }
            virtual const CStageAnm *GetAnmObj() const { return m_pAnm; }
            // Inline (defined below the class): VolleyF's
            // game/volley/CStageNetVolley.cpp has the weak copy, a
            // CStgMdlObjBase<bcs::CMdlObj>.
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual bcs::CMdlShadow *NewShadowMdl(long heap) const { return new (heap) bcs::CMdlShadow; }
            virtual bcs::CMdlRef *NewRefMdl(long heap) const { return new (heap) bcs::CMdlRef; }
            virtual ANM_ARG *AnmArgPush_();
            virtual ANM_ARG *AnmArgPop_();
            virtual const ANM_ARG *AnmArgFront_() const;
            virtual void AnmArgClear_();
            virtual bool AnmArgIsEmpty_() const;
            virtual void AnmQueueUpdate_();
            // Hooks; OnPreCreate returns true to go on with Create.
            virtual bool OnPreCreate() { return true; }
            virtual void OnPostCreate() {}
            virtual void OnSceneEntry() {}
            virtual void OnSceneLeave() {}
            virtual void OnUpdate(float rate) {}
    };
}

#include "bcs/CMdlObj.hpp"
#include "game/msw/CStgMdlObjBase.hpp"

namespace msw {

    // A gimmick model made from a MDL_PARAM of a resource file. A DOL class,
    // 0x140 bytes, vtable 0x80474E70 (game/msw/CStgMdl.cpp): the creator it
    // passes to CStgMdl::Create, and whether to use it.
    class CStgGenericGmkMdl : public CStgMdl {
        public:
            CStgGenericGmkMdl(CStgObj *pParent, nw4r::g3d::ResFile res, const MDL_PARAM *pParam, const char *pName);
            virtual ~CStgGenericGmkMdl();
            virtual void Create();

        private:
            CGenericGimmickCreator m_Creator; // 0x12C
            int m_bUseCreator;                // 0x13C (name guessed), 1 after construction
    };
}

namespace msw {

    inline bcs::CMdlObj *CStgMdl::NewMdl(long heap) const
    {
        return new (heap) CStgMdlObjBase<bcs::CMdlObj>;
    }
}

#endif /* !MSW_CSTGMDL_HPP_ */
