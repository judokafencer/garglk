// Main header for git
// $Id: git.h,v 1.32 2004/12/22 12:40:07 iain Exp $

#ifndef GIT_H
#define GIT_H

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <glk.h>

#include "version.h"
#include "config.h"

// Version number formatting

#define GIT_VERSION_NUM (GIT_MAJOR << 16) \
                      | (GIT_MINOR << 8)  \
                      | (GIT_PATCH)

#define _str2(s) #s
#define _str(s) _str2(s)

#if GIT_PATCH == 0
#define GIT_VERSION_STR \
_str(GIT_MAJOR) "." _str(GIT_MINOR)
#else
#define GIT_VERSION_STR \
_str(GIT_MAJOR) "." _str(GIT_MINOR) "." _str(GIT_PATCH)
#endif

// git.c

extern void gitWithStream (strid_t stream,
                           git_uint32 cacheSize,
                           git_uint32 undoSize);

extern void git (const git_uint8 * game,
                 git_uint32 gameSize,
                 git_uint32 cacheSize,
                 git_uint32 undoSize);

extern void fatalError (const char *);

// memory.c

#include "memory.h"

// gestalt.c

enum IOMode
{
    IO_NULL   = 0,
    IO_FILTER = 1,
    IO_GLK    = 2,
    IO_MAX
};

enum GestaltSelector
{
    GESTALT_SPEC_VERSION = 0,
    GESTALT_TERP_VERSION = 1,
    GESTALT_RESIZEMEM    = 2,
    GESTALT_UNDO         = 3,
    GESTALT_IO_SYSTEM    = 4,
    GESTALT_UNICODE      = 5,
    GESTALT_MEM_COPY     = 6,
    GESTALT_MALLOC       = 7,
    GESTALT_MALLOC_HEAP  = 8,
    
    // This special selector returns 1 if the cache control
    // opcodes 'git_setcacheram' and 'git_prunecache' are available.
    
    GESTALT_GIT_CACHE_CONTROL = 0x7940
};

extern git_uint32 gestalt (enum GestaltSelector sel, git_uint32 param);

// opcodes.c

extern void parseInstruction (git_uint32 * pc, int * done);

// operand.c

typedef enum { reg_L1, reg_L2, reg_L3, reg_L4, reg_L5, reg_L6, reg_L7 } LoadReg;
typedef enum { reg_S1, reg_S2 } StoreReg;
typedef enum { size32, size16, size8 } TransferSize;

extern git_uint32 parseLoad  (git_uint32 * pc, LoadReg reg, int mode, TransferSize, git_sint32 * constVal);
extern void       parseStore (git_uint32 * pc, StoreReg reg, int mode, TransferSize);

extern void parseCallStub  (git_uint32 * pc, int mode);
extern void parseCatchStub (git_uint32 * pc, int mode);
extern void parseSaveStub  (git_uint32 * pc, int mode);
extern void parseUndoStub  (git_uint32 * pc, int mode);

// compiler.c

#include "compiler.h"

// peephole.c

extern void resetPeepholeOptimiser();
extern void emitCode (Label);

// terp.c

#ifdef USE_DIRECT_THREADING
    extern Opcode* gOpcodeTable;
#   define labelToOpcode(label) (gOpcodeTable[label])
#else
#   define labelToOpcode(label) label
#endif

extern git_sint32* gStackPointer;

extern void startProgram (size_t cacheSize, enum IOMode ioMode);

// glkop.c

extern int git_init_dispatch();
extern glui32 git_perform_glk(glui32 funcnum, glui32 numargs, glui32 *arglist);
extern strid_t git_find_stream_by_id(glui32 id);

// git_search.c

extern glui32 git_binary_search(glui32 key, glui32 keysize, 
  glui32 start, glui32 structsize, glui32 numstructs, 
  glui32 keyoffset, glui32 options);

extern glui32 git_linked_search(glui32 key, glui32 keysize, 
  glui32 start, glui32 keyoffset, glui32 nextoffset, glui32 options);

extern glui32 git_linear_search(glui32 key, glui32 keysize, 
  glui32 start, glui32 structsize, glui32 numstructs, 
  glui32 keyoffset, glui32 options);

// savefile.c

extern git_sint32 saveToFile (git_sint32* base, git_sint32 * sp, git_sint32 file);
extern git_sint32 restoreFromFile (git_sint32* base, git_sint32 file,
                      git_uint32 protectPos, git_uint32 protectSize);

// saveundo.c

extern void initUndo (git_uint32 size);
extern void resetUndo ();
extern void shutdownUndo ();

extern int  saveUndo (git_sint32* base, git_sint32* sp);
extern int  restoreUndo (git_sint32* base,
                git_uint32 protectPos, git_uint32 protectSize);

#endif // GIT_H