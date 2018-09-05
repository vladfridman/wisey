//
//  TestArrayType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ArrayType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/Support/raw_ostream.h>
#include <llvm/IR/Constants.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/ArrayType.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ArrayTypeTest : public Test {
  IRGenerationContext mContext;
  llvm::LLVMContext& mLLVMContext;
  ArrayType* mArrayType;
  ArrayType* mMultiDimentionalArrayType;
  llvm::BasicBlock* mEntryBlock;
  llvm::BasicBlock* mDeclareBlock;
  llvm::Function* mFunction;
  string mStringBuffer;
  llvm::raw_string_ostream* mStringStream;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;

  ArrayTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    vector<unsigned long> dimensions;
    mArrayType = new ArrayType(PrimitiveTypes::LONG, 1u);
    mMultiDimentionalArrayType = new ArrayType(PrimitiveTypes::LONG, 2u);
 
    llvm::FunctionType* functionType =
    llvm::FunctionType::get(llvm::Type::getInt32Ty(mContext.getLLVMContext()), false);
    mFunction = llvm::Function::Create(functionType,
                                       llvm::GlobalValue::InternalLinkage,
                                       "main",
                                       mContext.getModule());
    mDeclareBlock = llvm::BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mEntryBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    InjectionArgumentList injectionArgumentList;
    IField* field = new ReceivedField(mArrayType, "mField", 0);
    ON_CALL(mConcreteObjectType, findField(_)).WillByDefault(Return(field));

    mStringStream = new llvm::raw_string_ostream(mStringBuffer);
}
};

TEST_F(ArrayTypeTest, getOwnerTest) {
  const ArrayOwnerType* arrayOwnerType = mArrayType->getOwner();
  EXPECT_TRUE(arrayOwnerType->isArray());
  EXPECT_TRUE(arrayOwnerType->isOwner());
  EXPECT_EQ(mArrayType, arrayOwnerType->getArrayType(mContext, 0));
}

TEST_F(ArrayTypeTest, getElementTypeTest) {
  EXPECT_EQ(PrimitiveTypes::LONG, mMultiDimentionalArrayType->getElementType());
}

TEST_F(ArrayTypeTest, getNumberOfDimensionsTest) {
  EXPECT_EQ(2u, mMultiDimentionalArrayType->getNumberOfDimensions());
}

TEST_F(ArrayTypeTest, getNameTest) {
  EXPECT_STREQ("long[]", mArrayType->getTypeName().c_str());
}

TEST_F(ArrayTypeTest, getLLVMTypeTest) {
  llvm::PointerType* arrayLLVMType = mArrayType->getLLVMType(mContext);
  ASSERT_TRUE(arrayLLVMType->getPointerElementType()->isStructTy());
  llvm::StructType* arrayStruct = (llvm::StructType*) arrayLLVMType->getPointerElementType();

  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(0));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(1));
  EXPECT_EQ(llvm::Type::getInt64Ty(mLLVMContext), arrayStruct->getElementType(2));
  EXPECT_EQ(llvm::ArrayType::get(llvm::Type::getInt64Ty(mLLVMContext), 0u),
            arrayStruct->getElementType(ArrayType::ARRAY_ELEMENTS_START_INDEX));
}

TEST_F(ArrayTypeTest, canCastToTest) {
  EXPECT_TRUE(mArrayType->canCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mArrayType->canCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArrayType->canCastTo(mContext, mArrayType));
}

TEST_F(ArrayTypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mArrayType->canAutoCastTo(mContext, PrimitiveTypes::BOOLEAN));
  EXPECT_FALSE(mArrayType->canAutoCastTo(mContext, PrimitiveTypes::STRING));
  EXPECT_TRUE(mArrayType->canAutoCastTo(mContext, mArrayType));
}

TEST_F(ArrayTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mArrayType->isPrimitive());
  EXPECT_FALSE(mArrayType->isOwner());
  EXPECT_TRUE(mArrayType->isReference());
  EXPECT_TRUE(mArrayType->isArray());
  EXPECT_FALSE(mArrayType->isFunction());
  EXPECT_FALSE(mArrayType->isPackage());
  EXPECT_FALSE(mArrayType->isNative());
  EXPECT_FALSE(mArrayType->isPointer());
  EXPECT_FALSE(mArrayType->isImmutable());
}

TEST_F(ArrayTypeTest, isObjectTest) {
  EXPECT_FALSE(mArrayType->isController());
  EXPECT_FALSE(mArrayType->isInterface());
  EXPECT_FALSE(mArrayType->isModel());
  EXPECT_FALSE(mArrayType->isNode());
}

TEST_F(ArrayTypeTest, createLocalVariableTest) {
  mArrayType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;

  string expected =
  "\ndeclare:"
  "\n  %0 = alloca { i64, i64, i64, [0 x i64] }*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store { i64, i64, i64, [0 x i64] }* null, { i64, i64, i64, [0 x i64] }** %0\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ArrayTypeTest, createFieldVariableTest) {
  mArrayType->createFieldVariable(mContext, "mField", &mConcreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(ArrayTypeTest, createParameterVariableTest) {
  llvm::Value* value = llvm::ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
  mArrayType->createParameterVariable(mContext, "var", value, 0);
  IVariable* variable = mContext.getScopes().getVariable("var");
  
  EXPECT_NE(variable, nullptr);
  
  *mStringStream << *mFunction;
  
  string expected =
  "\ndefine internal i32 @main() {"
  "\ndeclare:"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = icmp eq { i64, i64, i64, [0 x i64] }* null, null"
  "\n  br i1 %0, label %if.end, label %if.notnull"
  "\n"
  "\nif.end:                                           ; preds = %if.notnull, %entry"
  "\n"
  "\nif.notnull:                                       ; preds = %entry"
  "\n  %1 = bitcast { i64, i64, i64, [0 x i64] }* null to i64*"
  "\n  %count = load i64, i64* %1"
  "\n  %2 = add i64 %count, 1"
  "\n  store i64 %2, i64* %1"
  "\n  br label %if.end"
  "\n}\n";

  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(ArrayTypeTest, injectDeathTest) {
  Mock::AllowLeak(&mConcreteObjectType);
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mArrayType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type long[] is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ArrayTypeTest, extractLLVMArrayTest) {
  llvm::Value* value = llvm::ConstantPointerNull::get(mArrayType->getLLVMType(mContext));
  ArrayType::extractLLVMArray(mContext, value);
  
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\nentry:                                            ; No predecessors!"
  "\n  %0 = getelementptr { i64, i64, i64, [0 x i64] }, { i64, i64, i64, [0 x i64] }* null, i32 0, i32 3"
  "\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(TestFileRunner, llvmArrayRunTest) {
  runFile("tests/samples/test_llvm_array.yz", 4);
}

TEST_F(TestFileRunner, arrayTypeAutocastToBoolean) {
  runFile("tests/samples/test_array_type_autocast_to_boolean.yz", 5);
}

TEST_F(TestFileRunner, llvmArrayDebugDestructorsRunTest) {
  runFileCheckOutputWithDestructorDebug("tests/samples/test_llvm_array.yz",
                                        "destructor systems.vos.wisey.compiler.tests.MElement\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MElement\n"
                                        "destructor systems.vos.wisey.compiler.tests.MElement\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MElement\n"
                                        "destructor systems.vos.wisey.compiler.tests.MElement\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MElement\n"
                                        "destructor systems.vos.wisey.compiler.tests.MElement\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MElement\n"
                                        "destructor systems.vos.wisey.compiler.tests.MElement\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.MElement\n"
                                        "destructor systems.vos.wisey.compiler.tests.CProgram\n"
                                        "done destructing heap object systems.vos.wisey.compiler.tests.CProgram\n",
                                        "");
}
