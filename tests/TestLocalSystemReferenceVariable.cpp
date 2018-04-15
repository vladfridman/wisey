//
//  TestLocalSystemReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalSystemReferenceVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "TestFileRunner.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalSystemReferenceVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalSystemReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalSystemReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(llvmContext));
    types.push_back(Type::getInt32Ty(llvmContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(llvmContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 1u);
    
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName(), 0);
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalSystemReferenceVariableTest() {
    delete mThreadVariable;
  }
};

TEST_F(LocalSystemReferenceVariableTest, basicFieldsTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalSystemReferenceVariable("foo", mModel, fooValue);
  
  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mModel, variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_TRUE(variable->isSystem());
}

TEST_F(LocalSystemReferenceVariableTest, assignmentTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  
  IVariable* uninitializedHeapVariable = new LocalSystemReferenceVariable("foo", mModel, fooValue);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((llvm::PointerType*) llvmType);
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalSystemReferenceVariable variable("bar", mModel, referenceStore);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;
  
  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, %systems.vos.wisey.compiler.tests.MShape** %0\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalSystemReferenceVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalSystemReferenceVariable variable("foo", mModel, fooValue);
  llvm::Constant* null = ConstantPointerNull::get(llvmType);
  FakeExpression* fakeExpression = new FakeExpression(null, mModel);
  vector<const IExpression*> arrayIndices;
  variable.generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);
  
  Value* instruction = variable.generateIdentifierIR(mContext);
  
  *mStringStream << *instruction;
  string expected =
  "  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalSystemReferenceVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* llvmType = mModel->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalSystemReferenceVariable variable("foo", mModel, fooValueStore);
  
  EXPECT_EQ(fooValueStore, variable.generateIdentifierReferenceIR(mContext));
}

TEST_F(LocalSystemReferenceVariableTest, decrementReferenceCounterTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalSystemReferenceVariable variable("bar", mModel, referenceStore);
  
  variable.decrementReferenceCounter(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MShape* %1 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %2, i64 -1)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalSystemReferenceVariableTest, generateIdentifierUninitializedDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalSystemReferenceVariable variable("foo", mModel, fooValue);
  
  EXPECT_EXIT(variable.generateIdentifierIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: System variable 'foo' is not initialized");
}
