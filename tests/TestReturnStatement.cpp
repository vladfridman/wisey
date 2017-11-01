//
//  TestReturnStatement.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/14/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReturnStatement}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ReturnStatement.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::ExitedWithCode;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReturnStatementTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpression>* mExpression;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  ReturnStatementTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mExpression(new NiceMock<MockExpression>()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

    Value * value = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(*mExpression, generateIR(_)).WillByDefault(Return(value));

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, "MShape");
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);
    IConcreteObjectType::composeDestructorBody(mContext, mModel);
}

  ~ReturnStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnStatementTest, parentFunctionIsNullDeathTest) {
  mContext.setBasicBlock(BasicBlock::Create(mLLVMContext, "entry"));
  mContext.getScopes().pushScope();
  ReturnStatement returnStatement(mExpression);

  Mock::AllowLeak(mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "No corresponding method found for RETURN");
}

TEST_F(ReturnStatementTest, parentFunctionIsIncopatableTypeDeathTest) {
  FunctionType* functionType = FunctionType::get(Type::getVoidTy(mLLVMContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");

  mContext.setBasicBlock(BasicBlock::Create(mLLVMContext, "entry", function));
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID_TYPE);
  ReturnStatement returnStatement(mExpression);
  
  Mock::AllowLeak(mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'int' to 'void'");
}

TEST_F(ReturnStatementTest, parentFunctionIntTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::LONG_TYPE);
  ReturnStatement returnStatement(mExpression);

  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(2ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected = string() +
    "\nentry:"
    "\n  %conv = zext i32 3 to i64" +
    "\n  ret i64 %conv\n";
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
  
  delete function;
}

TEST_F(ReturnStatementTest, ownerVariablesAreClearedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::LONG_TYPE);

  Type* structType = mModel->getLLVMType(mLLVMContext)->getPointerElementType();
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* fooPointer = IRWriter::newAllocaInst(mContext, fooMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, fooMalloc, fooPointer);
  IVariable* foo = new LocalOwnerVariable("foo", mModel->getOwner(), fooPointer);
  mContext.getScopes().setVariable(foo);

  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  Value* barPointer = IRWriter::newAllocaInst(mContext, barMalloc->getType(), "pointer");
  IRWriter::newStoreInst(mContext, barMalloc, barPointer);
  IVariable* bar = new LocalOwnerVariable("bar", mModel->getOwner(), barPointer);
  mContext.getScopes().setVariable(bar);

  ReturnStatement returnStatement(mExpression);
  
  returnStatement.generateIR(mContext);
  
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr ("
  "%MShape, %MShape* null, i32 1) to i64))"
  "\n  %0 = bitcast i8* %malloccall to %MShape*"
  "\n  %pointer = alloca %MShape*"
  "\n  store %MShape* %0, %MShape** %pointer"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint (%MShape* getelementptr "
  "(%MShape, %MShape* null, i32 1) to i64))"
  "\n  %1 = bitcast i8* %malloccall1 to %MShape*"
  "\n  %pointer2 = alloca %MShape*"
  "\n  store %MShape* %1, %MShape** %pointer2"
  "\n  %conv = zext i32 3 to i64"
  "\n  %2 = load %MShape*, %MShape** %pointer2"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape(%MShape* %2)"
  "\n  %3 = load %MShape*, %MShape** %pointer"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape(%MShape* %3)"
  "\n  ret i64 %conv\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReturnStatementTest, referenceVariablesAreNotClearedTest) {
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(mLLVMContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* basicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::LONG_TYPE);
  
  Type* structType = Type::getInt8Ty(mLLVMContext);
  llvm::Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* fooMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* foo = new LocalReferenceVariable("foo", mModel, fooMalloc);
  mContext.getScopes().setVariable(foo);
  
  mContext.getScopes().pushScope();
  Instruction* barMalloc = IRWriter::createMalloc(mContext, structType, allocSize, "");
  IVariable* bar = new LocalReferenceVariable("bar", mModel, barMalloc);
  mContext.getScopes().setVariable(bar);
  
  ReturnStatement returnStatement(mExpression);
  
  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(4ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected =
  "\nentry:"
  "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %malloccall1 = tail call i8* @malloc(i64 ptrtoint "
  "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
  "\n  %conv = zext i32 3 to i64"
  "\n  ret i64 %conv\n";
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}

TEST_F(TestFileSampleRunner, returnStatementRunTest) {
  runFile("tests/samples/test_return_int.yz", "5");
}

TEST_F(TestFileSampleRunner, returnStatementCastRunTest) {
  runFile("tests/samples/test_return_boolean_cast.yz", "1");
}

TEST_F(TestFileSampleRunner, earlyReturnRunTest) {
  runFile("tests/samples/test_early_return.yz", "1");
}

TEST_F(TestFileSampleRunner, returnReferenceToFreedObjectRunDeathTest) {
  expectFailCompile("tests/samples/test_return_reference_to_freed_object.yz",
          1,
          "Error: Returning a reference to a value that does not exist in caller's scope.");
}
