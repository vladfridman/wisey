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
#include "TestFileSampleRunner.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/LocalReferenceVariable.hpp"
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

struct LocalReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBlock;
  Model* mModel;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
 
public:
  
  LocalReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
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
    vector<Field*> fields;
    ExpressionList fieldArguments;
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "width", fieldArguments));
    fields.push_back(new Field(FIXED_FIELD, PrimitiveTypes::INT_TYPE, "height", fieldArguments));
    mModel = Model::newModel(modelFullName, structType);
    mModel->setFields(fields, 1u);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(LocalReferenceVariableTest, heapReferenceVariableAssignmentTest) {
  Type* llvmType = mModel->getLLVMType(mLLVMContext);
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  
  IVariable* uninitializedHeapVariable = new LocalReferenceVariable("foo", mModel, fooValue);
  mContext.getScopes().setVariable(uninitializedHeapVariable);
  Value* barValue = ConstantPointerNull::get((PointerType*) llvmType);
  LocalReferenceVariable heapReferenceVariable("bar", mModel, NULL);
  NiceMock<MockExpression> expression;
  ON_CALL(expression, getType(_)).WillByDefault(Return(mModel));
  ON_CALL(expression, generateIR(_)).WillByDefault(Return(barValue));
  
  uninitializedHeapVariable->generateAssignmentIR(mContext, &expression);

  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape**"
  "\n  store %systems.vos.wisey.compiler.tests.MShape** null, "
    "%systems.vos.wisey.compiler.tests.MShape*** %0\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, heapReferenceVariableIdentifierTest) {
  Type* llvmType = mModel->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable heapReferenceVariable("foo", mModel, fooValue);

  heapReferenceVariable.setToNull(mContext);
  heapReferenceVariable.generateIdentifierIR(mContext, "fooVal");

  *mStringStream << *mBlock;
  string expected =
  "\nentry:"
  "\n  %0 = alloca %systems.vos.wisey.compiler.tests.MShape**"
  "\n  store %systems.vos.wisey.compiler.tests.MShape** null, "
  "%systems.vos.wisey.compiler.tests.MShape*** %0"
  "\n  %1 = load %systems.vos.wisey.compiler.tests.MShape**, "
    "%systems.vos.wisey.compiler.tests.MShape*** %0\n";
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalReferenceVariableTest, heapReferenceVariableIdentifierUninitializedDeathTest) {
  Type* llvmType = mModel->getOwner()->getLLVMType(mContext.getLLVMContext());
  Value* fooValue = IRWriter::newAllocaInst(mContext, llvmType, "");
  LocalReferenceVariable heapReferenceVariable("foo", mModel, fooValue);
  
  EXPECT_EXIT(heapReferenceVariable.generateIdentifierIR(mContext, "fooVal"),
              ::testing::ExitedWithCode(1),
              "Error: Variable 'foo' is used before it is initialized");
}

TEST_F(LocalReferenceVariableTest, existsInOuterScopeTest) {
  LocalReferenceVariable heapReferenceVariable("foo", mModel, NULL);

  EXPECT_FALSE(heapReferenceVariable.existsInOuterScope());
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