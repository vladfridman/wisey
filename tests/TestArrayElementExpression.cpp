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
#include "MockVariable.hpp"
#include "TestFileSampleRunner.hpp"
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
  NiceMock<MockExpression>* mArrayExpression;
  NiceMock<MockExpression>* mArrayIndexExpression;
  NiceMock<MockVariable>* mArrayVariable;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  Function* mFunction;
  BasicBlock* mBasicBlock;
  wisey::ArrayType* mArrayType;
  ArrayElementExpression* mArrayElementExpression;
  
  ArrayElementExpressionTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mArrayExpression(new NiceMock<MockExpression>()),
  mArrayIndexExpression(new NiceMock<MockExpression>()),
  mArrayVariable(new NiceMock<MockVariable>()) {
    FunctionType* functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    functionType = FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mArrayType = new wisey::ArrayType(PrimitiveTypes::INT_TYPE, 1u);
    Value* null = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
    ON_CALL(*mArrayExpression, generateIR(_, _)).WillByDefault(Return(null));
    ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayExpression, getVariable(_, _)).WillByDefault(Return(mArrayVariable));
    ON_CALL(*mArrayVariable, getType()).WillByDefault(Return(mArrayType));
    ON_CALL(*mArrayVariable, generateIdentifierIR(_)).WillByDefault(Return(null));
    ConstantInt* three = ConstantInt::get(Type::getInt32Ty(mLLVMContext), 3);
    ON_CALL(*mArrayIndexExpression, generateIR(_, _)).WillByDefault(Return(three));
    ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
    
    mArrayElementExpression = new ArrayElementExpression(mArrayExpression,
                                                         mArrayIndexExpression,
                                                         0);
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
  mArrayElementExpression->generateIR(mContext, PrimitiveTypes::VOID_TYPE);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\nentry:"
  "\n  %0 = bitcast { i64, i64, i64, [0 x i32] }* null to i8*"
  "\n  invoke void @__checkForNullAndThrow(i8* %0)"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\ncleanup:                                          ; preds = %entry"
  "\n  %1 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  resume { i8*, i32 } %1"
  "\n"
  "\ninvoke.continue:                                  ; preds = %entry"
  "\n  %2 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* null, i64 0, i32 2"
  "\n  %elementSize = load i64, i64* %2"
  "\n  %3 = getelementptr { i64, i64, i64, [0 x i32] }, { i64, i64, i64, [0 x i32] }* null, i64 0, i32 3"
  "\n  %conv = zext i32 3 to i64"
  "\n  %offset = mul i64 %elementSize, %conv"
  "\n  %4 = bitcast [0 x i32]* %3 to i8*"
  "\n  %5 = getelementptr i8, i8* %4, i64 %offset"
  "\n  %6 = bitcast i8* %5 to i32*"
  "\n  %7 = load i32, i32* %6"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ArrayElementExpressionTest, isConstantTest) {
  EXPECT_FALSE(mArrayElementExpression->isConstant());
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
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->generateIR(mContext, PrimitiveTypes::INT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Expecting array type expression before \\[\\] but expression type is int");
}

TEST_F(ArrayElementExpressionTest, generateIRForNonIntTypeIndexDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayIndexExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::FLOAT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->generateIR(mContext, PrimitiveTypes::FLOAT_TYPE),
              ::testing::ExitedWithCode(1),
              "Error: Array index should be integer type, but it is float");
}

TEST_F(ArrayElementExpressionTest, getTypeDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  ON_CALL(*mArrayExpression, getType(_)).WillByDefault(Return(PrimitiveTypes::INT_TYPE));
  
  EXPECT_EXIT(mArrayElementExpression->getType(mContext),
              ::testing::ExitedWithCode(1),
              "Error: Expecting array type expression before \\[\\] but expression type is int");
}

TEST_F(ArrayElementExpressionTest, generateIRDeathTest) {
  Mock::AllowLeak(mArrayExpression);
  Mock::AllowLeak(mArrayIndexExpression);
  Mock::AllowLeak(mArrayVariable);
  vector<const IExpression*> arrayIndices;

  mArrayType = new wisey::ArrayType(PrimitiveTypes::INT_TYPE, 2u);
  Value* null = ConstantPointerNull::get(mArrayType->getLLVMType(mContext));

  EXPECT_EXIT(ArrayElementExpression::generateElementIR(mContext,
                                                        mArrayType,
                                                        null,
                                                        arrayIndices),
              ::testing::ExitedWithCode(1),
              "Error: Expression does not reference an array element");
}

TEST_F(TestFileSampleRunner, arrayElementGetOnNullArrayRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_element_get_on_null_array.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_element_get_on_null_array.yz:10)\n");
}

TEST_F(TestFileSampleRunner, arrayElementSetOnNullArrayRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_array_element_set_on_null_array.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_array_element_set_on_null_array.yz:9)\n");
}

TEST_F(TestFileSampleRunner, fieldArrayReferenceOfModelReferenceIsNullRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_field_array_reference_of_model_references_is_null.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CController.init(tests/samples/test_field_array_reference_of_model_references_is_null.yz:16)\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_field_array_reference_of_model_references_is_null.yz:29)\n");
}

