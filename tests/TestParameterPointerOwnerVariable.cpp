//
//  TestParameterPointerOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterPointerOwnerVariable}
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
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/ParameterPointerOwnerVariable.hpp"
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

struct ParameterPointerOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  string mStringBuffer;
  Model* mModel;
  raw_string_ostream* mStringStream;
  ParameterPointerOwnerVariable* mVariable;
  Value* mFooValueStore;
  
public:
  
  ParameterPointerOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    vector<Type*> types;
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
    mModel->setFields(fields, 1u);
    
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
   
    const LLVMPointerOwnerType* type = LLVMPrimitiveTypes::I8->getPointerType()->getOwner();
    mFooValueStore = IRWriter::newAllocaInst(mContext, type->getLLVMType(mContext), "foo");
    mVariable = new ParameterPointerOwnerVariable("foo", type, mFooValueStore);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterPointerOwnerVariableTest, basicTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(LLVMPrimitiveTypes::I8->getPointerType()->getOwner(), mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(ParameterPointerOwnerVariableTest, generateIdentifierIRTest) {
  mVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca i8*"
  "\n  %0 = load i8*, i8** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterPointerOwnerVariableTest, freeTest) {
  mVariable->free(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca i8*"
  "\n  %0 = load i8*, i8** %foo"
  "\n  %1 = bitcast i8* %0 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %1)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterPointerOwnerVariableTest, setToNullTest) {
  mVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca i8*"
  "\n  store i8* null, i8** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterPointerOwnerVariableTest, generateIdentifierReferenceIRTest) {
  EXPECT_EQ(mFooValueStore, mVariable->generateIdentifierReferenceIR(mContext));
}

TEST_F(ParameterPointerOwnerVariableTest, assignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  
  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(TestFileSampleRunner, parameterPointerOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_parameter_pointer_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MModel\n"
                                        "free completed\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
