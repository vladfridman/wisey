//
//  TestReturnVoidStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReturnVoidStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "LocalOwnerVariable.hpp"
#include "LocalReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "ReceivedField.hpp"
#include "ReturnVoidStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::Return;
using ::testing::Test;

struct ReturnVoidStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

  ReturnVoidStatementTest() :
  mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, "MShape");
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new ReceivedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    IConcreteObjectType::declareTypeNameGlobal(mContext, mModel);
    IConcreteObjectType::defineVTable(mContext, mModel);
  }
  
  ~ReturnVoidStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnVoidStatementTest, returnVoidTest) {
  BasicBlock* entryBlock = BasicBlock::Create(mLLVMContext, "entry");
  BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare");
  mContext.setBasicBlock(entryBlock);
  mContext.setDeclarationsBlock(declareBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID);
  
  ReturnVoidStatement returnVoidStatement(0);
  returnVoidStatement.generateIR(mContext);
  
  ASSERT_EQ(1ul, entryBlock->size());
  *mStringStream << entryBlock->front();
  ASSERT_STREQ(mStringStream->str().c_str(), "  ret void");
}

TEST_F(ReturnVoidStatementTest, ownerVariablesAreClearedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
  BasicBlock* entryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(entryBlock);
  mContext.setDeclarationsBlock(declareBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID);
  
  Type* structType = mModel->getLLVMStructType(mContext);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 1);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new LocalOwnerVariable("foo", mModel->getOwner(), fooPointer, 0);
  mContext.getScopes().setVariable(mContext, foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new LocalOwnerVariable("bar", mModel->getOwner(), barPointer, 0);
  mContext.getScopes().setVariable(mContext, bar);
  
  ReturnVoidStatement returnStatement(0);
  
  returnStatement.generateIR(mContext);
  BranchInst::Create(entryBlock, declareBlock);

  *mStringStream << *function;
  string expected =
  "define internal i64 @test() personality ptr @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %pointer = alloca ptr, align 8"
  "\n  %pointer2 = alloca ptr, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MShape, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall, ptr %pointer, align 8"
  "\n  %malloccall1 = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MShape, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall1, ptr %pointer2, align 8"
  "\n  %0 = load ptr, ptr %pointer2, align 8"
  "\n  %1 = bitcast ptr %0 to ptr"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MShape.destructor(ptr %1, ptr null, ptr null, ptr null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %2 = landingpad { ptr, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { ptr, i32 }, align 8"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { ptr, i32 } %2, ptr %3, align 8"
  "\n  %4 = getelementptr { ptr, i32 }, ptr %3, i32 0, i32 0"
  "\n  %5 = load ptr, ptr %4, align 8"
  "\n  %6 = call ptr @__cxa_get_exception_ptr(ptr %5)"
  "\n  %7 = getelementptr i8, ptr %6, i64 8"
  "\n  %8 = load ptr, ptr %pointer2, align 8"
  "\n  %9 = bitcast ptr %8 to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MShape.destructor(ptr %9, ptr null, ptr null, ptr %7)"
  "\n  %10 = load ptr, ptr %pointer, align 8"
  "\n  %11 = bitcast ptr %10 to ptr"
  "\n  call void @systems.vos.wisey.compiler.tests.MShape.destructor(ptr %11, ptr null, ptr null, ptr %7)"
  "\n  resume { ptr, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %12 = load ptr, ptr %pointer, align 8"
  "\n  %13 = bitcast ptr %12 to ptr"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MShape.destructor(ptr %13, ptr null, ptr null, ptr null)"
  "\n          to label %invoke.continue3 unwind label %cleanup"
  "\n"
  "\ninvoke.continue3:                                 ; preds = %invoke.continue"
  "\n  ret void"
  "\n}"
  "\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReturnVoidStatementTest, referenceVariablesGetTheirRefCountDecrementedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
  BasicBlock* entryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setDeclarationsBlock(declareBlock);
  mContext.setBasicBlock(entryBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID);
  
  StructType* structType = StructType::create(mLLVMContext, "MModel");
  vector<Type*> types;
  types.push_back(Type::getInt32Ty(mLLVMContext));
  structType->setBody(types);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  llvm::Constant* one = ConstantInt::get(Type::getInt64Ty(mLLVMContext), 1);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* fooStore = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, fooMalloc, fooStore);
  IVariable* foo = new LocalReferenceVariable("foo", mModel, fooStore, 0);
  mContext.getScopes().setVariable(mContext, foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, one, "");
  Value* barStore = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "");
  IRWriter::newStoreInst(mContext, barMalloc, barStore);
  IVariable* bar = new LocalReferenceVariable("bar", mModel, barStore, 0);
  mContext.getScopes().setVariable(mContext, bar);

  ReturnVoidStatement returnStatement(0);
  
  returnStatement.generateIR(mContext);
  BranchInst::Create(entryBlock, declareBlock);

  *mStringStream << *function;
  
  string expected =
  "define internal i64 @test() {"
  "\ndeclare:"
  "\n  %0 = alloca ptr, align 8"
  "\n  %1 = alloca ptr, align 8"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MModel, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall, ptr %0, align 8"
  "\n  %malloccall1 = tail call ptr @malloc(i64 ptrtoint (ptr getelementptr (%MModel, ptr null, i32 1) to i64))"
  "\n  store ptr %malloccall1, ptr %1, align 8"
  "\n  %2 = load ptr, ptr %1, align 8"
  "\n  %3 = icmp eq ptr %2, null"
  "\n  br i1 %3, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %4 = load ptr, ptr %0, align 8"
  "\n  %5 = icmp eq ptr %4, null"
  "\n  br i1 %5, label %if.end2, label %if.notnull3"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %6 = bitcast ptr %2 to ptr"
  "\n  %7 = getelementptr i64, ptr %6, i64 -1"
  "\n  %8 = atomicrmw add ptr %7, i64 -1 monotonic, align 8"
  "\n  br label %if.end"
  "\n"
  "\nif.end2:                                          ; preds = %if.notnull3, %if.end"
  "\n  ret void"
  "\n"
  "\nif.notnull3:                                      ; preds = %if.end"
  "\n  %9 = bitcast ptr %4 to ptr"
  "\n  %10 = getelementptr i64, ptr %9, i64 -1"
  "\n  %11 = atomicrmw add ptr %10, i64 -1 monotonic, align 8"
  "\n  br label %if.end2"
  "\n}"
  "\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, unreachableReturnVoidRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_return_void.yz",
                    1,
                    "tests/samples/test_unreachable_return_void.yz\\(9\\): Error: Statement unreachable");
}

TEST_F(TestFileRunner, returnVoidErrorRunDeathTest) {
  expectFailCompile("tests/samples/test_returnvoid_error.yz",
                    1,
                    "tests/samples/test_returnvoid_error.yz\\(9\\): Error: Must return a value of type int");
}
