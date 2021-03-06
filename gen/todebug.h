//===-- gen/todebug.h - Symbolic debug information generation ---*- C++ -*-===//
//
//                         LDC – the LLVM D compiler
//
// This file is distributed under the BSD-style LDC license. See the LICENSE
// file for details.
//
//===----------------------------------------------------------------------===//
//
// Handles generation of symbolic debug information using LLVM's DWARF support.
//
//===----------------------------------------------------------------------===//

#ifndef LDC_GEN_TODEBUG_H
#define LDC_GEN_TODEBUG_H

#include "gen/irstate.h"
#include "gen/tollvm.h"


/// \brief Emit the Dwarf compile_unit global for a Module m.
/// \param m        Module to emit as compile unit.
void DtoDwarfCompileUnit(Module* m);

/// \brief Emit the Dwarf subprogram global for a function declaration fd.
/// \param fd       Function declaration to emit as subprogram.
/// \returns        the Dwarf subprogram global.
llvm::DISubprogram DtoDwarfSubProgram(FuncDeclaration* fd);

/// \brief Emit the Dwarf subprogram global for a internal function.
/// This is used for generated functions like moduleinfoctors,
/// module ctors/dtors and unittests.
/// \param prettyname   The name as seen in the source.
/// \param mangledname  The mangled name in the object file.
/// \returns       the Dwarf subprogram global.
llvm::DISubprogram DtoDwarfSubProgramInternal(const char* prettyname, const char* mangledname);

void DtoDwarfFuncStart(FuncDeclaration* fd);
void DtoDwarfFuncEnd(FuncDeclaration* fd);
void DtoDwarfBlockStart(Loc loc);
void DtoDwarfBlockEnd();

void DtoDwarfStopPoint(unsigned ln);

void DtoDwarfValue(LLValue *val, VarDeclaration* vd);

/// \brief Emits all things necessary for making debug info for a local variable vd.
/// \param ll       LLVM Value of the variable.
/// \param vd       Variable declaration to emit debug info for.
/// \param addr     An array of complex address operations.
void DtoDwarfLocalVariable(LLValue* ll, VarDeclaration* vd,
                           llvm::ArrayRef<LLValue*> addr = llvm::ArrayRef<LLValue*>());

/// \brief Emits all things necessary for making debug info for a global variable vd.
/// \param ll       LLVM global variable
/// \param vd       Variable declaration to emit debug info for.
/// \returns        Created debug info
llvm::DIGlobalVariable DtoDwarfGlobalVariable(LLGlobalVariable* ll, VarDeclaration* vd);

void DtoDwarfModuleEnd();

template<typename T>
void dwarfOpOffset(T &addr, LLStructType *type, int index)
{
    if (!global.params.symdebug)
        return;

    uint64_t offset = gDataLayout->getStructLayout(type)->getElementOffset(index);
    LLType *int64Ty = LLType::getInt64Ty(gIR->context());
    addr.push_back(LLConstantInt::get(int64Ty, llvm::DIBuilder::OpPlus));
    addr.push_back(LLConstantInt::get(int64Ty, offset));
}

template<typename T>
void dwarfOpOffset(T &addr, LLValue *val, int index)
{
    if (!global.params.symdebug)
        return;

    LLStructType *type = isaStruct(val->getType()->getContainedType(0));
    assert(type);
    dwarfOpOffset(addr, type, index);
}

template<typename T>
void dwarfOpDeref(T &addr)
{
    if (!global.params.symdebug)
        return;

    LLType *int64Ty = LLType::getInt64Ty(gIR->context());
    addr.push_back(LLConstantInt::get(int64Ty, llvm::DIBuilder::OpDeref));
}

#endif // LDC_GEN_TODEBUG_H
