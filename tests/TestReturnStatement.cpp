//
//  TestReturnStatement.cpp
//  Yazyk
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

#include "TestFileSampleRunner.hpp"
#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/LocalHeapVariable.hpp"
#include "yazyk/MethodDeclaration.hpp"
#include "yazyk/PrimitiveTypes.hpp"
#include "yazyk/ReturnStatement.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::_;
using ::testing::ExitedWithCode;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

class MockExpression : public IExpression {
public:
  MOCK_CONST_METHOD1(generateIR, Value* (IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, IType* (IRGenerationContext&));
  MOCK_CONST_METHOD1(releaseOwnership, void (IRGenerationContext&));
};

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
    StructType* structType = StructType::create(llvmContext, "Shape");
    structType->setBody(types);
    map<string, ModelField*> fields;
    fields["width"] = new ModelField(PrimitiveTypes::INT_TYPE, 0);
    fields["height"] = new ModelField(PrimitiveTypes::INT_TYPE, 1);
    vector<Method*> methods;
    vector<Interface*> interfaces;
    mModel = new Model("Shape", structType, fields, methods, interfaces);
  }

  ~ReturnStatementTest() {
    delete mStringStream;
  }
};

TEST_F(ReturnStatementTest, ParentFunctionIsNullDeathTest) {
  mContext.setBasicBlock(BasicBlock::Create(mContext.getLLVMContext(), "entry"));
  mContext.getScopes().pushScope();
  ReturnStatement returnStatement(mExpression);

  Mock::AllowLeak(&mExpression);
  EXPECT_EXIT(returnStatement.generateIR(mContext),
              ExitedWithCode(1),
              "No corresponding method found for RETURN");
}

TEST_F(ReturnStatementTest, ParentFunctionIsIncopatableTypeDeathTest) {
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

TEST_F(ReturnStatementTest, ParentFunctionIntTest) {
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

TEST_F(ReturnStatementTest, HeapVariablesAreClearedTest) {
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
  Type* pointerType = Type::getInt32Ty(llvmContext);
  Type* structType = Type::getInt8Ty(llvmContext);
  Constant* allocSize = ConstantExpr::getSizeOf(structType);
  allocSize = ConstantExpr::getTruncOrBitCast(allocSize, pointerType);
  Instruction* malloc = CallInst::CreateMalloc(basicBlock,
                                               pointerType,
                                               structType,
                                               allocSize,
                                               nullptr,
                                               nullptr,
                                               "");

  LocalHeapVariable* variable = new LocalHeapVariable("foo", mModel, malloc);
  mContext.getScopes().setVariable(variable);
  
  ReturnStatement returnStatement(mExpression);
  
  returnStatement.generateIR(mContext);
  
  ASSERT_EQ(3ul, basicBlock->size());
  *mStringStream << *basicBlock;
  string expected = string() +
    "\nentry:"
    "\n  %conv = zext i32 3 to i64" +
    "\n  tail call void @free(i8* %malloccall)"
    "\n  ret i64 %conv\n";
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
  
  basicBlock->getInstList().push_front(malloc);
}

TEST_F(TestFileSampleRunner, ReturnStatementRunTest) {
  runFile("tests/samples/test_return_int.yz", "5");
}

TEST_F(TestFileSampleRunner, ReturnStatementCastTest) {
  runFile("tests/samples/test_return_boolean_cast.yz", "1");
}
