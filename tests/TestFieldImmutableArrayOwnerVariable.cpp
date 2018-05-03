//
//  TestFieldImmutableArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link FieldImmutableArrayOwnerVariable}
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
#include "wisey/FieldImmutableArrayOwnerVariable.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
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

struct FieldImmutableArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
  FieldImmutableArrayOwnerVariable* mFieldImmutableArrayOwnerVariable;
  Controller* mObject;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  FieldImmutableArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);
    mImmutableArrayOwnerType = mArrayType->getImmutable()->getOwner();
    
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
    types.push_back(mArrayType->getOwner()->getLLVMType(mContext));
    string objectFullName = "systems.vos.wisey.compiler.tests.CObject";
    StructType* objectStructType = StructType::create(mLLVMContext, objectFullName);
    objectStructType->setBody(types);
    vector<IField*> fields;
    fields.push_back(new StateField(mImmutableArrayOwnerType, "foo", 0));
    mObject = Controller::newController(AccessLevel::PUBLIC_ACCESS,
                                        objectFullName,
                                        objectStructType,
                                        mContext.getImportProfile(),
                                        0);
    mObject->setFields(mContext, fields, 1u);
    
    Value* thisPointer = ConstantPointerNull::get(mObject->getLLVMType(mContext));
    IVariable* thisVariable = new ParameterReferenceVariable(IObjectType::THIS,
                                                             mObject,
                                                             thisPointer,
                                                             0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldImmutableArrayOwnerVariable = new FieldImmutableArrayOwnerVariable("foo", mObject, 0);
  }
  
  ~FieldImmutableArrayOwnerVariableTest() {
  }
};

TEST_F(FieldImmutableArrayOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldImmutableArrayOwnerVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayOwnerType, mFieldImmutableArrayOwnerVariable->getType());
  EXPECT_TRUE(mFieldImmutableArrayOwnerVariable->isField());
  EXPECT_FALSE(mFieldImmutableArrayOwnerVariable->isSystem());
}

TEST_F(FieldImmutableArrayOwnerVariableTest, generateIdentifierIRTest) {
  mFieldImmutableArrayOwnerVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  mFieldImmutableArrayOwnerVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayOwnerVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mImmutableArrayOwnerType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mImmutableArrayOwnerType));
  mFieldImmutableArrayOwnerVariable->generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %2, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayOwnerVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(mFieldImmutableArrayOwnerVariable->
              generateAssignmentIR(mContext, &mockExpression, arrayIndices, 5),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(5\\): Error: Incompatible types: "
              "can not cast from type 'int\\[\\]' to 'immutable int\\[\\]\\*'");
}

TEST_F(FieldImmutableArrayOwnerVariableTest, generateArrayElementAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  arrayIndices.push_back(&mockExpression);
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(mFieldImmutableArrayOwnerVariable->
              generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, fieldImmutableArrayOwnerRunTest) {
  runFile("tests/samples/test_field_immutable_array_owner.yz", "3");
}

TEST_F(TestFileRunner, fieldImmutableArrayOwnerRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_immutable_array_owner_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_immutable_array_owner_rce.yz:13)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldImmutableArrayOwnerNullsPreviousOwnerRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_immutable_array_owner_nulls_previous_owner.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_immutable_array_owner_nulls_previous_owner.yz:13)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldImmutableArrayOwnerElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_owner_element_assign.yz",
                    1,
                    "tests/samples/test_field_immutable_array_owner_element_assign.yz\\(13\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, fieldImmutableArrayOwnerCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_owner_cast_from_array_reference.yz",
                    1,
                    "tests/samples/test_field_immutable_array_owner_cast_from_array_reference.yz\\(12\\): "
                    "Error: Incompatible types: can not cast from type 'int\\[\\]\\[\\]' to 'immutable int\\[\\]\\[\\]\\*'");
}
