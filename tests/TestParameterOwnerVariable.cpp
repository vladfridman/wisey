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
  BasicBlock* mBlock;
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
    types.push_back(Type::getInt32Ty(mLLVMContext));
    types.push_back(Type::getInt32Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MShape";
    StructType* structType = StructType::create(mLLVMContext, modelFullName);
    structType->setBody(types);
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);

    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);

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

TEST_F(ParameterOwnerVariableTest, freeTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable heapMethodParameter("foo", mModel->getOwner(), fooValue);
  
  heapMethodParameter.free(mContext);
  
  *mStringStream << *mBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MShape("
    "%systems.vos.wisey.compiler.tests.MShape* %1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterOwnerVariableTest, setToNullTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  ParameterOwnerVariable heapMethodParameter("foo", mModel->getOwner(), fooValue);
  
  heapMethodParameter.setToNull(mContext);
  
  *mStringStream << *mBlock;
  
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
