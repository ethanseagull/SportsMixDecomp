#ifndef MSW_CSTGSESLOT_HPP_
#define MSW_CSTGSESLOT_HPP_

#include "bcs/FVEC3.hpp"

namespace bcs {
    class CTask;
    class CSndTask;
}

namespace msw {

    // A fixed set of sound effect slots owned by a stage object; each slot
    // plays one sound at a time. A DOL class, size 0xC with the vptr at 0x08
    // (the two data members are declared before the first virtual function).
    //
    // The order of the virtual functions is the order of the slots in the
    // second part of __vt__Q23msw13CStageManBoss (BossF). The bodies given
    // here are the ones the modules emit as weak functions; the others are
    // DOL functions. Parameter names and the return types of the functions
    // without a body here are guessed, except where a note says otherwise.
    class CStgSeSlot {
        public:
            // One slot, 0x50 bytes, the vptr at 0x4C (the data members are
            // declared before the virtual destructor). The member functions
            // are defined in game/msw/CStgSeSlot.cpp. Names are guesses.
            class CSESlot {
                public:
                    // Supplies the position of a sound that follows something.
                    class CPosFunc;

                    // Where the sound plays (m_nMode).
                    enum {
                        MODE_FUNC = 0, // at m_Pos, which m_pFunc updates
                        MODE_POS = 1,  // at the position given
                        MODE_NONE = 2, // without a position
                    };

                    // Bits of m_nFlag, copied to m_nState when the sound starts.
                    enum {
                        FLAG_ALIVE = 1 << 0,   // counts as alive even when silent
                        FLAG_LOOP = 1 << 1,    // started again when it ends
                        FLAG_NOTRACK = 1 << 2, // its task is not kept (sounds overlap)
                        FLAG_REPEAT = 1 << 3,  // started again after m_fDelay
                    };

                    unsigned long m_nHandle;  // 0x00, id of the sound's task
                    unsigned long m_nState;   // 0x04, FLAG_* bits of the playing sound
                    float m_fDelay;           // 0x08, frames until it starts; -1 when nothing waits
                    float m_fStopDelay;       // 0x0C, frames until it stops; -1 when nothing waits
                    float m_fStopFade;        // 0x10
                    float m_fInterval;        // 0x14, repeat interval
                    float m_fIntervalRand;    // 0x18, random extra on the interval
                    long m_nSe;               // 0x1C
                    unsigned long m_nFlag;    // 0x20
                    long m_nMode;             // 0x24
                    unsigned long m_nArg2;    // 0x28, the CGameSnd::Play arguments
                    long m_nArg3;             // 0x2C
                    float m_fArg4;            // 0x30
                    float m_fVolume;          // 0x34
                    float m_fArg6;            // 0x38
                    bcs::FVEC3 m_Pos;         // 0x3C
                    CPosFunc *m_pFunc;        // 0x48

                    CSESlot();
                    virtual ~CSESlot();

                    bcs::CSndTask *GetTask() const;
                    bool IsPlaying() const;
                    bool IsAlive() const;
                    void StopSound(float fade);
                    void Stop(float fade);
                    unsigned long Start(int bLoop);
                    void SetNextDelay();
                    void UpdateDelay(float rate);
                    void Set(long se, long n, long mode, unsigned long a, long b, float f4, float vol, float f6);
                    void SetPos(CPosFunc *pFunc);
                    unsigned long Play(long se, long n, unsigned long a, long b, float f4, float vol, float f6);
                    unsigned long Play(long se, const bcs::FVEC3 &rPos, long n, unsigned long a, long b, float f4,
                                       float vol, float f6);
                    unsigned long Play(long se, CPosFunc *pFunc, long n, unsigned long a, long b, float f4,
                                       float vol, float f6);
                    void DelayPlay(float delay, long se, long n, unsigned long a, long b, float f4, float vol,
                                   float f6);
                    void DelayPlay(float delay, long se, const bcs::FVEC3 &rPos, long n, unsigned long a, long b,
                                   float f4, float vol, float f6);
                    void DelayPlay(float delay, long se, CPosFunc *pFunc, long n, unsigned long a, long b, float f4,
                                   float vol, float f6);
                    void StartRepeat(float delay, float interval, float rand);
                    void RepeatPlay(float delay, long se, float interval, float rand, long n, unsigned long a,
                                    long b, float f4, float vol, float f6);
                    void RepeatPlay(float delay, long se, const bcs::FVEC3 &rPos, float interval, float rand,
                                    long n, unsigned long a, long b, float f4, float vol, float f6);
                    void RepeatPlay(float delay, long se, CPosFunc *pFunc, float interval, float rand, long n,
                                    unsigned long a, long b, float f4, float vol, float f6);
                    void Update(float rate);
            };

            // The last five arguments of the *Ex functions, in their order.
            // The plain functions pass 1, -1, 0.0f, 1.0f and their own float
            // argument.
            struct OPTS {
                unsigned long m_Unk00; // 0x00
                long m_Unk04;          // 0x04
                float m_Unk08;         // 0x08
                float m_Unk0C;         // 0x0C
                float m_Unk10;         // 0x10
            };

        private:
            CSESlot *m_pSlot;  // 0x00, SE_GetSlotNum() slots, made by SE_Init
            long m_nSlotNum;   // 0x04

        public:
            CStgSeSlot();
            virtual ~CStgSeSlot();
            virtual void SE_Init();
            virtual long SE_GetSlotNum() const { return 0; }
            // SE_GetHandle and SE_PlayEx return the slot's first word.
            virtual unsigned long SE_GetHandle(long slot);
            virtual bcs::CTask *SE_GetObject(long slot);
            virtual bool SE_IsAlive(long slot);
            virtual unsigned long SE_Play(long slot, long se, float f, long n)
            {
                return SE_PlayEx(slot, se, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual unsigned long SE_PlayPos(long slot, long se, const bcs::FVEC3 &rPos, float f, long n)
            {
                return SE_PlayPosEx(slot, se, rPos, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual unsigned long SE_PlayPos(long slot, long se, float f, long n, CSESlot::CPosFunc *pFunc)
            {
                return SE_PlayPosEx(slot, se, n, pFunc, 1, -1, 0.0f, 1.0f, f);
            }
            virtual unsigned long SE_PlayEx(long slot, long se, long n, unsigned long, long, float, float, float);
            virtual unsigned long SE_PlayPosEx(long slot, long se, const bcs::FVEC3 &rPos, long n, unsigned long, long,
                                               float, float, float);
            virtual unsigned long SE_PlayPosEx(long slot, long se, long n, CSESlot::CPosFunc *pFunc, unsigned long,
                                               long, float, float, float);
            virtual void SE_Stop(long slot, float fade);
            virtual void SE_StopAll();
            virtual void SE_SetVolume(long slot, float, float);
            virtual void SE_DelayPlay(long slot, float delay, long se, float f, long n)
            {
                SE_DelayPlayEx(slot, delay, se, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_DelayPlayPos(long slot, float delay, long se, const bcs::FVEC3 &rPos, float f, long n)
            {
                SE_DelayPlayPosEx(slot, delay, se, rPos, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_DelayPlayPos(long slot, float delay, long se, float f, long n, CSESlot::CPosFunc *pFunc)
            {
                SE_DelayPlayPosEx(slot, delay, se, n, pFunc, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_DelayPlayEx(long slot, float delay, long se, long n, unsigned long, long, float, float,
                                        float);
            virtual void SE_DelayPlayPosEx(long slot, float delay, long se, const bcs::FVEC3 &rPos, long n,
                                           unsigned long, long, float, float, float);
            virtual void SE_DelayPlayPosEx(long slot, float delay, long se, long n, CSESlot::CPosFunc *pFunc,
                                           unsigned long, long, float, float, float);
            virtual void SE_DelayStop(long slot, float, float);
            virtual void SE_RepeatPlay(long slot, long se, float f1, float f2, float f3, float f, long n)
            {
                SE_RepeatPlayEx(slot, se, f1, f2, f3, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_RepeatPlayPos(long slot, long se, const bcs::FVEC3 &rPos, float f1, float f2, float f3,
                                          float f, long n)
            {
                SE_RepeatPlayPosEx(slot, se, rPos, f1, f2, f3, n, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_RepeatPlayPos(long slot, long se, float f1, float f2, float f3, float f, long n,
                                          CSESlot::CPosFunc *pFunc)
            {
                SE_RepeatPlayPosEx(slot, se, f1, f2, f3, n, pFunc, 1, -1, 0.0f, 1.0f, f);
            }
            virtual void SE_RepeatPlayEx(long slot, long se, float, float, float, long n, unsigned long, long, float,
                                         float, float);
            virtual void SE_RepeatPlayPosEx(long slot, long se, const bcs::FVEC3 &rPos, float, float, float, long n,
                                            unsigned long, long, float, float, float);
            virtual void SE_RepeatPlayPosEx(long slot, long se, float, float, float, long n,
                                            CSESlot::CPosFunc *pFunc, unsigned long, long, float, float, float);
            virtual unsigned long SE_PlayOpts(long slot, long se, const OPTS &rOpts, long n)
            {
                return SE_PlayEx(slot, se, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08, rOpts.m_Unk0C,
                                 rOpts.m_Unk10);
            }
            virtual unsigned long SE_PlayPosOpts(long slot, long se, const bcs::FVEC3 &rPos, const OPTS &rOpts, long n)
            {
                return SE_PlayPosEx(slot, se, rPos, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08, rOpts.m_Unk0C,
                                    rOpts.m_Unk10);
            }
            virtual unsigned long SE_PlayPosOpts(long slot, long se, const OPTS &rOpts, long n,
                                                 CSESlot::CPosFunc *pFunc)
            {
                return SE_PlayPosEx(slot, se, n, pFunc, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08, rOpts.m_Unk0C,
                                    rOpts.m_Unk10);
            }
            virtual void SE_DelayPlay(long slot, float delay, long se, const OPTS &rOpts, long n)
            {
                SE_DelayPlayEx(slot, delay, se, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08, rOpts.m_Unk0C,
                               rOpts.m_Unk10);
            }
            virtual void SE_DelayPlayPos(long slot, float delay, long se, const bcs::FVEC3 &rPos, const OPTS &rOpts,
                                         long n)
            {
                SE_DelayPlayPosEx(slot, delay, se, rPos, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08,
                                  rOpts.m_Unk0C, rOpts.m_Unk10);
            }
            virtual void SE_DelayPlayPos(long slot, float delay, long se, const OPTS &rOpts, long n,
                                         CSESlot::CPosFunc *pFunc)
            {
                SE_DelayPlayPosEx(slot, delay, se, n, pFunc, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08,
                                  rOpts.m_Unk0C, rOpts.m_Unk10);
            }
            // The fourth float is not used: the options supply the last three.
            virtual void SE_RepeatPlay(long slot, long se, const OPTS &rOpts, float f1, float f2, float f3, float,
                                       long n)
            {
                SE_RepeatPlayEx(slot, se, f1, f2, f3, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08, rOpts.m_Unk0C,
                                rOpts.m_Unk10);
            }
            virtual void SE_RepeatPlayPos(long slot, long se, const bcs::FVEC3 &rPos, const OPTS &rOpts, float f1,
                                          float f2, float f3, float, long n)
            {
                SE_RepeatPlayPosEx(slot, se, rPos, f1, f2, f3, n, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08,
                                   rOpts.m_Unk0C, rOpts.m_Unk10);
            }
            virtual void SE_RepeatPlayPos(long slot, long se, const OPTS &rOpts, float f1, float f2, float f3, float,
                                          long n, CSESlot::CPosFunc *pFunc)
            {
                SE_RepeatPlayPosEx(slot, se, f1, f2, f3, n, pFunc, rOpts.m_Unk00, rOpts.m_Unk04, rOpts.m_Unk08,
                                   rOpts.m_Unk0C, rOpts.m_Unk10);
            }
            virtual void SE_Update(float);
            virtual CSESlot::CPosFunc *SE_GetDefaultFunc() { return 0; }
    };

    // (purpose and names guessed) Supplies the position of a sound that
    // follows something: the slot asks GetPos each time. The vptr is at
    // 0x00. Layout from the DOL's msw::CStage04Base::CCartNode::CPosFunc
    // (vtable lbl_8048E500: the destructor, then GetPos).
    class CStgSeSlot::CSESlot::CPosFunc {
        public:
            virtual ~CPosFunc() {}
            virtual void GetPos(bcs::FVEC3 *pPos) = 0;
    };
}

#endif /* !MSW_CSTGSESLOT_HPP_ */
