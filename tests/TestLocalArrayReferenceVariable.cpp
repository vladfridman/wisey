//
//  TestLocalArrayReferenceVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LocalArrayReferenceVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "TestFileSampleRunner.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/IntConstant.hpp"
#include "wisey/LocalArrayReferenceVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LocalArrayReferenceVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  const wisey::ArrayType* mAnotherArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  LocalArrayReferenceVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 1u);
    mAnotherArrayType = mContext.getArrayType(PrimitiveTypes::FLOAT_TYPE, 1u);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
  
  ~LocalArrayReferenceVariableTest() {
  }
};

TEST_F(LocalArrayReferenceVariableTest, basicFieldsTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc);

  EXPECT_STREQ("foo", variable.getName().c_str());
  EXPECT_EQ(mArrayType, variable.getType());
  EXPECT_FALSE(variable.isField());
  EXPECT_FALSE(variable.isSystem());
}

TEST_F(LocalArrayReferenceVariableTest, generateArrayIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc);
  variable.generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LocalArrayReferenceVariableTest, generateArrayWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mArrayType));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, mArrayType));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);

  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %1, i64 -1)"
  "\n  %2 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %2, i64 1)"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";

  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}


TEST_F(LocalArrayReferenceVariableTest, generateArrayWholeArrayAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mArrayType->getLLVMType(mContext);
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayReferenceVariable variable("foo", mArrayType, alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mAnotherArrayType));

  Mock::AllowLeak(&mockExpression);

  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatible types: can not cast from type 'float\\[\\]' to 'int\\[\\]'");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfIntsRunTest) {
  runFile("tests/samples/test_array_reference_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfIntsAutocastRunTest) {
  runFile("tests/samples/test_array_reference_of_ints_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_array_reference_of_ints_increment_element.yz", "5");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfModelOwnersRunTest) {
  runFile("tests/samples/test_array_reference_of_model_owners.yz", "2018");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_array_reference_of_model_owners.yz",
                                        "destructor object<8 bytes>*[5]\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileSampleRunner, arrayReferenceElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_reference_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_reference_element_is_nulled_on_owner_transfer.yz:19)\n");
}

TEST_F(TestFileSampleRunner,
       ownerVariableIsNulledOnOwnershipTransferToArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_reference_element.yz:18)\n");
}

TEST_F(TestFileSampleRunner, ownerArrayReferenceElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_array_reference_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_array_reference_elements_initialized_to_null.yz:16)\n");
}

TEST_F(TestFileSampleRunner, arrayReferenceOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_reference_of_model_references.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceCountDecrementsOnArrayReferenceElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_reference_element_unassign.yz", "5");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToArrayReferenceElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_reference_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_reference_element.yz:17)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, referenceArrayReferenceInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_reference_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_reference_initialized_to_null.yz:16)\n");
}

TEST_F(TestFileSampleRunner, localArrayReferenceOfModelReferencesOutOfBoundsRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_local_array_reference_of_model_references_out_of_bounds.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MArrayIndexOutOfBoundsException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_local_array_reference_of_model_references_out_of_bounds.yz:14)\n"
                               "Details: Index -1 is used on array of size 5\n");
}

