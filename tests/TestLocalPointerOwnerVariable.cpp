//
//  TestLocalPointerOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/28/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalPointerOwnerVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/LocalPointerOwnerVariable.hpp"
#include "wisey/LocalOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalPointerOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  LLVMPointerOwnerType* mPointerOwnerType;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalPointerOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);

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
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "width"));
    fields.push_back(new FixedField(PrimitiveTypes::INT_TYPE, "height"));
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, structType);
    mModel->setFields(fields, 1u);

    mStringStream = new raw_string_ostream(mStringBuffer);
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    mPointerOwnerType = new LLVMPointerOwnerType(LLVMPrimitiveTypes::I64->getPointerType());
  }
  
  ~LocalPointerOwnerVariableTest() {
    delete mStringStream;
  }
};

TEST_F(LocalPointerOwnerVariableTest, basicFieldsTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  
  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mPointerOwnerType, variable->getType());
  EXPECT_FALSE(variable->isField());
  EXPECT_FALSE(variable->isSystem());
}

TEST_F(LocalPointerOwnerVariableTest, generateIdentifierIRTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  variable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = load i64*, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalPointerOwnerVariableTest, generateIdentifierReferenceIRTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);
  
  EXPECT_EQ(fooValue, variable->generateIdentifierReferenceIR(mContext));
}

TEST_F(LocalPointerOwnerVariableTest, freeTest) {
  Type* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IOwnerVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);

  variable->free(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = load i64*, i64** %0"
  "\n  %2 = bitcast i64* %1 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %2)\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalPointerOwnerVariableTest, generateAssignmentIRTest) {
  llvm::PointerType* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IOwnerVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);

  mContext.getScopes().setVariable(variable);
  Value* barValue = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  Value* ownerStore = IRWriter::newAllocaInst(mContext, mModel->getLLVMType(mContext), "");
  LocalOwnerVariable localOwnerVariable("bar", mModel->getOwner(), ownerStore);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;
  
  variable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  %1 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i64*"
  "\n  %3 = load i64*, i64** %0"
  "\n  %4 = bitcast i64* %3 to i8*"
  "\n  call void @__destroyOwnerObjectFunction(i8* %4)"
  "\n  store i64* %2, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalPointerOwnerVariableTest, setToNullTest) {
  llvm::PointerType* llvmType = mPointerOwnerType->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IOwnerVariable* variable = new LocalPointerOwnerVariable("foo", mPointerOwnerType, fooValue);

  variable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %0 = alloca i64*"
  "\n  store i64* null, i64** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileSampleRunner, llvmPointerOwnerVariableRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_llvm_pointer_owner_variable.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MModel\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
