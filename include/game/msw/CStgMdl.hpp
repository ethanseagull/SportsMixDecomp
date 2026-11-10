#ifndef MSW_CSTGMDL_HPP_
#define MSW_CSTGMDL_HPP_

#include "bcs/CScnObj.hpp"
#include "game/msw/CStageAnm.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    struct FMTX34;
    class CAnmObj;
    class CMdlObj;
    class CMdlRef;
    class CMdlShadow;
}

namespace msw {

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
                unsigned long unk1C;            // 0x1C
            };

            // Builds the parts of a model for Create(const CCreator &). An
            // interface with the vptr at 0x00; its vtable has no destructor.
            // The DOL functions in the vtables of its derived classes have
            // no exported names: the first one makes the model (it looks up
            // MDL_PARAM::pMdlName), the next ones the shadow, the reference
            // model and the animations. Only the first is declared (name
            // guessed).
            class CCreator {
                public:
                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;
            };

            // The creator of a gimmick model: the DOL vtable
            // __vt__Q33msw7CStgMdl22CGenericGimmickCreator and three words.
            // Declaring its first virtual function without a body keeps the
            // compiler from emitting the vtable.
            class CGenericGimmickCreator : public CCreator {
                public:
                    CGenericGimmickCreator(nw4r::g3d::ResFile res, const MDL_PARAM *pParam)
                        : m_Res(res), m_pParam(pParam), m_Unk0C(0)
                    {
                    }

                    virtual bool CreateMdl(bcs::CMdlObj *pMdl) const;

                private:
                    nw4r::g3d::ResFile m_Res;  // 0x04
                    const MDL_PARAM *m_pParam; // 0x08
                    unsigned long m_Unk0C;     // 0x0C
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
            };

            CStgMdl(CStgObj *pParent);

        private:
            CStgObj *m_pParent;           // 0x00
            unsigned char m_Unk04[0x124]; // 0x04

        public:
            virtual ~CStgMdl();
            virtual void Create();
            virtual void Create(long res, const MDL_PARAM *pParam);
            virtual void Create(const CCreator &rCreator);
            virtual void DeleteMdl();
            virtual void SetIndex(unsigned long idx);
            virtual unsigned long GetIndex() const;
            virtual void Main(float rate);
            virtual void Commit();
            virtual CStgObj *GetParent();
            virtual const CStgObj *GetParent() const;
            virtual void SceneEntry();
            virtual void SceneLeave();
            virtual bool IsSceneEntry() const;
            virtual void SetMtx(const bcs::FMTX34 &rMtx);
            virtual const bcs::FMTX34 &GetMtx() const;
            virtual unsigned short GetLayer() const;
            virtual void SetLayer(unsigned short layer);
            virtual void SetDrawPriority(long opa, long xlu);
            virtual bcs::CScnObj *GetScnObj();
            virtual const bcs::CScnObj *GetScnObj() const;
            virtual bcs::CMdlObj *GetMdlObj();
            virtual const bcs::CMdlObj *GetMdlObj() const;
            virtual bcs::CMdlShadow *GetMdlShadow();
            virtual const bcs::CMdlShadow *GetMdlShadow() const;
            virtual bcs::CMdlRef *GetMdlRef();
            virtual const bcs::CMdlRef *GetMdlRef() const;
            virtual void SetVisible(int bVisible);
            virtual void AttachToMdl(CStgMdl *pMdl, long node, bcs::CScnObj::AttachMode mode);
            virtual void DetachFromMdl();
            virtual void FadeIn(float time, float alpha);
            virtual void FadeOut(float time, float alpha);
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
            virtual float GetAnmFrame(CStageAnm::ANM_IDX idx) const;
            virtual void SetAnmFrame(float frame);
            virtual float GetAnmTotalFrame(CStageAnm::ANM_IDX idx) const;
            virtual void SetAnmSpeed(float speed);
            virtual bool IsAnmEnd(unsigned long);
            virtual bool IsAnmEndAll(unsigned long);
            virtual bool IsAnmLooped(unsigned long);
            virtual bool IsAnmLoopedAll(unsigned long);
            virtual bool IsAnmLoop(unsigned long);
            virtual bool IsAnmLoopAll(unsigned long);
            virtual bool IsAnmExist(unsigned long anm, CStageAnm::ANM_IDX idx) const;
            virtual unsigned long GetCurAnmId(CStageAnm::ANM_IDX idx) const;
            virtual bcs::CAnmObj *GetAnmObj();
            virtual const bcs::CAnmObj *GetAnmObj() const;
            virtual bcs::CMdlObj *NewMdl(long heap) const;
            virtual bcs::CMdlShadow *NewShadowMdl(long heap) const;
            virtual bcs::CMdlRef *NewRefMdl(long heap) const;
            virtual void AnmArgPush_();
            virtual void AnmArgPop_();
            virtual const void *AnmArgFront_() const;
            virtual void AnmArgClear_();
            virtual bool AnmArgIsEmpty_() const;
            virtual void AnmQueueUpdate_();
            virtual void OnPreCreate();
            virtual void OnPostCreate();
            virtual void OnSceneEntry();
            virtual void OnSceneLeave();
            virtual void OnUpdate(float rate);
    };
}

#endif /* !MSW_CSTGMDL_HPP_ */
