#ifndef CLOADDATA_HPP_
#define CLOADDATA_HPP_

#include "nw4r/g3d/ResFile.hpp"

namespace msw {

    // Only the members used so far.
    class CLoadData {
        public:
            // Data of entry `index` in the game's table of files.
            static void *GetPtr(long index); // return type guessed
            // Its path: the start of the entry (entries are 0x110 bytes).
            static const char *GetFilePath(long index);
            static unsigned long GetSize(long index); // return type guessed
            // Non-zero once the entry has been read.
            static int IsComplete(long index); // return type guessed
            // (return type guessed) Non-zero once entry `index` is loaded;
            // msw::SelectMenu::CCupChr::Initialize tests it.
            static int IsLoaded(long index);
            // Added for MenuF game/menu/CMenu.cpp: start loading the menu's
            // files, reset the table, and free what was loaded.
            static void LoadMenu();
            static void InitData();
            static void UnLoad();

            // The ones below are static in the DOL too: their bodies index
            // the table of files with the first argument.

            // Entry `index` as a model resource file (it passes the entry
            // on to bcs::CLoadMan::GetRes). The callers keep the result in a
            // nw4r::g3d::ResFile; whether that or the bare pointer is the
            // declared return type cannot be told from the code.
            static nw4r::g3d::ResFile GetRes(long index);
            // A character's model file inside archive `index`. (meaning of
            // the last argument not known: the callers pass the high byte of
            // msw::CEvDemoRes::_CHR_INFO's word at 0x08)
            static nw4r::g3d::ResFile GetChrMdlRes(long index, long nChr, long);
            // A character's motion file inside archive `index`.
            static nw4r::g3d::ResFile GetChrMotRes(long index, long nChr);
            // Makes entry `index` the event motion file of character `nChr`
            // and returns `index`. With bShare set it first looks for an
            // entry that already is that file and returns that one instead.
            // The DOL body does not read the third argument. (parameter
            // names guessed)
            static long LoadEventMotion(long index, long nChr, long, int bShare);
            // Makes entry `index` a file of the event data under pDir/pName
            // and returns `index`, or with bShare set the entry that already
            // has it. (the two numbers select the file; their meaning and all
            // parameter names are guessed)
            static long LoadEventData(long index, const char *pDir, const char *pName, long nNo, int bShare, long nKind);
    };
}

#endif /* !CLOADDATA_HPP_ */
