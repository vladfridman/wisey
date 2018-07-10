//
//  TestParameterArrayOwnerVariable.cpp
//  runtests
//
//  Created by Vladimir Fridman on 1/24/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ParameterArrayOwnerVariable}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterArrayOwnerVariable.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ParameterArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  const wisey::ArrayType* mArrayType;
  ParameterArrayOwnerVariable* mVariable;
  AllocaInst* mArrayAlloc;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT, 1u);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
    mArrayAlloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
    mVariable = new ParameterArrayOwnerVariable("foo", mArrayType->getOwner(), mArrayAlloc, 0);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterArrayOwnerVariableTest, basicFieldsTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
  EXPECT_EQ(mArrayType->getOwner(), mVariable->getType());
  EXPECT_FALSE(mVariable->isField());
  EXPECT_FALSE(mVariable->isStatic());
}

TEST_F(ParameterArrayOwnerVariableTest, generateIdentifierIRTest) {
  mVariable->generateIdentifierIR(mContext, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, generateIdentifierReferenceIRTest) {
  EXPECT_EQ(mArrayAlloc, mVariable->generateIdentifierReferenceIR(mContext, 0));
}

TEST_F(ParameterArrayOwnerVariableTest, freeTest) {
  llvm::PointerType* int8Pointer = Type::getInt8Ty(mLLVMContext)->getPointerTo();
  Value* nullPointer = ConstantPointerNull::get(int8Pointer);
  mVariable->free(mContext, nullPointer, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = load { i64, i64, i64, [0 x i32] }*, { i64, i64, i64, [0 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i32] }* %0 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %1, i64 1, i8* getelementptr inbounds ([7 x i8], [7 x i8]* @\"int[]*\", i32 0, i32 0), i8* null)"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, setToNullTest) {
  mVariable->setToNull(mContext, 0);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %foo = alloca { i64, i64, i64, [0 x i32] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store { i64, i64, i64, [0 x i32] }* null, { i64, i64, i64, [0 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, generateAssignmentDeathTest) {
  vector<const IExpression*> indices;

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mVariable->generateAssignmentIR(mContext, NULL, indices, 1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Assignment to method parameters is not allowed\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, parameterArrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_parameter_array_owner_of_ints.yz", 5);
}

TEST_F(TestFileRunner, parameterArrayOwnerOfIntsIsNulledOnAssignmentRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz:19)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, parameterArrayOwnerOfIntsIsFreedRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_owner_of_ints_is_freed.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_parameter_array_owner_of_ints_is_freed.yz:9)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_owner_of_ints_is_freed.yz:18)\n"
                               "Details: Object referenced by expression of type int[][]* still has 1 active reference\n"
                               "Main thread ended without a result\n");
}
