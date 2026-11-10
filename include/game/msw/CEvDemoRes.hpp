#ifndef CEVDEMORES_HPP_
#define CEVDEMORES_HPP_

#include "bcs/CTask.hpp"

namespace msw {
    class CEventModel;
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

                CEventModel *pModel;     // 0x00
                void *pInit;             // 0x04: third argument of SetChrInit
                // 0x08: for a character, SetChrInit packs its first argument
                // (the character) into the low byte and its second into the
                // high byte; for an object, the object's number.
                short nId;
                unsigned char m_Unk0A[2];
                signed char nKind;       // 0x0C: 0 character, 2 object, 3 background object
                signed char nMdlFile;    // 0x0D: msw::CLoadData entry of the model
                // 0x0E: msw::CLoadData entries of the motions, indexed by
                // nMotSel; negative while not loaded.
                signed char nMotFile[2];
                signed char nMotSel;     // 0x10: last argument of SetChrInit
                signed char nTeam[2];    // 0x11: the arguments of SetTeam
                signed char nIndex;      // 0x13
                signed char m_Unk14[2];  // 0x14: second and third values of SetDispInfo
                unsigned char nDispInfo; // 0x16: bit 0x80 is IsDisp
                unsigned char nFlag;     // 0x17
            };

            // The three functions the class's own vtable holds (DOL).
            virtual ~CEvDemoRes();
            virtual void Main();
            virtual void Disp();

            static CEvDemoRes *GetInstance();

            // (name guessed) The half-word at 0x6A.
            unsigned short GetFlag() const { return m_nFlag; }

        private:
            // 0x34 (name guessed): what makes the models of each kind
            // (_CHR_INFO::nKind); _CreateRes in the DOL calls the one of the
            // model's kind.
            IEvMdlCreater *m_pCreater[4];
            unsigned char m_Unk44[0x26];
            unsigned short m_nFlag;      // 0x6A (name guessed)
            unsigned char m_Unk6C[0x18];
    };
}

#endif /* !CEVDEMORES_HPP_ */
