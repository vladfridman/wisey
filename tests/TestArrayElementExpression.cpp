//
//  TestArrayElementExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/12/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayElementExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockExpression.hpp"
#include "MockExpressionAssignable.hpp"
#include "MockVariable.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayElementExpression.hpp"
#include "wisey/IRWriter.hpp"
#include "wisey/PrimitiveTypes.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct ArrayElementExpressionTest : Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockExpressionAssignable>* mArrayExpression;
  NiceMock<MockExpression>* mArrayIndexExpression;
  NiceMock<MockVariable>* mArrayVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  BasicBlock* mDeclareBlock;
  BasicBlock* mEntryBlock;
  wisey::ArrayType* mArrayType;
  ArrayElementExpression* mArrayElementExpression;
  
  ArrayElementExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mArrayExpression(new NiceMock<MockExpressionAssignable>()),
  mArrayIndexExpression(new NiceMock<MockExpression>()),
  mArrayVariable(new NiceMock<MockVariable>()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = new wisey::ArrayType(PrimitiveTypes::INT, 1u);
    Value* null = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
    ON_CALL(*mArrayExpression, generateIR(_, _)).WillByDefault(Return(null));
    ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayExpression, getVariable(_, _)).WillByDefault(Return(mArrayVariable));
    ON_CALL(*mArrayVariable, getType()).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayVariable, generateIdentifierIR(_, _)).WillByDefault(Return(null));
    ConstantInt* three = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mArrayIndexExpression, generateIR(_, _)).WillByDefault(Return(three));
    ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
    
    mArrayElementExpression = new ArrayElementExpression(mArrayExpression,
                                                         mArrayIndexExpression,
                                                         3);
  }
  
  ~ArrayElementExpressionTest() {
    delete mStringStream;
    delete mArrayExpression;
    delete mArrayIndexExpression;
    delete mArrayVariable;
  }
  
  static void printArrayExpression(IRGenerationContext& context, iostream& stream) {
    stream << "array";
  }
  
  static void printArrayIndex(IRGenerationContext& context, iostream& stream) {
    stream << "index";
  }
};

TEST_F(ArrayElementExpressionTest, generateIRTest) {
  mArrayElementExpression->generateIR(mContext, PrimitiveTypes::VOID);
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %isNull = icmp eq { i64, i64, i64, [0 x i32] }* null, null"
  "\n  br i1 %isNull, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  invoke void @__throwNPE()"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %conv = sext i32 3 to i64"
  "\n  %0 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* null, i64 0, i32 1"
  "\n  %arraySize = load i64, i64* %0"
  "\n  invoke void @__checkArrayIndexFunction(i64 %conv, i64 %arraySize)"
  "\n          to label %invoke.continue1 unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %if.not.null, %if.null"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %2 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %1, { i8*, i32 }* %2"
  "\n  %3 = getelementptr { i8*, i32 }, { i8*, i32 }* %2, i32 0, i32 0"
  "\n  %4 = load i8*, i8** %3"
  "\n  %5 = call i8* @__cxa_get_exception_ptr(i8* %4)"
  "\n  %6 = getelementptr i8, i8* %5, i64 8"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.null"
  "\n  unreachable"
  "\n"
  "\ninvoke.continue1:                                 ; preds = %if.not.null"
  "\n  %7 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* null, i64 0, i32 2"
  "\n  %elementSize = load i64, i64* %7"
  "\n  %8 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* null, i64 0, i32 3"
  "\n  %offset = mul i64 %elementSize, %conv"
  "\n  %9 = bitcast [0 x i32]* %8 to i8*"
  "\n  %10 = getelementptr i8, i8* %9, i64 %offset"
  "\n  %11 = bitcast i8* %10 to i32*"
  "\n  %12 = load i32, i32* %11"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayElementExpressionTest, isConstantTest) {
  EXPECT_FALSE(mArrayElementExpression->isConstant());
}

TEST_F(ArrayElementExpressionTest, isAssignableTest) {
  EXPECT_TRUE(mArrayElementExpression->isAssignable());
}

TEST_F(ArrayElementExpressionTest, getTypeTest) {
  EXPECT_EQ(mArrayType->getElementType(), mArrayElementExpression->getType(mContext));
}

TEST_F(ArrayElementExpressionTest, printToStreamTest) {
  stringstream stringStream;
  ON_CALL(*mArrayExpression, printToStream(_, _)).WillByDefault(Invoke(printArrayExpression));
  ON_CALL(*mArrayIndexExpression, printToStream(_, _)).WillByDefault(Invoke(printArrayIndex));
  mArrayElementExpression->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("array[index]", stringStream.str().c_str());
}

TEST_F(ArrayElementExpressionTest, generateIRForNonArrayTypeDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mArrayElementExpression->generateIR(mContext, PrimitiveTypes::INT));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Expecting array type expression before [] but expression type is int\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ArrayElementExpressionTest, generateIRForNonIntTypeIndexDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mArrayElementExpression->generateIR(mContext, PrimitiveTypes::FLOAT));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Array index should be integer type, but it is float\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ArrayElementExpressionTest, getTypeDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT));
  
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());

  EXPECT_ANY_THROW(mArrayElementExpression->getType(mContext));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Expecting array type expression before [] but expression type is int\n",
              buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ArrayElementExpressionTest, generateIRDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  vector<const IExpression*> arrayIndices;

  mArrayType = new wisey::ArrayType(PrimitiveTypes::INT, 2u);
  Value* null = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(ArrayElementExpression::generateElementIR(mContext,
                                                             mArrayType,
                                                             null,
                                                             arrayIndices,
                                                             3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: Expression does not reference an array element\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, ownerArrayNotNulledOnElementAssignTest) {
  runFile("tests/samples/test_owner_array_not_nulled_on_element_assign.yz", 1);
}

TEST_F(TestFileRunner, ownerArrayNulledOnWholeArrayAssignTest) {
  runFile("tests/samples/test_owner_array_nulled_on_whole_array_assign.yz", 1);
}

TEST_F(TestFileRunner, arrayElementGetOnNullArrayRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_element_get_on_null_array.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_element_get_on_null_array.yz:10)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, arrayElementSetOnNullArrayRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_element_set_on_null_array.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_element_set_on_null_array.yz:9)\n"
                               "Main thread ended without a result\n");
}

TEST_F(TestFileRunner, fieldArrayReferenceOfModelReferenceIsNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_reference_of_model_references_is_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_field_array_reference_of_model_references_is_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_reference_of_model_references_is_null.yz:29)\n"
                               "Main thread ended without a result\n");
}

