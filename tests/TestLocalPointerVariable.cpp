//
//  TestLocalPointerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/13/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalPointerVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalPointerVariable.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalPointerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const LLVMPointerType* mPointerType;
  Value* mValueStore;
  LocalPointerVariable* mVariable;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  LocalPointerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
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
    fields.push_back(new FixedField(PrimitiveTypes::INT, "width", 0));
    fields.push_back(new FixedField(PrimitiveTypes::INT, "height", 0));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS,
                             modelFullName,
                             structType,
                             mContext.getImportProfile(),
                             0);
    mModel->setFields(mContext, fields, 1u);

    mPointerType = LLVMPrimitiveTypes::I64->getPointerType(mContext, 0);
    llvm::PointerType* llvmType = mPointerType->getLLVMType(mContext);
    mValueStore = IRWriter::newAllocaInst(mContext, llvmType, "");
    IRWriter::newStoreInst(mContext, ConstantPointerNull::get(llvmType), mValueStore);
    mVariable = new LocalPointerVariable("foo", mPointerType, mValueStore);
  }
  
  ~LocalPointerVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalPointerVariableTest, basicTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mPointerType, mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isSystem());
}

TEST_F(LocalPointerVariableTest, identifierTest) {
  mVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  store i64* null, i64** %0"
  "\n  %1 = load i64*, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalPointerVariableTest, identifierReferenceTest) {
  EXPECT_EQ(mValueStore, mVariable->generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(LocalPointerVariableTest, assignmentTest) {
  llvm::PointerType* modelLLVMType = mModel->getLLVMType(mContext);
  Value* barValue = ConstantPointerNull::get(modelLLVMType);
  ParameterReferenceVariable parameterReferenceVariable("bar", mModel, barValue);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;
  
  mVariable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  store i64* null, i64** %0"
  "\n  %1 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i64*"
  "\n  store i64* %1, i64** %0\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, localPointerVariableRunTest) {
  runFile("tests/samples/test_local_pointer_variable.yz", "5");
}
