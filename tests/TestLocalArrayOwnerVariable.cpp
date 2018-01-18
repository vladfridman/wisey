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
#include "TestFileSampleRunner.hpp"
#include "TestPrefix.hpp"
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
  Model* mModel;
  const wisey::ArrayType* mPrimitiveArrayType;
  const wisey::ArrayType* mOwnerArrayType;
  const wisey::ArrayType* mReferenceArrayType;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  LocalArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    vector<Type*> modelTypes;
    modelTypes.push_back(Type::getInt64Ty(mLLVMContext));
    string modelFullName = "systems.vos.wisey.compiler.tests.MModel";
    StructType* modelStructType = StructType::create(mLLVMContext, modelFullName);
    modelStructType->setBody(modelTypes);
    mModel = Model::newModel(AccessLevel::PUBLIC_ACCESS, modelFullName, modelStructType);
    mContext.addModel(mModel);
    IConcreteObjectType::generateNameGlobal(mContext, mModel);
    IConcreteObjectType::generateShortNameGlobal(mContext, mModel);
    IConcreteObjectType::generateVTable(mContext, mModel);

    mPrimitiveArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, 3u);
    mOwnerArrayType = mContext.getArrayType(mModel->getOwner(), 5u);
    mReferenceArrayType = mContext.getArrayType(mModel, 5u);

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

TEST_F(LocalArrayOwnerVariableTest, generatePrimitiveArrayIdentifierIRTest) {
  llvm::PointerType* arrayPointerType = mPrimitiveArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mPrimitiveArrayType->getOwner(), alloc);
  
  EXPECT_EQ(alloc, variable.generateIdentifierIR(mContext));
}

TEST_F(LocalArrayOwnerVariableTest, generatePrimitiveArrayWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mPrimitiveArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mPrimitiveArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mPrimitiveArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, IR_GENERATION_RELEASE));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca [3 x i32]*"
  "\n  store [3 x i32]* null, [3 x i32]** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateOwnerArrayWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mOwnerArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mOwnerArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mOwnerArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, IR_GENERATION_RELEASE));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca [5 x %systems.vos.wisey.compiler.tests.MModel*]*"
  "\n  store [5 x %systems.vos.wisey.compiler.tests.MModel*]* null, [5 x %systems.vos.wisey.compiler.tests.MModel*]** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateOwnerArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mOwnerArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mOwnerArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  arrayIndices.push_back(new IntConstant(1));
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mModel->getOwner()));
  Value* value = ConstantPointerNull::get(mModel->getOwner()->getLLVMType(mContext));
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, IR_GENERATION_RELEASE));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca [5 x %systems.vos.wisey.compiler.tests.MModel*]*"
  "\n  %0 = load [5 x %systems.vos.wisey.compiler.tests.MModel*]*, [5 x %systems.vos.wisey.compiler.tests.MModel*]** %foo"
  "\n  %1 = getelementptr [5 x %systems.vos.wisey.compiler.tests.MModel*], [5 x %systems.vos.wisey.compiler.tests.MModel*]* %0, i32 0, i32 1"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** %1"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MModel* %2 to i8*"
  "\n  call void @destructor.systems.vos.wisey.compiler.tests.MModel(i8* %3)"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** %1"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateReferenceArrayWholeArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mReferenceArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mReferenceArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mReferenceArrayType->getOwner()));
  Value* value = ConstantPointerNull::get(arrayPointerType);
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, IR_GENERATION_RELEASE));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca [5 x %systems.vos.wisey.compiler.tests.MModel*]*"
  "\n  store [5 x %systems.vos.wisey.compiler.tests.MModel*]* null, [5 x %systems.vos.wisey.compiler.tests.MModel*]** %foo\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generateReferenceArrayAssignmentTest) {
  llvm::PointerType* arrayPointerType = mReferenceArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mReferenceArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  arrayIndices.push_back(new IntConstant(1));
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mModel));
  Value* value = ConstantPointerNull::get(mModel->getLLVMType(mContext));
  ON_CALL(mockExpression, generateIR(_, _)).WillByDefault(Return(value));
  EXPECT_CALL(mockExpression, generateIR(_, IR_GENERATION_NORMAL));
  variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0);
  
  *mStringStream << *mBasicBlock;
  string expected =
  "\nentry:"
  "\n  %foo = alloca [5 x %systems.vos.wisey.compiler.tests.MModel*]*"
  "\n  %0 = load [5 x %systems.vos.wisey.compiler.tests.MModel*]*, [5 x %systems.vos.wisey.compiler.tests.MModel*]** %foo"
  "\n  %1 = getelementptr [5 x %systems.vos.wisey.compiler.tests.MModel*], [5 x %systems.vos.wisey.compiler.tests.MModel*]* %0, i32 0, i32 1"
  "\n  %2 = load %systems.vos.wisey.compiler.tests.MModel*, %systems.vos.wisey.compiler.tests.MModel** %1"
  "\n  %3 = bitcast %systems.vos.wisey.compiler.tests.MModel* %2 to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %3, i64 -1)"
  "\n  %4 = bitcast %systems.vos.wisey.compiler.tests.MModel* null to i8*"
  "\n  call void @__adjustReferenceCounterForConcreteObjectSafely(i8* %4, i64 1)"
  "\n  store %systems.vos.wisey.compiler.tests.MModel* null, %systems.vos.wisey.compiler.tests.MModel** %1"
  "\n";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(LocalArrayOwnerVariableTest, generatePrimitiveArrayWholeArrayAssignmentDeathTest) {
  llvm::PointerType* arrayPointerType = mPrimitiveArrayType->getOwner()->
    getLLVMType(mContext)->getPointerTo();
  AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
  LocalArrayOwnerVariable variable("foo", mPrimitiveArrayType->getOwner(), alloc);
  vector<const IExpression*> arrayIndices;
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(mPrimitiveArrayType));
  
  Mock::AllowLeak(&mockExpression);
  
  EXPECT_EXIT(variable.generateAssignmentIR(mContext, &mockExpression, arrayIndices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Incompatable array types in array assignement");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_array_owner_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfIntsAutocastRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_autocast.yz", "1");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfIntsIncrementElementRunTest) {
  runFile("tests/samples/test_array_owner_of_ints_increment_element.yz", "5");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfModelOwnersRunTest) {
  runFile("tests/samples/test_array_of_model_owners.yz", "2018");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfModelOwnersDestructorsAreCalledRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_array_of_model_owners.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MCar*[5]*\n"
                                        "destructor systems.vos.wisey.compiler.tests.MCar\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}

TEST_F(TestFileSampleRunner, arrayOwnerElementIsNulledOnOwnerTranserRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_owner_element_is_nulled_on_owner_transfer.yz:19)\n");
}

TEST_F(TestFileSampleRunner,
       ownerVariableIsNulledOnOwnershipTransferToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_variable_is_nulled_on_ownership_transfer_to_array_owner_element.yz:18)\n");
}

TEST_F(TestFileSampleRunner, ownerArrayOwnerElementsInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_owner_array_owner_elements_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_owner_array_owner_elements_initialized_to_null.yz:16)\n");
}

TEST_F(TestFileSampleRunner, arrayOwnerOfModelReferencesRunTest) {
  runFile("tests/samples/test_array_owner_of_model_references.yz", "2018");
}

TEST_F(TestFileSampleRunner, referenceCountDecrementsOnArrayOwnerElementUnassignRunTest) {
  runFile("tests/samples/test_reference_count_decrements_on_array_owner_element_unassign.yz", "5");
}

TEST_F(TestFileSampleRunner, referenceCountIncrementsOnAssignToArrayOwnerElementRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_count_increments_on_assign_to_array_owner_element.yz:17)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}

TEST_F(TestFileSampleRunner, referenceArrayOwnerInitializedToNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_reference_array_owner_initialized_to_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_reference_array_owner_initialized_to_null.yz:16)\n");
}

