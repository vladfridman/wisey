//
//  TestLocalReferenceVariable.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalReferenceVariable}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockVariable.hpp"
#include "TestPrefix.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/FixedField.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
#include "wisey/Names.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ProgramPrefix.hpp"
#include "wisey/ThreadExpression.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  NiceMock<MockVariable>* mThreadVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
 
public:
  
  LocalReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    ProgramPrefix programPrefix;
    programPrefix.generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBlock);
    mContext.getScopes().pushScope();

    vector<Type*> types;
    LLVMContext& llvmContext = mContext.getLLVMContext();
    types.push_back(Type::getInt64Ty(mLLVMContext));
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
    
    Interface* threadInterface = mContext.getInterface(Names::getThreadInterfaceFullName());
    Value* threadObject = ConstantPointerNull::get(threadInterface->getLLVMType(mContext));
    mThreadVariable = new NiceMock<MockVariable>();
    ON_CALL(*mThreadVariable, getName()).WillByDefault(Return(ThreadExpression::THREAD));
    ON_CALL(*mThreadVariable, getType()).WillByDefault(Return(threadInterface));
    ON_CALL(*mThreadVariable, generateIdentifierIR(_)).WillByDefault(Return(threadObject));
    mContext.getScopes().setVariable(mThreadVariable);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
  
  ~LocalReferenceVariableTest() {
    delete mThreadVariable;
  }
};

TEST_F(LocalReferenceVariableTest, basicFieldsTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  IVariable* variable = new LocalReferenceVariable("foo", mModel, fooValue);

  EXPECT_STREQ("foo", variable->getName().c_str());
  EXPECT_EQ(mModel, variable->getType());
  EXPECT_FALSE(variable->isSystem());
}

TEST_F(LocalReferenceVariableTest, localReferenceVariableAssignmentTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  
  IVariable* uninitializedHeapVariable = new LocalReferenceVariable("foo", mModel, fooValue);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((PointerType*) llvmType);
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("bar", mModel, referenceStore);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_, _)).WillByDefault(Return(barValue));
  vector<const IExpression*> arrayIndices;
  
  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression, arrayIndices, 0);

  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MShape* %2 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %3, i64 -1)"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MShape* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.MShape* null, "
  "%systems.vos.wisey.compiler.tests.MShape** %0\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, localReferenceVariableIdentifierTest) {
  PointerType* llvmType = mModel->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValue);
  llvm::Constant* null = ConstantPointerNull::get(llvmType);
  FakeExpression* fakeExpression = new FakeExpression(null, mModel);
  vector<const IExpression*> arrayIndices;
  localReferenceVariable.generateAssignmentIR(mContext, fakeExpression, arrayIndices, 0);

  Value* instruction = localReferenceVariable.generateIdentifierIR(mContext);

  *mStringStream << *instruction;
  string expected =
  "  %4 = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** %0";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, decrementReferenceCounterTest) {
  Type* llvmType = mModel->getLLVMType(mContext);
  
  Value* referenceStore = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("bar", mModel, referenceStore);
  
  localReferenceVariable.decrementReferenceCounter(mContext);
  
  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape*"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape*, "
  "%systems.vos.wisey.compiler.tests.MShape** %0"
  "\n  %2 = bitcast %systems.vos.wisey.compiler.tests.MShape* %1 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %2, i64 -1)\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, localReferenceVariableIdentifierUninitializedDeathTest) {
  Mock::AllowLeak(mThreadVariable);
  
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable localReferenceVariable("foo", mModel, fooValue);
  
  EXPECT_EXIT(localReferenceVariable.generateIdentifierIR(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is used before it is initialized");
}

TEST_F(TestFileSampleRunner, headReferenceVariableAssignmentRunTest) {
  runFile("tests/samples/test_assignment_model_variable.yz", "0");
}

TEST_F(TestFileSampleRunner, interfaceVariableAssignmentRunTest) {
  runFile("tests/samples/test_interface_variable_assignment.yz", "25");
}

TEST_F(TestFileSampleRunner, assignLocalReferenceToFieldOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_field_owner.yz");
}

TEST_F(TestFileSampleRunner, assignLocalReferenceToFieldReferenceCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_field_reference.yz");
}

TEST_F(TestFileSampleRunner, assignLocalReferenceToLocalOwnerCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_local_owner.yz");
}

TEST_F(TestFileSampleRunner, assignLocalReferenceToLocalReferenceCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_local_reference.yz");
}

TEST_F(TestFileSampleRunner, assignLocalReferenceToNullCompileTest) {
  compileFile("tests/samples/test_assign_local_reference_to_null.yz");
}

TEST_F(TestFileSampleRunner, usingUninitializedLocalReferenceVariableRunDeathTest) {
  expectFailCompile("tests/samples/test_heap_reference_variable_not_initialized.yz",
                    1,
                    "Error: Variable 'color' is used before it is initialized");
}

TEST_F(TestFileSampleRunner, incompatableHeapVariableTypesInAssignmentRunDeathTest) {
  expectFailCompile("tests/samples/test_incompatible_heap_variable_types_in_assignment.yz",
                    1,
                    "Error: Incompatible types: can not cast from type "
                    "'systems.vos.wisey.compiler.tests.MShape\\*' to "
                    "'systems.vos.wisey.compiler.tests.MColor\\*'");
}
