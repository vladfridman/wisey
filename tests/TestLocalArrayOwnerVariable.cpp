//
//  TestLocalArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalArrayOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/LocalArrayOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  LocalArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalArrayOwnerVariableTest() {
  }
};

TEST_F(LocalArrayOwnerVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mArrayType->getOwner(), variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(LocalArrayOwnerVariableTest, generateIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc);
  variable.generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc);

  EXPECT_EQ(alloc, variable.generateIdentifierReferenceIR(mContext));
}

TEST_F(LocalArrayOwnerVariableTest, generateAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType->getOwner()));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %1, i64 1, i64 4, i1 true)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'int\\[\\]' to 'int\\[\\]\\*'");
}

TEST_F(TestFileRunner, arrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_array_owner_of_ints.yz", "5");
}

TEST_F(TestFileRunner, arrayOwnerOfIntsAutocastRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_autocast.yz", "1");
}

TEST_F(TestFileRunner, arrayOwnerOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_increment_element.yz", "5");
}

TEST_F(TestFileRunner, localArrayOwner3dOfIntsStaticAllocationRunTest) {
  runFile("tests/samples/test_local_array_owner_3d_of_ints_static_allocation.yz", "12");
}

TEST_F(TestFileRunner, arrayOwnerOfModelOwnersRunTest) {
  runFile("tests/samples/test_array_owner_of_model_owners.yz", "2018");
}

TEST_F(TestFileRunner, arrayOwnerOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_array_owner_of_model_owners.yz",
                                        "destructor object<8 bytes>*[5]\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileRunner, arrayOwnerElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz:19)\n");
}

TEST_F(TestFileRunner,
       ownerVariableIsNulledOnOwnershipTransferToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz:18)\n");
}

TEST_F(TestFileRunner, ownerArrayOwnerElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_array_owner_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_array_owner_elements_initialized_to_null.yz:16)\n");
}

TEST_F(TestFileRunner, arrayOwnerOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_owner_of_model_references.yz", "2018");
}

TEST_F(TestFileRunner, referenceCountDecrementsOnArrayOwnerElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_owner_element_unassign.yz", "5");
}

TEST_F(TestFileRunner, referenceCountIncrementsOnAssignToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz:17)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileRunner, referenceArrayOwnerInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_owner_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_owner_initialized_to_null.yz:16)\n");
}

TEST_F(TestFileRunner, localArrayOfIntsRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_ints_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_ints_rce.yz:10)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileRunner, localArrayOfOwnersRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_owners_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_owners_rce.yz:14)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileRunner, localArrayOfReferencesRceRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_of_references_rce.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_of_references_rce.yz:14)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfIntsOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_ints_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_ints_out_of_bounds.yz:10)\n"
                               "Details: Index 2 is used on array of size 2\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfInts2dOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_ints_2d_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_ints_2d_out_of_bounds.yz:10)\n"
                               "Details: Index 6 is used on array of size 5\n");
}

TEST_F(TestFileRunner, localArrayOwnerOfModelOwnersOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_owner_of_model_owners_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_owner_of_model_owners_out_of_bounds.yz:13)\n"
                               "Details: Index 5 is used on array of size 5\n");
}
