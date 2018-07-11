//
//  TestArrayGetSizeMethod.cpp
//  runtests
//
//  Created by Vladimir Fridman on 5/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayGetSizeMethod}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayGetSizeMethod.hpp"
#include "wisey/FakeExpression.hpp"
#include "wisey/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArrayGetSizeMethodTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const ArrayGetSizeMethod* mArrayGetSizeMethod;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  ArrayGetSizeMethodTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
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
    
    mArrayGetSizeMethod = ArrayGetSizeMethod::ARRAY_GET_SIZE_METHOD;
  }
  
  ~ArrayGetSizeMethodTest() {
    delete mStringStream;
  }
};

TEST_F(ArrayGetSizeMethodTest, getNameTest) {
  EXPECT_STREQ("getSize", mArrayGetSizeMethod->getName().c_str());
}

TEST_F(ArrayGetSizeMethodTest, getLLVMTypeTest) {
  EXPECT_EQ(FunctionType::get(Type::getInt64Ty(mLLVMContext), false),
            mArrayGetSizeMethod->getLLVMType(mContext));
}

TEST_F(ArrayGetSizeMethodTest, generateIRTest) {
  wisey::ArrayType* arrayType = mContext.getArrayType(PrimitiveTypes::FLOAT, 1);
  Value* null = ConstantPointerNull::get(arrayType->getLLVMType(mContext));
  ArrayGetSizeMethod::generateIR(mContext, new FakeExpression(null, arrayType));
  BranchInst::Create(mEntryBlock, mDeclareBlock);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @main() personality i32 (...)* @__gxx_personality_v0 {"
  "\ndeclare:"
  "\n  br label %entry"
  "\n"
  "\nentry:                                            ; preds = %declare"
  "\n  %isNull = icmp eq { i64, i64, i64, [0 x float] }* null, null"
  "\n  br i1 %isNull, label %if.null, label %if.not.null"
  "\n"
  "\nif.null:                                          ; preds = %entry"
  "\n  invoke void @__throwNPE()"
  "\n          to label %invoke.continue unwind label %cleanup"
  "\n"
  "\nif.not.null:                                      ; preds = %entry"
  "\n  %0 = getelementptr { i64, i64, i64, [0 x float] }, { i64, i64, i64, [0 x float] }* null, i32 0, i32 1"
  "\n  %1 = load i64, i64* %0"
  "\n"
  "\ncleanup:                                          ; preds = %if.null"
  "\n  %2 = landingpad { i8*, i32 }"
  "\n          cleanup"
  "\n  %3 = alloca { i8*, i32 }"
  "\n  br label %cleanup.cont"
  "\n"
  "\ncleanup.cont:                                     ; preds = %cleanup"
  "\n  store { i8*, i32 } %2, { i8*, i32 }* %3"
  "\n  %4 = getelementptr { i8*, i32 }, { i8*, i32 }* %3, i32 0, i32 0"
  "\n  %5 = load i8*, i8** %4"
  "\n  %6 = call i8* @__cxa_get_exception_ptr(i8* %5)"
  "\n  %7 = getelementptr i8, i8* %6, i64 8"
  "\n  resume { i8*, i32 } %2"
  "\n"
  "\ninvoke.continue:                                  ; preds = %if.null"
  "\n  unreachable"
  "\n}"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ArrayGetSizeMethodTest, isTypeKindTest) {
  EXPECT_FALSE(mArrayGetSizeMethod->isPrimitive());
  EXPECT_FALSE(mArrayGetSizeMethod->isOwner());
  EXPECT_FALSE(mArrayGetSizeMethod->isReference());
  EXPECT_FALSE(mArrayGetSizeMethod->isArray());
  EXPECT_TRUE(mArrayGetSizeMethod->isFunction());
  EXPECT_FALSE(mArrayGetSizeMethod->isPackage());
  EXPECT_FALSE(mArrayGetSizeMethod->isNative());
  EXPECT_FALSE(mArrayGetSizeMethod->isPointer());
  EXPECT_FALSE(mArrayGetSizeMethod->isImmutable());
}

TEST_F(ArrayGetSizeMethodTest, isObjectTest) {
  EXPECT_FALSE(mArrayGetSizeMethod->isController());
  EXPECT_FALSE(mArrayGetSizeMethod->isInterface());
  EXPECT_FALSE(mArrayGetSizeMethod->isModel());
  EXPECT_FALSE(mArrayGetSizeMethod->isNode());
}

TEST_F(ArrayGetSizeMethodTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mArrayGetSizeMethod->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type [].getSize is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(TestFileRunner, getArraySizeRunTest) {
  runFile("tests/samples/test_get_array_size.yz", 7);
}

TEST_F(TestFileRunner, getNullArraySizeRunDeathTest) {
  compileAndRunFileCheckOutput("tests/samples/test_get_null_array_size.yz",
                               1,
                               "",
                               "Unhandled exception wisey.lang.MNullPointerException\n"
                               "  at systems.vos.wisey.compiler.tests.CProgram.run(tests/samples/test_get_null_array_size.yz:9)\n"
                               "Main thread ended without a result\n");
}
