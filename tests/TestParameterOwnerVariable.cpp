//
//  TestParameterOwnerVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterOwnerVariable.hpp"
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

struct ParameterOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    vector<Type*> types;
    types.push_back(Type::getInt64Ty(mLLVMContext));
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width"));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height"));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 2u);

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterOwnerVariableTest, basicFieldsTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable variable("foo", mModel->getOwner(), fooValueStore);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mModel->getOwner(), variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(ParameterOwnerVariableTest, generateIdentifierIRTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable variable("foo", mModel->getOwner(), fooValueStore);
  
  variable.generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterOwnerVariableTest, generateIdentifierReferenceIRTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable variable("foo", mModel->getOwner(), fooValueStore);
  
  EXPECT_EQ(fooValueStore, variable.generateIdentifierReferenceIR(mContext));
}

TEST_F(ParameterOwnerVariableTest, freeTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable heapMethodParameter("foo", mModel->getOwner(), fooValue);
  
  heapMethodParameter.free(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, %systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MShape* %1 to i8*"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape(i8* %2)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterOwnerVariableTest, setToNullTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable heapMethodParameter("foo", mModel->getOwner(), fooValue);
  
  heapMethodParameter.setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, "
  "%systems.vos.wisey.compiler.tests.MShape** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, assignParameterOwnerToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_parameter_owner_to_local_owner.yz");
}

TEST_F(TestFileSampleRunner, assignParameterOwnerToNullCompileTest) {
  compileFile("tests/samples/test_assign_parameter_owner_to_null.yz");
}
