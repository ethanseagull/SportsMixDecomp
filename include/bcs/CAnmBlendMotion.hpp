#ifndef BCS_CANMBLENDMOTION_HPP_
#define BCS_CANMBLENDMOTION_HPP_

#include "bcs/CAnmChrMotion.hpp"
#include "bcs/CAnmObjMotBase.hpp"
#include "nw4r/g3d/ResAnmChr.hpp"
#include "nw4r/g3d/ResFile.hpp"

namespace bcs {

    class CMdlObj;

    // Skeletal animation that blends up to six motions per part of the body
    // (one or two parts, chosen per node by NODE_EX_INFO::nFlag), and
    // cross-fades from the previous set of motions when a new one starts.
    // A DOL class; vtable 0x80463EB8. msw::CChrCore keeps one at 0x94 and
    // wraps most of its interface (the Anim* functions); their names are
    // the source of most names here. Its constructor, destructor, Create
    // and the release function are in the CAnmMotion part of the file
    // (0x80009FB8 to 0x8000A5B4); the rest from 0x8000A5B4.
    //
    // Each part has two banks of motions (m_nCur selects the playing one,
    // the other is the one being faded out), each with m_nLayerNum
    // bcs::CAnmChrMotion layers and a weight per layer.
    class CAnmBlendMotion : public CAnmObjMotBase {
        public:
            enum {
                PART_MAX = 2,
                BANK_NUM = 2,
                LAYER_MAX = 6
            };

            // m_nFlag bits (names guessed).
            enum {
                FLAG_PLAY = 1,       // the part plays
                FLAG_BLEND = 2,      // fading out the previous bank
                FLAG_PAUSE = 4,      // the playing bank's frames do not advance
                FLAG_BLEND_LIVE = 8, // the previous bank keeps playing during the fade
                FLAG_BLEND_MASK = FLAG_BLEND | FLAG_PAUSE | FLAG_BLEND_LIVE
            };

            // ANIM_ARG::nFlag bits besides the ones passed on to the motions
            // (names guessed).
            enum {
                ARG_BLEND_LIVE = 4,  // keep the previous motions playing while fading
                ARG_PAUSE = 8        // start paused
            };

            // The arguments of Play (field names guessed; msw::CChrCore
            // builds them).
            struct ANIM_ARG {
                long nMotID[LAYER_MAX];     // 0x00 motion numbers; negative: mirrored
                float fWeight[LAYER_MAX];   // 0x18
                unsigned long nNum;         // 0x30 layers used
                float fRate;                // 0x34
                float fFrame;               // 0x38 start frame
                float fBlendFrame;          // 0x3C frames of the cross-fade, 0 for none
                unsigned long nFlag;        // 0x40 bit 0 is passed on mirrored; see ARG_*
            };

            // (name guessed) A value going from fStart to fEnd while fT
            // falls from 1 to 0 by fStep per frame.
            struct LERP {
                float fStart; // 0x00
                float fEnd;   // 0x04
                float fT;     // 0x08
                float fStep;  // 0x0C

                // (added for the constructor in the CAnmMotion slice, which
                // clears both arrays of them element by element)
                LERP() { Reset(); }

                float Update(float fFrame)
                {
                    if (fT > 0.0f) {
                        fT -= fStep * fFrame;
                    }
                    if (fT <= 0.0f) {
                        fT = 0.0f;
                        return fEnd;
                    }
                    return fEnd + fT * (fStart - fEnd);
                }

                void Set(float fValue)
                {
                    fStart = fEnd = fValue;
                    fT = fStep = 0.0f;
                }

                void Reset() { fStart = fEnd = fT = fStep = 0.0f; }
            };

            // (DOL fn_80009C78, fn_80009FB8, fn_8000A030; in the CAnmMotion
            // slice) msw::CChrCore allocates 0x284 bytes and calls Create
            // with (2, 6, 0): two parts of six layers.
            CAnmBlendMotion();
            virtual ~CAnmBlendMotion();
            int Create(const CMdlObj *pMdl, nw4r::g3d::ResFile file, const NODE_EX_INFO *pInfo,
                       unsigned long nPartNum, unsigned long nLayerNum, long nHeap);

            virtual void ObjProc(unsigned long nTask, unsigned long, void *pInfo); // 0x1C
            // 0x20 to 0x40: the same for both parts (mask 3).
            virtual void SetFrame(float frame);
            virtual float GetFrame() const;
            virtual float VFunc28() const;
            virtual void SetRate(float rate);
            virtual float GetRate() const;
            virtual void UpdateFrame(float fStep);
            virtual int IsLooped() const;
            virtual int VFunc3C() const;
            virtual int IsEnd() const;

            virtual const nw4r::g3d::ChrAnmResult *GetResult(nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode);
            virtual int VFunc48();
            virtual void SetAnmFrame(unsigned long nMask, float fFrame);
            virtual float GetAnmFrame(unsigned long nMask) const;
            virtual float VFunc58(long nMask) const;
            virtual void SetAnmRate(unsigned long nMask, float fRate);
            virtual float GetAnmRate(unsigned long nMask) const;
            virtual long VFunc64(long nMask, unsigned long nLayer) const;
            virtual float GetBlendWeight(unsigned long nMask, unsigned long nLayer) const;
            virtual long GetNodeMotID(unsigned long nNode, unsigned long nLayer) const;
            virtual const char *GetAnmName(unsigned long nMask, unsigned long nLayer) const;
            virtual const void *GetMotExtInfo(unsigned long nMask, unsigned long nLayer) const;
            virtual const char *GetNodeAnmName(unsigned long nNode, unsigned long nLayer) const;
            // 0x7C, 0x80: inline (weak copies at the end of the file).
            virtual unsigned long GetPartNum() const { return m_nPartNum; }
            virtual unsigned long GetLayerNum() const { return m_nLayerNum; }
            virtual unsigned long GetResAnmNum() const;
            virtual float VFunc88(long nAnim) const;
            virtual int VFunc8C(long nAnim) const;
            virtual const void *GetResMotExtInfo(long nAnim) const;
            virtual const char *GetResAnmName(long nAnim) const;
            virtual int VFunc98(long nMask, unsigned long nLayer) const;
            virtual int VFunc9C(long nMask, unsigned long nLayer) const;
            virtual int VFuncA0(long nMask) const;
            virtual float VFuncA4(long nMask) const;
            virtual void VFuncA8(int bLoop);
            virtual const NODE_EX_INFO *GetNodeExInfo() const;

            // Starts the motions of rArg on the parts in nMask
            // (msw::CChrCore::AnimPlay, which passes 3 for 0).
            void Play(const ANIM_ARG &rArg, unsigned long nMask);
            // (AnimStop) Stops the parts in nMask.
            void Stop(unsigned long nMask);
            // (AnimChgSpeed) Moves the rate to fRate over fFrame frames.
            void ChgAnmRate(unsigned long nMask, float fRate, float fFrame);
            // (AnimChgBlend) Moves the layer weights to pWeight over fFrame
            // frames.
            void ChgBlend(unsigned long nMask, long nNum, const float *pWeight, float fFrame);
            // (IsAnimLooped, IsAnimLoop, IsAnimEnd) For all the parts in
            // nMask.
            int IsLooped(unsigned long nMask) const;
            int IsLoop(unsigned long nMask) const;
            int IsEnd(unsigned long nMask) const;
            // Sets the layer weights at once.
            void SetBlend(const float *pWeight, unsigned long nNum, unsigned long nMask);
            // (AnimSearchTrgFrame) The frame of the first trigger of kind
            // nTrg at or after fFrame in motion nAnim, -1 if none.
            float SearchTrgFrame(long nAnim, unsigned long nTrg, float fFrame) const;
            // (names guessed) Sets the callback of the first layer of each
            // bank.
            void SetTrgCallback(void (*pFunc)(const MOT_TRG *pTrg, void *pUser), void *pUser);

        private:
            // Layer nLayer of bank nBank of part nPart. (Loops over the
            // layers write the same sum out: the original reuses the layer
            // count of the loop test there, which an inline call prevents.)
            CAnmChrMotion *GetMotion(unsigned long nPart, unsigned long nBank, unsigned long nLayer) const
            {
                return m_pMotion + nPart * 2 * m_nLayerNum + nBank * m_nLayerNum + nLayer;
            }

            nw4r::g3d::ResAnmChr GetResAnmChr(long nAnim) const;

            // (DOL fn_8000A3BC, name guessed; in the CAnmMotion slice)
            // Detaches the animation and frees what Create allocated.
            void Release();
            // (name guessed) Clears the play state of both parts. Inline in
            // the constructor, Create and Release.
            void ResetPlay()
            {
                for (int i = 0; i < PART_MAX; i++) {
                    m_nFlag[i] = 0;
                    m_nCur[i] = 0;
                    m_fBlendFrame[i] = 0.0f;
                    m_fBlendStep[i] = 0.0f;
                    m_fBlendRate[i] = 0.0f;
                }
            }

            // The blended result of node nNode of one bank of a part.
            const nw4r::g3d::ChrAnmResult *GetBankResult(unsigned long nPart, unsigned long nBank,
                                                         nw4r::g3d::ChrAnmResult *pResult, unsigned long nNode);
            // Fills m_pResult[0] for one part or for two.
            void CalcResult();
            void CalcResultParts();

            const CMdlObj *m_pMdl;                      // 0x18
            const NODE_EX_INFO *m_pNodeExInfo;          // 0x1C
            MOT_EX_INFO *m_pExtInfo;                    // 0x20 mot_ex_info.bin of the file, if any
            nw4r::g3d::ResFile m_File;                  // 0x24
            unsigned short m_nPartNum;                  // 0x28
            unsigned short m_nLayerNum;                 // 0x2A
            unsigned long m_nFlag[PART_MAX];            // 0x2C
            unsigned long m_nCur[PART_MAX];             // 0x34 the playing bank
            float m_fBlendFrame[PART_MAX];              // 0x3C
            float m_fBlendStep[PART_MAX];               // 0x44
            float m_fBlendRate[PART_MAX];               // 0x4C 1 at the start of a fade, 0 at its end
            LERP m_RateLerp[PART_MAX][BANK_NUM];        // 0x54
            LERP m_WeightLerp[PART_MAX][BANK_NUM][LAYER_MAX]; // 0x94
            float m_fWeight[PART_MAX][BANK_NUM][LAYER_MAX];   // 0x214
            CAnmChrMotion *m_pMotion;                   // 0x274 [part][bank][layer]
            nw4r::g3d::ChrAnmResult *m_pResult[2];      // 0x278 the output, and the pose faded out from
            unsigned short *m_pNodeFlagAll;             // 0x280 the node flags of all the motions
    };
}

#endif /* !BCS_CANMBLENDMOTION_HPP_ */
