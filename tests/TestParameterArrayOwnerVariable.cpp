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

#include "TestFileSampleRunner.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/ParameterArrayOwnerVariable.hpp"
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

struct ParameterArrayOwnerVariableTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mBasicBlock;
  const wisey::ArrayType* mArrayType;
  ParameterArrayOwnerVariable* mVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
public:
  
  ParameterArrayOwnerVariableTest() : mLLVMContext(mContext.getLLVMContext()) {
    vector<unsigned long> dimensions;
    dimensions.push_back(3u);
    mArrayType = mContext.getArrayType(PrimitiveTypes::INT_TYPE, dimensions);

    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "test",
                                          mContext.getModule());
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    
    llvm::PointerType* arrayPointerType = mArrayType->getOwner()->getLLVMType(mContext);
    AllocaInst* alloc = IRWriter::newAllocaInst(mContext, arrayPointerType, "foo");
    mVariable = new ParameterArrayOwnerVariable("foo", mArrayType->getOwner(), alloc);

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(ParameterArrayOwnerVariableTest, getNameTest) {
  EXPECT_STREQ("foo", mVariable->getName().c_str());
}

TEST_F(ParameterArrayOwnerVariableTest, getTypeTest) {
  EXPECT_EQ(mArrayType->getOwner(), mVariable->getType());
}

TEST_F(ParameterArrayOwnerVariableTest, generateIdentifierIRTest) {
  mVariable->generateIdentifierIR(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, { i64 }, [3 x i32] }*"
  "\n  %0 = load { i64, i64, { i64 }, [3 x i32] }*, { i64, i64, { i64 }, [3 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, freeTest) {
  mVariable->free(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, { i64 }, [3 x i32] }*"
  "\n  %0 = load { i64, i64, { i64 }, [3 x i32] }*, { i64, i64, { i64 }, [3 x i32] }** %foo"
  "\n  %1 = bitcast { i64, i64, { i64 }, [3 x i32] }* %0 to i64*"
  "\n  call void @__destroyPrimitiveArrayFunction(i64* %1)"
  "\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, setToNullTest) {
  mVariable->setToNull(mContext);
  
  *mStringStream << *mBasicBlock;
  
  string expected =
  "\nentry:"
  "\n  %foo = alloca { i64, i64, { i64 }, [3 x i32] }*"
  "\n  store { i64, i64, { i64 }, [3 x i32] }* null, { i64, i64, { i64 }, [3 x i32] }** %foo\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ParameterArrayOwnerVariableTest, generateAssignmentDathTest) {
  vector<const IExpression*> indices;

  EXPECT_EXIT(mVariable->generateAssignmentIR(mContext, NULL, indices, 0),
              ::testing::ExitedWithCode(1),
              "Error: Assignment to method parameters is not allowed");
}

TEST_F(TestFileSampleRunner, parameterArrayOwnerOfIntsRunTest) {
  runFile("tests/samples/test_parameter_array_owner_of_ints.yz", "5");
}

TEST_F(TestFileSampleRunner, parameterArrayOwnerOfIntsIsNulledOnAssignmentRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_owner_of_ints_is_nulled_on_assignment.yz:19)\n");
}

TEST_F(TestFileSampleRunner, parameterArrayOwnerOfIntsIsFreedRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_parameter_array_owner_of_ints_is_freed.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MReferenceCountException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_parameter_array_owner_of_ints_is_freed.yz:10)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_parameter_array_owner_of_ints_is_freed.yz:18)\n"
                               "Details: Object referenced by expression still has 1 active reference\n");
}
