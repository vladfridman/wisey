//
//  TestLocalImmutableArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 4/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalImmutableArrayOwnerVariable}
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
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/ImmutableArrayOwnerType.hpp"
#include "wisey/ImmutableArrayType.hpp"
#include "wisey/LocalImmutableArrayOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalImmutableArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const ImmutableArrayOwnerType* mImmutableArrayOwnerType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalImmutableArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    mImmutableArrayOwnerType = mArrayType->getImmutable()->getOwner();
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalImmutableArrayOwnerVariableTest() {
  }
};

TEST_F(LocalImmutableArrayOwnerVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  
  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mImmutableArrayOwnerType, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(LocalImmutableArrayOwnerVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  variable.generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalImmutableArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  
  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext));
}

TEST_F(LocalImmutableArrayOwnerVariableTest, generateWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mImmutableArrayOwnerType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mImmutableArrayOwnerType));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %1, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalImmutableArrayOwnerVariableTest, generateWholeArrayAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Incompatible types: "
              "can not cast from type 'int\\[\\]' to 'immutable int\\[\\]\\*'");
}

TEST_F(LocalImmutableArrayOwnerVariableTest, generateArrayElementAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mImmutableArrayOwnerType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalImmutableArrayOwnerVariable variable("foo", mImmutableArrayOwnerType, alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  arrayIndices.push_back(&mockExpression);
  
  Mock::AllowLeak(&mockExpression);

  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 11),
              ::testing::ExitedWithCode(1),
              "/tmp/source.yz\\(11\\): Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, localImmutableArrayOwnerRunTest) {
  runFile("tests/samples/test_local_immutable_array_owner.yz", "3");
}

TEST_F(TestFileRunner, localImmutableArrayOwnerRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_immutable_array_owner_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_immutable_array_owner_rce.yz:11)\n"
                               "Details: Object referenced by expression still has 1 active reference\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localImmutableArrayOwnerNullsPreviousOwnerRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_immutable_array_owner_nulls_previous_owner.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_immutable_array_owner_nulls_previous_owner.yz:11)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, localImmutableArrayOwnerElementAssignRunDeathTest) {
  expectFailCompile("tests/samples/test_local_immutable_array_owner_element_assign.yz",
                    1,
                    "tests/samples/test_local_immutable_array_owner_element_assign.yz\\(11\\): "
                    "Error: Attempting to change a value in an immutable array");
}

TEST_F(TestFileRunner, localImmutableArrayOwnerCastFromArrayReferenceRunDeathTest) {
  expectFailCompile("tests/samples/test_local_immutable_array_owner_cast_from_array_reference.yz",
                    1,
                    "tests/samples/test_local_immutable_array_owner_cast_from_array_reference.yz\\(10\\): "
                    "Error: Incompatible types: can not cast from type 'int\\[\\]\\[\\]' to 'immutable int\\[\\]\\[\\]\\*'");
}