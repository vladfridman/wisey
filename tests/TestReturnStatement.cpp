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
#include "wisey/LocalHeapVariable.hpp"
#include "wisey/MethodDeclaration.hpp"
#include "wisey/PrimitiveTypes.hpp"
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
  NiceMock<MockExpression> mExpression;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  ReturnStatementTest() {
    Value * value = ConstantInt::get(Type::getInt32Ty(mContext.getLLVMContext()), 3);
    ON_CALL(mExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    ON_CALL(mExpression, generateIR(_)).WillByDefault(Return(value));

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, "MShape");
    structType->setBody(types);
    map<string, Field*> fields;
    ExpressionList fieldArguments;
    fields["width"] = new Field(PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments);
    fields["height"] = new Field(PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("MShape", modelFullName, structType, fields, methods, interfaces);
  }

  ~ReturnStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnStatementTest, parentFunctionIsNullDeathTest) {
  mContext.setBasicBlock(BasicBlock::Create(mContext.getLLVMContext(), "entry"));
  mContext.getScopes().pushScope();
  ReturnStatement returnStatement(mExpression);

  Mock::AllowLeak(&mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "No corresponding method found for RETURN");
}

TEST_F(ReturnStatementTest, parentFunctionIsIncopatableTypeDeathTest) {
  LLVMContext &llvmContext = mContext.getLLVMContext();

  FunctionType* functionType = FunctionType::get(Type::getVoidTy(llvmContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");

  mContext.setBasicBlock(BasicBlock::Create(llvmContext, "entry", function));
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::VOID_TYPE);
  ReturnStatement returnStatement(mExpression);
  
  Mock::AllowLeak(&mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "Error: Incopatible types: can not cast from type 'int' to 'void'");
}

TEST_F(ReturnStatementTest, parentFunctionIntTest) {
  LLVMContext &llvmContext = mContext.getLLVMContext();
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(llvmContext), false);
  Function* function = Function::Create(functionType, GlobalValue::InternalLinkage, "test");
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
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

TEST_F(ReturnStatementTest, heapVariablesAreClearedTest) {
  LLVMContext &llvmContext = mContext.getLLVMContext();
  FunctionType* functionType = FunctionType::get(Type::getInt64Ty(llvmContext), false);
  Function* function = Function::Create(functionType,
                                        GlobalValue::InternalLinkage,
                                        "test",
                                        mContext.getModule());
  BasicBlock* basicBlock = BasicBlock::Create(llvmContext, "entry", function);
  mContext.setBasicBlock(basicBlock);
  mContext.getScopes().pushScope();
  mContext.getScopes().setReturnType(PrimitiveTypes::LONG_TYPE);

  Type* structType = Type::getInt8Ty(llvmContext);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  Instruction* malloc = IRWriter::createMalloc(mContext, structType, allocSize, "");

  LocalHeapVariable* variable = new LocalHeapVariable("foo", mModel, malloc);
  mContext.getScopes().setVariable(variable);
  
  ReturnStatement returnStatement(mExpression);
  
  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(4ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected =
    "\nentry:"
    "\n  %malloccall = tail call i8* @malloc(i64 ptrtoint "
      "(i8* getelementptr (i8, i8* null, i32 1) to i64))"
    "\n  %conv = zext i32 3 to i64"
    "\n  tail call void @free(i8* %malloccall)"
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
