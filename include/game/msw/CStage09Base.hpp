#ifndef MSW_CSTAGE09BASE_HPP_
#define MSW_CSTAGE09BASE_HPP_

namespace bcs {
    class FVEC3;
}

namespace msw {

    class CStageMdlListNode;
    class CStgSeSlot;

    // (purpose guessed from the names) Stage 09, the conveyor belt stage: a
    // second base class next to the sport's stage manager (BasketF's
    // CStageBasket09 has it at 0x218). A DOL class, size 0x18: data first,
    // vptr at 0x14, like CStage16Base. It owns a table of conveyors
    // (CConveyor), made through the sport's NewConveyor, and starts, stops
    // and updates them. The sport's stage gives the number of conveyors, the
    // conveyors' tuning (through its own CConveyor subclass) and its sound
    // slots. Field names and types are guesses from the DOL's accesses.
    // The bodies given here are the ones the modules emit as weak functions;
    // the functions without a body are DOL functions.
    class CStage09Base {
        public:
            // (purpose guessed) How the conveyors relate to each other;
            // CStageBasket09 sets 1 before the stage is initialised. The
            // constructor's default is 1 too.
            enum CV_RELATE {
                CV_RELATE_0,
                CV_RELATE_1
            };

            // One conveyor belt. A DOL class, size 0x50, vptr at 0x4C after
            // the data. The sport's stage derives from it to give the speed
            // and animation tuning (GetParam_*).
            class CConveyor {
                public:
                    // The direction the belt runs in: 0 stopped, 1 and 2 the
                    // two directions along the belt (2 is the reverse of 1),
                    // 3 and 4 a second pair (each the reverse of the other).
                    enum FLOW_DIR {
                        FLOW_DIR_NONE,
                        FLOW_DIR_1,
                        FLOW_DIR_2,
                        FLOW_DIR_3,
                        FLOW_DIR_4
                    };

                    CConveyor();

                private:
                    long m_nIndex;                 // 0x00
                    CStage09Base *m_pOwner;        // 0x04
                    FLOW_DIR m_nFlowDir;           // 0x08
                    float m_fUnk0C;                // 0x0C
                    float m_fUnk10;                // 0x10
                    float m_fUnk14;                // 0x14
                    float m_fCurSpeed;             // 0x18
                    float m_fUnk1C;                // 0x1C
                    float m_fLastSpeed;            // 0x20
                    float m_fOddDist;              // 0x24
                    CStageMdlListNode *m_pMdlNode;  // 0x28
                    CStageMdlListNode *m_pMdlNode2; // 0x2C
                    float m_fUnk30;                // 0x30
                    long m_nSpeedLevel;            // 0x34, nonzero while running fast
                    int m_bForceStop;              // 0x38
                    float m_fDelayTimer;           // 0x3C, below zero when no delayed start or stop is pending
                    long m_nDelayDir;              // 0x40, the direction of a delayed start (0: a delayed stop)
                    long m_bDelayAcc;              // 0x44, a delayed start accelerates (AccStart)
                    float m_fDelayAccTime;         // 0x48, its acceleration time

                public:
                    virtual ~CConveyor();                                                // 0x08
                    virtual long GetIndex() const { return m_nIndex; }                   // 0x0C
                    virtual void SetIndex(long index) { m_nIndex = index; }              // 0x10
                    virtual CStage09Base *GetOwner() { return m_pOwner; }                // 0x14
                    virtual const CStage09Base *GetOwner() const { return m_pOwner; }    // 0x18
                    virtual void SetOwner(CStage09Base *pOwner) { m_pOwner = pOwner; }   // 0x1C
                    virtual void Main(float frame);                                      // 0x20
                    virtual void Start(FLOW_DIR dir);                                    // 0x24
                    virtual void AccStart(FLOW_DIR dir, float time);                     // 0x28
                    virtual void Stop();                                                 // 0x2C
                    virtual void DelayStart(float delay, FLOW_DIR dir);                  // 0x30
                    virtual void DelayAccStart(float delay, FLOW_DIR dir, float time);   // 0x34
                    virtual void DelayStop(float delay);                                 // 0x38
                    virtual float GetDelayTimer() const { return m_fDelayTimer; }        // 0x3C
                    virtual void Reset();                                                // 0x40
                    virtual bool IsStopped() const { return GetFlowDir() == FLOW_DIR_NONE; } // 0x44
                    virtual void SetForceStop(int bStop) { m_bForceStop = bStop; }       // 0x48
                    virtual int IsForceStop() const { return m_bForceStop; }             // 0x4C
                    virtual void SetSpeedLevel(long level);                              // 0x50
                    virtual long GetSpeedLevel() const { return m_nSpeedLevel; }         // 0x54
                    virtual void SetMdlNode(CStageMdlListNode *pNode) { m_pMdlNode = pNode; } // 0x58
                    virtual CStageMdlListNode *GetMdlNode() { return m_pMdlNode; }       // 0x5C
                    virtual const CStageMdlListNode *GetMdlNode() const { return m_pMdlNode; } // 0x60
                    virtual void SetMdlNode2(CStageMdlListNode *pNode) { m_pMdlNode2 = pNode; } // 0x64
                    virtual CStageMdlListNode *GetMdlNode2() { return m_pMdlNode2; }     // 0x68
                    virtual const CStageMdlListNode *GetMdlNode2() const { return m_pMdlNode2; } // 0x6C
                    virtual FLOW_DIR GetFlowDir() const { return m_nFlowDir; }           // 0x70
                    virtual float GetCurSpeed() const { return m_fCurSpeed; }            // 0x74
                    virtual float GetLastSpeed() const { return m_fLastSpeed; }          // 0x78
                    virtual float GetOddDist() const { return m_fOddDist; }              // 0x7C
                    virtual void GetFlow(bcs::FVEC3 &rFlow) const;                       // 0x80
                    virtual float GetFlowRate() const;                                   // 0x84

                    // Tuning, given by the sport's subclass.
                    virtual float GetParam_CvSpeed() const = 0;                          // 0x88, per frame
                    virtual float GetParam_JumpAccFollow() const { return 1.0f; }        // 0x8C
                    virtual float GetParam_JumpAccAgainst() const { return 1.0f; }       // 0x90
                    virtual float GetParam_AnmFlow() const = 0;                          // 0x94
                    virtual float GetParam_MdlAnmDir() const = 0;                        // 0x98, 1 or -1
                    virtual void SetSpeed(float speed);                                  // 0x9C
            };

            CStage09Base();

        private:
            CConveyor **m_ppConveyor; // 0x00, GetConveyorNum entries (CreateConveyor)
            long m_nTblSize;          // 0x04, the number of entries of m_ppConveyor
            CV_RELATE m_nRelate;      // 0x08
            int m_bStopEvent;         // 0x0C, set by a conveyor when its delayed stop happens
            int m_bStartEvent;        // 0x10, set by a conveyor when its delayed start happens

        public:
            virtual ~CStage09Base();                                                     // 0x08
            virtual long GetConveyorNum() const = 0;                                     // 0x0C
            virtual CConveyor *NewConveyor() const = 0;                                  // 0x10
            virtual void CreateConveyor();                                               // 0x14
            virtual void DeleteConveyor();                                               // 0x18
            virtual long GetTblSize() const { return m_nTblSize; }                       // 0x1C
            virtual CConveyor *GetConveyor(long no);                                     // 0x20
            virtual const CConveyor *GetConveyor(long no) const;                         // 0x24
            virtual void Conveyor_UpdateAll(float frame);                                // 0x28
            virtual void Conveyor_Start(long no, CConveyor::FLOW_DIR dir, int, float);   // 0x2C
            virtual void Conveyor_Stop(long no);                                         // 0x30
            virtual void Conveyor_DelayStart(float delay, long no, CConveyor::FLOW_DIR dir, int, float); // 0x34
            virtual void Conveyor_DelayStop(float delay, long no);                       // 0x38
            virtual void Conveyor_StartAll(CConveyor::FLOW_DIR dir, long, float);        // 0x3C
            virtual void Conveyor_AccStartAll(CConveyor::FLOW_DIR dir, float time);      // 0x40
            virtual void Conveyor_StopAll();                                             // 0x44
            virtual void Conveyor_DelayStartAll(float delay, CConveyor::FLOW_DIR dir, long, float); // 0x48
            virtual void Conveyor_DelayAccStartAll(float delay, CConveyor::FLOW_DIR dir, float time); // 0x4C
            virtual void Conveyor_DelayStopAll(float delay);                             // 0x50
            virtual void Conveyor_ResetAll();                                            // 0x54
            virtual bool Conveyor_IsStoppedAll() const;                                  // 0x58
            virtual void Conveyor_SetRelate(CV_RELATE relate) { m_nRelate = relate; }    // 0x5C
            virtual CV_RELATE Conveyor_GetRelate() const { return m_nRelate; }           // 0x60

            // 0x64
            virtual CConveyor::FLOW_DIR Conveyor_GetReverseDir(CConveyor::FLOW_DIR dir) const
            {
                switch (dir) {
                    case CConveyor::FLOW_DIR_1:
                        return CConveyor::FLOW_DIR_2;
                    case CConveyor::FLOW_DIR_2:
                        return CConveyor::FLOW_DIR_1;
                    case CConveyor::FLOW_DIR_3:
                        return CConveyor::FLOW_DIR_4;
                    case CConveyor::FLOW_DIR_4:
                        return CConveyor::FLOW_DIR_3;
                }
                return CConveyor::FLOW_DIR_NONE;
            }

            // The sound slots of the sport's stage: the slot set and the
            // first slot the conveyor sounds use.
            virtual CStgSeSlot *GetSeSlot() = 0;                                         // 0x68
            virtual long GetSeSlotBeg() const = 0;                                       // 0x6C
            virtual void PlayStartSe();                                                  // 0x70
            virtual void PlayStopSe();                                                   // 0x74
            virtual void Stage09_OnPreInitialize();                                      // 0x78

            // Hooks called when conveyors start or stop.
            virtual void Conveyor_OnStart(long no, CConveyor::FLOW_DIR dir, int, float) {} // 0x7C
            virtual void Conveyor_OnStop(long no) {}                                     // 0x80
            virtual void Conveyor_OnSetSpeedLevel(long no, long, long) {}                // 0x84
            virtual void Conveyor_OnStartAll(CConveyor::FLOW_DIR dir, long, float) {}    // 0x88
            virtual void Conveyor_OnStopAll() {}                                         // 0x8C
            virtual long GetParam_SE(long no) const;                                     // 0x90
    };
}

#endif /* !MSW_CSTAGE09BASE_HPP_ */
