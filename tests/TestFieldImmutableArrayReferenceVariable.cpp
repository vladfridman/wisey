//
//  TestFieldImmutableArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldImmutableArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/FieldImmutableArrayReferenceVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/ParameterReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct FieldImmutableArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayType* mImmutableArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  FieldImmutableArrayReferenceVariable* mFieldImmutableArrayReferenceVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldImmutableArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<unsigned long> dimensions;
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mImmutableArrayType = mArrayType->getImmutable();
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::INT, 2u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    vector<Type*> types;
    types.push_back(FunctionType::get(Type::getInt32Ty(mLLVMContext), true)
                    ->getPointerTo()->getPointerTo());
    types.push_back(mArrayType->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mImmutableArrayType, "foo", 0));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType,
                                        mContext.getImportProfile(),
                                        0);
    mObject->setFields(mContext, fields, 1u);
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer);
    mContext.getScopes().setVariable(thisVariable);
    
    mFieldImmutableArrayReferenceVariable = new FieldImmutableArrayReferenceVariable("foo",
                                                                                     mObject);
  }
  
  ~FieldImmutableArrayReferenceVariableTest() {
  }
};

TEST_F(FieldImmutableArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldImmutableArrayReferenceVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayType, mFieldImmutableArrayReferenceVariable->getType());
  EXPECT_TRUE(mFieldImmutableArrayReferenceVariable->isField());
  EXPECT_FALSE(mFieldImmutableArrayReferenceVariable->isSystem());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateIdentifierIRTest) {
  mFieldImmutableArrayReferenceVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  mFieldImmutableArrayReferenceVariable->generateIdentifierReferenceIR(mContext);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayType->getLLVMType(mContext);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mImmutableArrayType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mImmutableArrayType));
  mFieldImmutableArrayReferenceVariable->generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i8*"
  "\n  call void @__adjustReferenceCounterForImmutableArray(i8* %2, i64 -1)"
  "\n  %3 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForImmutableArray(i8* %3, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(mFieldImmutableArrayReferenceVariable->generateAssignmentIR(mContext,
                                                                          &mockExpression,
                                                                          arrayIndices,
                                                                          5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'int\\[\\]\\[\\]' to 'immutable int\\[\\]\\'");
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateArrayElementAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  arrayIndices.push_back(&mockExpression);
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(mFieldImmutableArrayReferenceVariable->
              generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceRunTest) {
  runFile("tests/samples/test_field_immutable_array_reference.yz", "7");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_immutable_array_reference_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_immutable_array_reference_rce.yz:15)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_reference_element_assign.yz",
                    1,
                    "tests/samples/test_field_immutable_array_reference_element_assign.yz\\(15\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_reference_cast_from_array_reference.yz",
                    1,
                    "tests/samples/test_field_immutable_array_reference_cast_from_array_reference.yz\\(12\\): "
                    "Error: Incompatible types: can not cast from type 'int\\[\\]\\[\\]' to 'immutable int\\[\\]\\[\\]'");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceCastFromArrayOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_reference_cast_from_array_owner.yz",
                    1,
                    "tests/samples/test_field_immutable_array_reference_cast_from_array_owner.yz\\(12\\): "
                    "Error: Incompatible types: can not cast from type 'int\\[\\]\\[\\]\\*' to 'immutable int\\[\\]\\[\\]'");
}
