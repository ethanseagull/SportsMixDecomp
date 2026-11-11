#ifndef MSW_CSTAGE02BASE_HPP_
#define MSW_CSTAGE02BASE_HPP_

#include "bcs/CListNodeBase.hpp"
#include "bcs/FVEC3.hpp"
#include "bcs/New.hpp"
#include "game/msw/CStageMan.hpp"

namespace bcs {
    class CRegex;
    class CTask;
}

namespace nw4r {
    namespace g3d {
        class ResFile;
    }
}

namespace msw {

    class CArn;
    class CStgGmk_WaveCluster;

    namespace STAGE {
        struct MDL_GEN_RULE;
    }

    // (purpose guessed from the names) The waves of a sport's stage 02: a
    // second base class next to the sport's stage manager (BasketF's
    // msw::CStageBasket02 has it at 0x218). A DOL class, size 0x34: data
    // first, vptr at 0x30 (like CStage04Base). Layout read from the DOL
    // constructor (0x80215608). The sport's stage hands its hooks on
    // (Stage02_*), makes the wave cluster (NewWaveCluster) and gives the
    // tuning through the GetParam_* functions. The order of the virtual
    // functions is that of the DOL vtable (lbl_80488608).
    class CStage02Base {
        public:
            // (name guessed) A splash effect that follows the waves: a list
            // node, size 0x28. DOL class; its constructor, destructor, Main,
            // Kick, Stop, Erase, StopAll, EraseAll and GetEffId are in the
            // DOL (game/msw/CStage02Base/CSplashEff.cpp); the position
            // accessors are inline (the modules emit them weak).
            class CSplashEff : public bcs::CListNodeBase {
                public:
                    CSplashEff(long, const bcs::FVEC3 &rPos);
                    virtual ~CSplashEff();
                    // Called each frame by the splash list.
                    virtual void Main(float frame, const CStgGmk_WaveCluster *pCluster);
                    virtual void Kick(long);
                    virtual void Stop(long);
                    virtual void Erase(long);
                    virtual void StopAll();
                    virtual void EraseAll();
                    virtual void SetPos(const bcs::FVEC3 &rPos) { m_vPos = rPos; }
                    virtual const bcs::FVEC3 &GetPos() const { return m_vPos; }
                    virtual void GetPos(bcs::FVEC3 &rPos) const { rPos = m_vPos; }
                    // The effect played for kind `type`, or -1.
                    virtual long GetEffId(long type) const;

                private:
                    long m_Unk10;                // 0x10, the constructor's first argument
                    unsigned char m_Unk14[0x8];  // 0x14, two 4-byte objects with a DOL constructor and destructor (effect handles?)
                    bcs::FVEC3 m_vPos;           // 0x1C
            };

            // (name guessed) A splash with one fixed effect, size 0x30: the
            // effect nEffId for kind nType. Made by NewSplashNode.
            class CSplashEffImm : public CSplashEff {
                public:
                    CSplashEffImm(long nType, long nEffId, const bcs::FVEC3 &rPos)
                        : CSplashEff(0, rPos), m_nType(nType), m_nEffId(nEffId)
                    {
                    }
                    virtual ~CSplashEffImm() {}

                    virtual long GetEffId(long type) const
                    {
                        if (type == m_nType) {
                            return m_nEffId;
                        }
                        return -1;
                    }

                private:
                    long m_nType;  // 0x28
                    long m_nEffId; // 0x2C
            };

            CStage02Base(bcs::CTask *pParent);

        private:
            // (name guessed) The list of splash effects, size 0x28: a
            // bcs::CListBase, one word, then its own vptr at 0x24. Its
            // functions are in the DOL (vtable lbl_80488664; the slot at 0x10
            // runs the splashes with the wave cluster). Not mapped further.
            class CSplashList : public bcs::CListBase {
                private:
                    unsigned long m_Unk20; // 0x20

                public:
                    virtual ~CSplashList();
            };

            bcs::CTask *m_pParent;                 // 0x00
            CStgGmk_WaveCluster *m_pWaveCluster;   // 0x04, made by NewWaveCluster
            CSplashList m_SplashList;              // 0x08

        public:
            virtual ~CStage02Base();
            // Called by the stage's Main_ every frame.
            virtual void Stage02_Main(float frame);
            // Bit 1 when rPos is on a wave.
            virtual long Stage02_GetFloorAttribute(const bcs::FVEC3 &rPos) const;
            virtual void Stage02_GetFloorFlow(bcs::FVEC3 &rFlow, const bcs::FVEC3 &rPos,
                                              CStageMan::FLOW_TYPE type) const;
            // Tuning, overridden by the sport's stage. The defaults are the
            // DOL's unnamed copies (0.7, 0.4, 2.0).
            virtual float GetParam_PushPower() const { return 0.7f; }
            virtual float GetParam_PullPower() const { return 0.4f; }
            virtual float GetParam_WavePosMargin() const { return 2.0f; }
            // Whether the stage's gimmick runs.
            virtual int Stage02_IsGimmickReady() const { return !CStageMan::GetInstance()->IsGimmickStop(); }
            // The DOL's default makes a plain msw::CStgGmk_WaveCluster.
            virtual CStgGmk_WaveCluster *NewWaveCluster(bcs::CTask *pParent) const;
            virtual CStgGmk_WaveCluster *GetWaveCluster() { return m_pWaveCluster; }
            virtual const CStgGmk_WaveCluster *GetWaveCluster() const { return m_pWaveCluster; }
            // Adds a splash at rPos playing effect nEffId for kind nType.
            // The node is linked even if the allocation failed, as in the
            // original.
            virtual void NewSplashNode(long nType, long nEffId, const bcs::FVEC3 &rPos)
            {
                CSplashEffImm *pEff = new (1) CSplashEffImm(nType, nEffId, rPos);
                pEff->Link(m_SplashList.GetEnd());
            }
            virtual bool Stage02_OnPreCreateMdl(const STAGE::MDL_GEN_RULE *pRule, bcs::CRegex &rRegex,
                                                nw4r::g3d::ResFile file, CArn &rArn, long n);
            virtual void Stage02_OnPostInitialize();
            virtual void Stage02_OnSceneEntry();
            virtual void Stage02_OnPeriodInit();
            virtual void Stage02_OnGimmickReset();
            virtual void Stage02_OnGimmickPause();
            virtual void Stage02_OnGimmickResume();
            // Whether rPos is on a wave.
            virtual int Stage02_IsOnWave(const bcs::FVEC3 &rPos) const;
            virtual unsigned long Stage02_OnCalcSum() const;
    };
}

#endif /* !MSW_CSTAGE02BASE_HPP_ */
