//
//  TestLLVMImmutablePointerOwnerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMImmutablePointerOwnerType}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/LLVMImmutablePointerOwnerType.hpp"
#include "wisey/LLVMImmutablePointerType.hpp"
#include "wisey/LLVMPrimitiveTypes.hpp"
#include "wisey/StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMImmutablePointerOwnerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  const LLVMImmutablePointerOwnerType* mLLVMImmutablePointerOwnerType;
  const LLVMPointerType* mPointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMImmutablePointerOwnerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = llvm::BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = llvm::BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    StructType* structType = StructType::create(mLLVMContext, "mystruct");
    
    ON_CALL(mConcreteObjectType, isReference()).WillByDefault(Return(true));
    ON_CALL(mConcreteObjectType, isNative()).WillByDefault(Return(false));
    ON_CALL(mConcreteObjectType, getLLVMType(_)).WillByDefault(Return(structType->getPointerTo()));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I8);
    mLLVMImmutablePointerOwnerType = mPointerType->getImmutable()->getOwner();
  }
  
  ~LLVMImmutablePointerOwnerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMImmutablePointerOwnerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mLLVMImmutablePointerOwnerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8::pointer::immutable*",
               mLLVMImmutablePointerOwnerType->getTypeName().c_str());
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, canAutoCastToTest) {
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->canAutoCastTo(mContext,
                                                            mPointerType->getImmutable()));
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  const LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  const LLVMImmutablePointerOwnerType* anotherPointerOwnerType =
    anotherPointerType->getImmutable()->getOwner();
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canAutoCastTo(mContext, anotherPointerOwnerType));
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, canCastTest) {
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->canCastTo(mContext, mPointerType->getImmutable()));
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  const LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  const LLVMImmutablePointerOwnerType* anotherPointerOwnerType =
    anotherPointerType->getImmutable()->getOwner();
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canCastTo(mContext, anotherPointerOwnerType));
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, castToTest) {
  Value* value = ConstantPointerNull::get(mLLVMImmutablePointerOwnerType->getLLVMType(mContext));
  Value* result = mLLVMImmutablePointerOwnerType->
  castTo(mContext, value, mPointerType->getImmutable(), 0);
 
  EXPECT_EQ(value, result);
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isPrimitive());
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->isOwner());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isReference());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isArray());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isFunction());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isPackage());
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->isNative());
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->isPointer());
  EXPECT_TRUE(mLLVMImmutablePointerOwnerType->isImmutable());
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isController());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isInterface());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isModel());
  EXPECT_FALSE(mLLVMImmutablePointerOwnerType->isNode());
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, createLocalVariableTest) {
  mLLVMImmutablePointerOwnerType->createLocalVariable(mContext, "temp", 0);
  IVariable* variable = mContext.getScopes().getVariable("temp");
  
  ASSERT_NE(variable, nullptr);
  
  *mStringStream << *mDeclareBlock;
  *mStringStream << *mEntryBlock;
  
  string expected =
  "\ndeclare:"
  "\n  %temp = alloca i8*"
  "\n"
  "\nentry:                                            ; No predecessors!"
  "\n  store i8* null, i8** %temp\n";
  
  EXPECT_STREQ(expected.c_str(), mStringStream->str().c_str());
  mStringBuffer.clear();
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMImmutablePointerOwnerType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMImmutablePointerOwnerType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, createParameterVariableTest) {
  llvm::Constant* null =
  ConstantPointerNull::get(mLLVMImmutablePointerOwnerType->getLLVMType(mContext));
  mLLVMImmutablePointerOwnerType->createParameterVariable(mContext, "foo", null, 0);
  IVariable* variable = mContext.getScopes().getVariable("foo");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMImmutablePointerOwnerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer::immutable*", stringStream.str().c_str());
}

TEST_F(LLVMImmutablePointerOwnerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMImmutablePointerOwnerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::i8::pointer::immutable* is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
