#ifndef MSW_CEVENTSCENE_HPP_
#define MSW_CEVENTSCENE_HPP_

#include "bcs/CTask.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {
    class CCameraAnm;
}

namespace msw {

    class CEvNodeData;
    class CEventModel;

    // An event scene: a camera animation with the event models that play
    // in it. A DOL class (constructor __ct__Q23msw11CEventSceneFlPQ23bcs5CTaskl,
    // vtable 0x8046F4C8). Only the members msw::CSceneCalc reads are mapped
    // so far; names guessed.
    class CEventScene : public bcs::CTask {
        public:
            // (added for VolleyF CPerfectDemoVolley; member names guessed)
            // One cut of the scene, 0x24 bytes; EventFirst fills an array of
            // them, one per scene of the camera animation.
            struct CUT_WORK {
                const char *pName; // 0x00: node name ("NoData" until set)
                short nUnk04;      // 0x04
                short nUnk06;      // 0x06
                short nUnk08;      // 0x08
                short nUnk0A;      // 0x0A
                long nUnk0C;       // 0x0C
                float fUnk10;      // 0x10
                float fUnk14;      // 0x14
                float fUnk18;      // 0x18
                float fUnk1C;      // 0x1C
                float fUnk20;      // 0x20
            };

            CEventScene(long, bcs::CTask *pParent, long);
            virtual ~CEventScene();
            virtual void Main();
            virtual void Disp() {}

            // The virtual functions of the scene. The bodies are the DOL's
            // defaults (unnamed there); the modules emit the last four weak.
            virtual nw4r::g3d::ResFile GetRes() { return nw4r::g3d::ResFile(); }   // 0x18
            virtual int EventFirst() { return 9; }                                  // 0x1C
            virtual void EventStart() {}                                            // 0x20
            virtual void EventKill() {}                                             // 0x24
            virtual long EventPlay(CUT_WORK &rCut) { return 7; }                    // 0x28
            virtual long EventCansel(CUT_WORK &rCut, long nRet) { return nRet; }    // 0x2C
            virtual void CheckEventSkip() {}                                        // 0x30
            virtual int IsEventSkip() { return 0; }                                 // 0x34
            virtual void EventTimer(short nId, short, unsigned long) {}             // 0x38
            virtual void EventTimerReset(short nId, short, unsigned long) {}        // 0x3C
            virtual void EventFlags(unsigned long, unsigned long) {}                // 0x40

            // (DOL functions; return types and parameter names guessed)
            void SetDispChr(int bDisp);
            int IsFadeOutOnly(long nCut) const;
            long StartFadeOut(const CUT_WORK &rCut, long nCut);
            float AddSceneCut(long nCut);
            void SetNodeChr(const char *pNode, long nChr, long, unsigned long);
            void CreateTimer(long nNum);
            void AddTimer(short nId, float fFrame, short, unsigned long);

            // The model nChr when that slot is in use (bit nChr of the mask
            // at 0x98), null otherwise.
            CEventModel *GetChr(long nChr) const
            {
                if (nChr != -1 && (m_nChrMask & (1 << nChr))) {
                    return m_ppChr[nChr];
                }
                return 0;
            }

        protected:
            float m_fUnk34;              // 0x34 (read by VolleyF's CPerfectDemoVolley::EventStart)
            unsigned char m_Unk38[0x1C]; // 0x38

        public:
            bcs::CCameraAnm *m_pCamera;  // 0x54: made by the constructor, 0x1A4 bytes

        protected:
            CEvNodeData *m_pNodeData;    // 0x58
            CEventModel **m_ppChr;       // 0x5C: one per slot, count in the byte at 0x9B
            unsigned char m_Unk60[4];    // 0x60
            CUT_WORK *m_pCut;            // 0x64: one per scene of the camera animation
            unsigned char m_Unk68[0x30]; // 0x68
            unsigned short m_nChrMask;   // 0x98: bit n set when slot n holds a model
            unsigned char m_Unk9A;       // 0x9A
            signed char m_nChrNum;       // 0x9B: number of model slots
            signed char m_nCutNum;       // 0x9C: cuts added so far (AddSceneCut)
            unsigned char m_Unk9D;       // 0x9D
            unsigned char m_nUnk9E;      // 0x9E
            unsigned char m_Unk9F[5];    // 0x9F; the class is 0xA4 bytes
    };
}

#endif /* !MSW_CEVENTSCENE_HPP_ */
