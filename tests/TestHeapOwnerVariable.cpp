//
//  TestHeapOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link HeapOwnerVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/HeapOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct HeapOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  HeapOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", 0, fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", 1, fieldArguments));
    mModel = new Model(modelFullName, structType);
    mModel->setFields(fields);
    
    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);
    IConcreteObjectType::composeDestructorBody(mContext, mModel);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(HeapOwnerVariableTest, heapVariableAssignmentTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");

  IVariable* uninitializedHeapVariable =
    new HeapOwnerVariable("foo", mModel->getOwner(), fooValue);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((PointerType*) llvmType->getPointerTo());
  HeapOwnerVariable heapOwnerVariable("bar", mModel->getOwner(), NULL);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(barValue));
  
  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %ownerToFree = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MShape* %ownerToFree to i8*"
  "\n  call void @__freeIfNotNull(i8* %1)"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** null"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* %2, "
  "%systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(HeapOwnerVariableTest, heapOwnerVariableIdentifierTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  HeapOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue);
  
  heapOwnerVariable.setToNull(mContext);

  EXPECT_EQ(heapOwnerVariable.generateIdentifierIR(mContext, "fooVal"), fooValue);
}

TEST_F(HeapOwnerVariableTest, heapOwnerVariableIdentifierUninitializedDeathTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  HeapOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue);
  
  EXPECT_EXIT(heapOwnerVariable.generateIdentifierIR(mContext, "fooVal"),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is used before it is initialized");
}

TEST_F(HeapOwnerVariableTest, existsInOuterScopeTest) {
  Value* fooValue = ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mLLVMContext));
  HeapOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue);
  
  EXPECT_FALSE(heapOwnerVariable.existsInOuterScope());
}

TEST_F(HeapOwnerVariableTest, freeTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  HeapOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue);
  
  heapOwnerVariable.free(mContext);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape("
  "%systems.vos.wisey.compiler.tests.MShape** %0)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(HeapOwnerVariableTest, setToNullTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  HeapOwnerVariable heapOwnerVariable("foo", mModel->getOwner(), fooValue);
  
  heapOwnerVariable.setToNull(mContext);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, "
  "%systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
  
}

TEST_F(TestFileSampleRunner, usingUninitializedHeapOwnerVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_heap_owner_variable_not_initialized.yz",
                    1,
                    "Error: Variable 'color' is used before it is initialized");
}
