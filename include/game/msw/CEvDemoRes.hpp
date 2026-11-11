#ifndef CEVDEMORES_HPP_
#define CEVDEMORES_HPP_

#include "bcs/CTask.hpp"
#include "bcs/FVEC3.hpp"

namespace msw {
    class CEventModel;
    class CStageMan;
    class IEvMdlCreater;

    // Owner of the models of an event demo. A DOL task, 0x84 bytes (the
    // size CreateInstance allocates). Only the members used so far.
    class CEvDemoRes : public bcs::CTask {
        public:
            // Size 0x18. One model of the demo: what it is, the files it is
            // made from and the model once created. The layout is read from
            // the member functions in the DOL; the data member names are
            // guessed.
            struct _CHR_INFO {
                // 1 when the top bit of the byte at 0x16 is set.
                int IsDisp() const;
                // Bit 0 of the flags.
                int IsBind() const;
                void SetBind(int bBind);
                // Bit 1 of the flags.
                int IsCreate() const;
                void SetCreate(int bCreate);
                // The model's shadow size: a table entry chosen by the low
                // three bits of the byte at 0x16 (-1: none).
                short GetShadowSize() const;
                // Bit 2 of the flags. (meaning not known)
                void SetRelease(int bRelease);
                // A player character: the character, its colour, its Mii
                // data and the motion selection (nMotSel). (parameter names
                // guessed)
                void SetChrInit(long nChr, long nColor, void *pMii, long nMotSel);
                // An object (kind 2): its number, its init data and the
                // motion selection. (parameter names guessed)
                void SetObjInit(long nObj, void *pInit, long nMotSel);
                // A background object (kind 3), as SetObjInit. (parameter
                // names guessed)
                void SetObjBgInit(long nObj, void *pInit, long nMotSel);
                void SetTeam(long nTeam, long nNo);
                // bDisp sets bit 0x80 of the third value.
                void SetDispInfo(int bDisp, long nDrawLayer, long nLightSelector, long nDispInfo);

                CEventModel *pModel;     // 0x00
                void *pInit;             // 0x04: third argument of SetChrInit
                // 0x08: for a character, SetChrInit packs its first argument
                // (the character) into the low byte and its second into the
                // high byte; for an object, the object's number.
                short nId;
                // 0x0A, 0x0B: for an object, which file of the event data
                // holds it (the last and the fourth argument of
                // msw::CLoadData::LoadEventData). (names guessed)
                signed char nEvDataKind;
                signed char nEvDataNo;
                signed char nKind;       // 0x0C: 0 character, 2 object, 3 background object
                signed char nMdlFile;    // 0x0D: msw::CLoadData entry of the model
                // 0x0E: msw::CLoadData entries of the motions, indexed by
                // nMotSel; negative while not loaded.
                signed char nMotFile[2];
                signed char nMotSel;     // 0x10: last argument of SetChrInit
                signed char nTeam[2];    // 0x11: the arguments of SetTeam
                signed char nIndex;      // 0x13
                // 0x14, 0x15: second and third values of SetDispInfo; the
                // object creaters in TourF pass them on as the draw layer and
                // the light selector of the model. (names guessed)
                signed char nDrawLayer;
                signed char nLightSelector;
                unsigned char nDispInfo; // 0x16: bit 0x80 is IsDisp
                // 0x17. Written through bit-fields: clearing a bit shows as
                // a rotate-and-mask without the byte mask a plain `&= ~n`
                // gives.
                union {
                    unsigned char nFlag;
                    struct {
                        unsigned char m_Unk17 : 5;
                        unsigned char bRelease : 1;
                        unsigned char bCreate : 1;
                        unsigned char bBind : 1;
                    } m_Flag;
                };
            };

            // The three functions the class's own vtable holds (DOL).
            virtual ~CEvDemoRes();
            virtual void Main();
            virtual void Disp();

            static CEvDemoRes *GetInstance();
            // Called by CEventDemoBase::_Init (DOL), which passes 0x100.
            // (parameter name and return type guessed)
            static CEvDemoRes *CreateInstance(bcs::CTask *pParent);
            void Setup(long nSize);

            // (name guessed) The half-word at 0x6A.
            unsigned short GetFlag() const { return m_nFlag; }
            // (name guessed) Bit 2 of that half-word: the models play their
            // motions mirrored (the creaters pass it to
            // CEventModel::SetMotionReverse). Read as a bit-field: BossF's
            // CEvMdlCreaterGmMdlChr::CreateModel extracts the bit right
            // after the load, where `(GetFlag() >> 2) & 1` defers it.
            int IsReverse() const { return m_Flag.bReverse; }
            // (name guessed) The pointer at 0x4C: the demo's name, the file
            // name under "event/demos" the objects are loaded from.
            const char *GetDemoName() const { return m_pDemoName; }

            // The entry of model nIndex of kind nKind; a new entry when
            // there is none yet. Never null in the DOL body.
            _CHR_INFO &FindIndexNew(long nIndex, long nKind);
            // Gives the entry the two numbers at 0x66/0x68 and its index.
            void SetInit(_CHR_INFO &rInfo, long nIndex);
            // Makes the entry's model with the creater of its kind and
            // returns it (0 when none was made).
            CEventModel *_CreateRes(_CHR_INFO &rInfo);
            // Destroys the model of entry nIndex of kind nKind. (name from
            // the symbol, parameter names guessed)
            void FindIndexDestory(long nIndex, long nKind);
            // (parameter name guessed) The centre the mirrored motions turn
            // about.
            void SetRevCenter(const bcs::FVEC3 &vCenter);

            // (name guessed) The stage manager of the models (written by
            // msw::CTournamentMan in TourF).
            void SetStageMan(CStageMan *pStageMan) { m_pStageMan = pStageMan; }

            // (names guessed) Inline in BossF's CEventDemoBoss constructor,
            // which writes the members directly.
            void SetCreater(long nKind, IEvMdlCreater *pCreater) { m_pCreater[nKind] = pCreater; }
            void SetUnk62(short n) { m_nUnk62 = n; }
            void SetUnk64(short n) { m_nUnk64 = n; }
            void SetFlag(unsigned short nFlag) { m_nFlag = nFlag; }
            // (names guessed) CEventDemoBase::_Init copies the stage and the
            // game mode of CGame::g_GameWork into 0x66/0x68.
            void SetStage(short nStage) { m_nStage = nStage; }
            void SetGameMode(short nGameMode) { m_nGameMode = nGameMode; }

        private:
            // 0x34 (name guessed): what makes the models of each kind
            // (_CHR_INFO::nKind); _CreateRes in the DOL calls the one of the
            // model's kind.
            IEvMdlCreater *m_pCreater[4];
            CStageMan *m_pStageMan;      // 0x44 (name guessed)
            unsigned char m_Unk48[0x04];
            const char *m_pDemoName;     // 0x4C (name guessed)
            unsigned char m_Unk50[0x12];
            short m_nUnk62;              // 0x62: BossF sets 0
            short m_nUnk64;              // 0x64: BossF sets 1
            short m_nStage;              // 0x66 (name guessed)
            short m_nGameMode;           // 0x68 (name guessed)
            union {
                unsigned short m_nFlag;  // 0x6A (name guessed)
                struct {
                    unsigned short m_Unk6A : 13;
                    unsigned short bReverse : 1;
                    unsigned short m_Unk6A_2 : 2;
                } m_Flag;
            };
            unsigned char m_Unk6C[0x18];
    };
}

#endif /* !CEVDEMORES_HPP_ */
