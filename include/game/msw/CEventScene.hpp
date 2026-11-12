#ifndef MSW_CEVENTSCENE_HPP_
#define MSW_CEVENTSCENE_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"
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
            // (added for VolleyF CReadyStartDemoVolley; DOL, names and
            // return types guessed)
            void AddTimerRel(short nId, float fFrame, short, unsigned long);
            float AddCut(const char *pName, long, long, float);
            void SetFloorMove(long, long, int);
            void ResetTimer();
            void _StartFadeOut() const;
            int _IsPlayFade() const;
            // (added for the DOL's victory demos in game/msw/EvDemo.cpp;
            // return types and parameter names guessed) Moves and turns the
            // node paths of the scene (kept at 0x6C/0x78).
            void AddOffset(const bcs::FVEC3 &rPos, const bcs::FVEC3 &rRot, int);

            // (DOL functions unnamed in the DOL; names guessed from what
            // they do, read from their bodies)
            // fn_8011975C: the demo flags of this scene's slot
            // (CEventDemo::m_Work.anFlags, indexed by the camera's word at
            // 0xF4).
            unsigned long _GetDemoFlags() const;
            // fn_8011977C: registers pTask as the demo task of that slot
            // (CEventDemo::m_Work.apTask).
            void _SetDemoTask(bcs::CTask *pTask);
            // fn_80119598: advances the timers by fFrame and fires the
            // virtual EventTimer of each one that is due.
            void _UpdateTimer(float fFrame);
            // fn_8011889C: sets the byte at 0x9E from the cut playing
            // (1 before the first, 4 inside, 7 after the last).
            void _UpdateCutStep();
            // fn_801184BC: starts the camera animation of cut rCut.
            void _PlayCutCamera(const CUT_WORK &rCut);
            // fn_80118B08: puts the models nFirst..nLast - 1 on the node
            // path "player%02d" of the scene; the node, -1 without one.
            long _SetChrNode(long nFirst, long nLast);
            // fn_80118EC4: reads node paths of animation pAnm for the models
            // of team nTeam and plays motion nMot (when not negative) on
            // them at speed fSpeed.
            void _SetTeamNode(const char *pAnm, long nTeam, long nMot, unsigned long nFlag, float fSpeed);

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
            unsigned char m_Unk38[4];    // 0x38
            float m_fUnk3C;              // 0x3C (cleared by VolleyF's CReadyStartDemoVolley::EventPlay)
            unsigned char m_Unk40[0x14]; // 0x40

        public:
            bcs::CCameraAnm *m_pCamera;  // 0x54: made by the constructor, 0x1A4 bytes

        protected:
            CEvNodeData *m_pNodeData;    // 0x58
            CEventModel **m_ppChr;       // 0x5C: one per slot, count in the byte at 0x9B
            unsigned char m_Unk60[4];    // 0x60
            CUT_WORK *m_pCut;            // 0x64: one per scene of the camera animation
            // (0x68 to 0x8C: names and meanings guessed from AddOffset's
            // users) How the node paths are flipped, moved and turned.
            long m_nNodeFlip;            // 0x68
            bcs::FVEC3 m_vNodeOffset;    // 0x6C
            bcs::FVEC3 m_vNodeOffsetRot; // 0x78
            unsigned char m_Unk84[4];    // 0x84
            float m_fNodeRotY;           // 0x88
            unsigned char m_Unk8C[4];    // 0x8C
            // (names guessed) The scene's resource file (GetRes) and the
            // demo flags last seen (see _GetDemoFlags).
            nw4r::g3d::ResFile m_resFile; // 0x90
            unsigned long m_nDemoFlags;  // 0x94
            unsigned short m_nChrMask;   // 0x98: bit n set when slot n holds a model
            unsigned char m_Unk9A;       // 0x9A
            signed char m_nChrNum;       // 0x9B: number of model slots
            signed char m_nCutNum;       // 0x9C: cuts added so far (AddSceneCut)
            signed char m_nUnk9D;        // 0x9D (read signed)
            unsigned char m_nUnk9E;      // 0x9E
            signed char m_nUnk9F;        // 0x9F: not zero while a fade is to be started on cancel
            // 0xA0/0xA2 (names guessed): skip bits requested and the
            // bits that mean no skip (IsEventSkip compares them).
            unsigned short m_nSkip;      // 0xA0
            unsigned short m_nSkipNone;  // 0xA2; the class is 0xA4 bytes
    };
}

#endif /* !MSW_CEVENTSCENE_HPP_ */
