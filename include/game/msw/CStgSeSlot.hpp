#ifndef MSW_CSTGSESLOT_HPP_
#define MSW_CSTGSESLOT_HPP_

namespace bcs {
    struct FVEC3;
    class CTask;
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
            // One slot, 0x50 bytes. Only the first member is mapped.
            class CSESlot {
                public:
                    // Supplies the position of a sound that follows something.
                    class CPosFunc;

                private:
                    unsigned long m_nHandle;     // 0x00, id of the sound's task
                    unsigned char m_Unk04[0x4C]; // 0x04
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
}

#endif /* !MSW_CSTGSESLOT_HPP_ */
