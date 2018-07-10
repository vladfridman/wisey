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
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"
#include "wisey/ReturnVoidStatement.hpp"

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
  
  Type* structType = mModel->getLLVMType(mContext)->getPointerElementType();
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
  "\ndefine internal i64 @test() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  %pointer = alloca %MShape*"
  "\n  %pointer2 = alloca %MShape*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr (%MShape, %MShape* null, i32 1) to i64))"
  "\n  %0 = bitcast i8* %malloccall to %MShape*"
  "\n  store %MShape* %0, %MShape** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr (%MShape, %MShape* null, i32 1) to i64))"
  "\n  %1 = bitcast i8* %malloccall1 to %MShape*"
  "\n  store %MShape* %1, %MShape** %pointer2"
  "\n  %2 = load %MShape*, %MShape** %pointer2"
  "\n  %3 = bitcast %MShape* %2 to i8*"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MShape.destructor(i8* %3, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %invoke.continue, %entry"
  "\n  %4 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %5 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %4, { i8*, i32 }* %5"
  "\n  %6 = getelementptr { i8*, i32 }, { i8*, i32 }* %5, i32 0, i32 0"
  "\n  %7 = load i8*, i8** %6"
  "\n  %8 = call i8* @__cxa_get_exception_ptr(i8* %7)"
  "\n  %9 = getelementptr i8, i8* %8, i64 8"
  "\n  %10 = load %MShape*, %MShape** %pointer2"
  "\n  %11 = bitcast %MShape* %10 to i8*"
  "\n  call void @systems.vos.wisey.compiler.tests.MShape.destructor(i8* %11, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* %9)"
  "\n  %12 = load %MShape*, %MShape** %pointer"
  "\n  %13 = bitcast %MShape* %12 to i8*"
  "\n  call void @systems.vos.wisey.compiler.tests.MShape.destructor(i8* %13, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* %9)"
  "\n  resume { i8*, i32 } %4"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %14 = load %MShape*, %MShape** %pointer"
  "\n  %15 = bitcast %MShape* %14 to i8*"
  "\n  invoke void @systems.vos.wisey.compiler.tests.MShape.destructor(i8* %15, %wisey.threads.IThread* null, %wisey.threads.CCallStack* null, i8* null)"
  "\n          to label %invoke.continue3 unwind label %cleanup"
  "\n"
  "\ninvoke.continue3:                                 ; preds = %invoke.continue"
  "\n  ret void"
  "\n}\n";

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
  "\ndefine internal i64 @test() {"
  "\ndeclare:"
  "\n  %0 = alloca %MModel*"
  "\n  %1 = alloca %MModel*"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64))"
  "\n  %2 = bitcast i8* %malloccall to %MModel*"
  "\n  store %MModel* %2, %MModel** %0"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (i32* getelementptr (i32, i32* null, i32 1) to i64))"
  "\n  %3 = bitcast i8* %malloccall1 to %MModel*"
  "\n  store %MModel* %3, %MModel** %1"
  "\n  %4 = load %MModel*, %MModel** %1"
  "\n  %5 = icmp eq %MModel* %4, null"
  "\n  br i1 %5, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %6 = load %MModel*, %MModel** %0"
  "\n  %7 = icmp eq %MModel* %6, null"
  "\n  br i1 %7, label %if.end2, label %if.notnull3"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %8 = bitcast %MModel* %4 to i64*"
  "\n  %9 = getelementptr i64, i64* %8, i64 -1"
  "\n  %10 = atomicrmw add i64* %9, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n"
  "\nif.end2:                                          ; preds = %if.notnull3, %if.end"
  "\n  ret void"
  "\n"
  "\nif.notnull3:                                      ; preds = %if.end"
  "\n  %11 = bitcast %MModel* %6 to i64*"
  "\n  %12 = getelementptr i64, i64* %11, i64 -1"
  "\n  %13 = atomicrmw add i64* %12, i64 -1 monotonic"
  "\n  br label %if.end2"
  "\n}\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(TestFileRunner, unreachableReturnVoidRunDeathTest) {
  expectFailCompile("tests/samples/test_unreachable_return_void.yz",
                    1,
                    "tests/samples/test_unreachable_return_void.yz\\(9\\): Error: Statement unreachable");
}
