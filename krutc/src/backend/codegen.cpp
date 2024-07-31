
#include "codegen.h"

#include <iostream>

#include "llvm/IR/Value.h"
#include "tree.h"

using namespace std;
using namespace llvm;

int CodeGen::codegen() {}

Value *ExprStmt::codegen() {}
Value *ClassStmt::codegen() {}
Value *AttrStmt::codegen() {}
Value *MethodStmt::codegen() {}
Value *FormalStmt::codegen() {}
Value *ForStmt::codegen() {}
Value *IfStmt::codegen() {}
Value *WhileStmt::codegen() {}
Value *BreakStmt::codegen() {}
Value *ContStmt::codegen() {}

/////////////////////////////////////////////////////////////////
//
//
// EXPRESSION CODE GENERATION
//
/////////////////////////////////////////////////////////////////

Value *ReturnExpr::codegen() {}
Value *IntConstExpr::codegen() {}
Value *DeciConstExpr::codegen() {}
Value *StrConstExpr::codegen() {}
Value *CharConstExpr::codegen() {}
Value *BoolConstExpr::codegen() {}
Value *SetConstExpr::codegen() {}
Value *ListConstExpr::codegen() {}
Value *ListElemRef::codegen() {}
Value *SublistExpr::codegen() {}
Value *ObjectIdExpr::codegen() {}
Value *DispatchExpr::codegen() {}
Value *BinopExpr::codegen() {}
Value *NewExpr::codegen() {}
