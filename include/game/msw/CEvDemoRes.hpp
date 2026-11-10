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
                // The model's shadow size: a table entry chosen by bits 3-5
                // of the byte at 0x16.
                short GetShadowSize() const;

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
                unsigned char nFlag;     // 0x17
            };

            // The three functions the class's own vtable holds (DOL).
            virtual ~CEvDemoRes();
            virtual void Main();
            virtual void Disp();

            static CEvDemoRes *GetInstance();

            // (name guessed) The half-word at 0x6A.
            unsigned short GetFlag() const { return m_nFlag; }
            // (name guessed) The pointer at 0x4C: the demo's name, the file
            // name under "event/demos" the objects are loaded from.
            const char *GetDemoName() const { return m_pDemoName; }

        private:
            // 0x34 (name guessed): what makes the models of each kind
            // (_CHR_INFO::nKind); _CreateRes in the DOL calls the one of the
            // model's kind.
            IEvMdlCreater *m_pCreater[4];
            unsigned char m_Unk44[0x08];
            const char *m_pDemoName;     // 0x4C (name guessed)
            unsigned char m_Unk50[0x1A];
            unsigned short m_nFlag;      // 0x6A (name guessed)
            unsigned char m_Unk6C[0x18];
    };
}

#endif /* !CEVDEMORES_HPP_ */
