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
#include "ArrayType.hpp"
#include "FieldImmutableArrayReferenceVariable.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "IRWriter.hpp"
#include "ImmutableArrayType.hpp"
#include "IntConstant.hpp"
#include "ParameterReferenceVariable.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

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
  Function* mFunction;
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
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "test",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
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
    IVariable* thisVariable =
    new ParameterReferenceVariable(IObjectType::THIS, mObject, thisPointer, 0);
    mContext.getScopes().setVariable(mContext, thisVariable);
    
    mFieldImmutableArrayReferenceVariable =
    new FieldImmutableArrayReferenceVariable("foo", mObject, 0);
  }
  
  ~FieldImmutableArrayReferenceVariableTest() {
  }
};

TEST_F(FieldImmutableArrayReferenceVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mFieldImmutableArrayReferenceVariable->getName().c_str());
  EXPECT_EQ(mImmutableArrayType, mFieldImmutableArrayReferenceVariable->getType());
  EXPECT_TRUE(mFieldImmutableArrayReferenceVariable->isField());
  EXPECT_FALSE(mFieldImmutableArrayReferenceVariable->isStatic());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateIdentifierIRTest) {
  mFieldImmutableArrayReferenceVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateIdentifierReferenceIRTest) {
  mFieldImmutableArrayReferenceVariable->generateIdentifierReferenceIR(mContext, 0);
  
  *mStringStream << *mBasicBlock;
  string expected = string() +
  "\nentry:                                            ; No predecessors!" +
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
  
  *mStringStream << *mFunction;
  string expected =
  "\ndefine internal i32 @test() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = getelementptr %systems.vos.wisey.compiler.tests.CObject, %systems.vos.wisey.compiler.tests.CObject* null, i32 0, i32 1"
  "\n  %1 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %0"
  "\n  %2 = icmp eq { i64, i64, i64, [0 x i32] }* %1, null"
  "\n  br i1 %2, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n  %3 = icmp eq { i64, i64, i64, [0 x i32] }* null, null"
  "\n  br i1 %3, label %if.end1, label %if.notnull2"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %4 = bitcast { i64, i64, i64, [0 x i32] }* %1 to i64*"
  "\n  %5 = atomicrmw add i64* %4, i64 -1 monotonic"
  "\n  br label %if.end"
  "\n"
  "\nif.end1:                                          ; preds = %if.notnull2, %if.end"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %0"
  "\n"
  "\nif.notnull2:                                      ; preds = %if.end"
  "\n  %6 = bitcast { i64, i64, i64, [0 x i32] }* null to i64*"
  "\n  %7 = atomicrmw add i64* %6, i64 1 monotonic"
  "\n  br label %if.end1"
  "\n}\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateWholeArrayAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mFieldImmutableArrayReferenceVariable->generateAssignmentIR(mContext,
                                                                               &mockExpression,
                                                                               arrayIndices,
                                                                               5));
  EXPECT_STREQ("/tmp/source.yz(5): Error: Incompatible types: can not cast from type int[][] to immutable int[]\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(FieldImmutableArrayReferenceVariableTest, generateArrayElementAssignmentDeathTest) {
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  arrayIndices.push_back(&mockExpression);
  
  Mock::AllowLeak(&mockExpression);
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mFieldImmutableArrayReferenceVariable->
                   generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11));
  EXPECT_STREQ("/tmp/source.yz(11): Error: Attempting to change a value in an immutable array\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceRunTest) {
  runFile("tests/samples/test_field_immutable_array_reference.yz", 7);
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_immutable_array_reference_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_immutable_array_reference_rce.yz:15)\n"
                               "Details: Object referenced by expression of type immutable int[][]* still has 1 active reference\n"
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
                    "Error: Incompatible types: can not cast from type int\\[\\]\\[\\] to immutable int\\[\\]\\[\\]");
}

TEST_F(TestFileRunner, fieldImmutableArrayReferenceCastFromArrayOwnerRunDeathTest) {
  expectFailCompile("tests/samples/test_field_immutable_array_reference_cast_from_array_owner.yz",
                    1,
                    "tests/samples/test_field_immutable_array_reference_cast_from_array_owner.yz\\(12\\): "
                    "Error: Incompatible types: can not cast from type int\\[\\]\\[\\]\\* to immutable int\\[\\]\\[\\]");
}
