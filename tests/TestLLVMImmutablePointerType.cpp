//
//  TestLLVMImmutablePointerType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/5/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMImmutablePointerType}
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

struct LLVMImmutablePointerTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  BasicBlock* mEntryBlock;
  BasicBlock* mDeclareBlock;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  LLVMPointerType* mLLVMPointerType;
  LLVMImmutablePointerType* mLLVMImmutablePointerType;
  NiceMock<MockConcreteObjectType> mConcreteObjectType;
  
public:
  
  LLVMImmutablePointerTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    mDeclareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mEntryBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(mDeclareBlock);
    mContext.setBasicBlock(mEntryBlock);
    mContext.getScopes().pushScope();
    
    StructType* structType = StructType::create(mLLVMContext, "mystruct");
    
    ON_CALL(mConcreteObjectType, isReference()).WillByDefault(Return(true));
    ON_CALL(mConcreteObjectType, isNative()).WillByDefault(Return(false));
    ON_CALL(mConcreteObjectType, getLLVMType(_)).WillByDefault(Return(structType->getPointerTo()));
    
    mStringStream = new raw_string_ostream(mStringBuffer);
    
    mLLVMPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I8);
    mLLVMImmutablePointerType = new LLVMImmutablePointerType(mLLVMPointerType);
  }
  
  ~LLVMImmutablePointerTypeTest() {
    delete mStringStream;
  }
};

TEST_F(LLVMImmutablePointerTypeTest, pointerTypeTest) {
  EXPECT_EQ(Type::getInt8Ty(mLLVMContext)->getPointerTo(),
            mLLVMImmutablePointerType->getLLVMType(mContext));
  EXPECT_STREQ("::llvm::i8::pointer::immutable", mLLVMImmutablePointerType->getTypeName().c_str());
}

TEST_F(LLVMImmutablePointerTypeTest, canAutoCastToTest) {
  EXPECT_FALSE(mLLVMImmutablePointerType->canAutoCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  LLVMImmutablePointerType* anotherImmutablePointerType =
    new LLVMImmutablePointerType(anotherPointerType);
  EXPECT_FALSE(mLLVMImmutablePointerType->canAutoCastTo(mContext, anotherImmutablePointerType));
  EXPECT_FALSE(mLLVMImmutablePointerType->canAutoCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMImmutablePointerTypeTest, canCastTest) {
  EXPECT_FALSE(mLLVMImmutablePointerType->canCastTo(mContext, LLVMPrimitiveTypes::I8));
  LLVMPointerType* anotherPointerType = LLVMPointerType::create(LLVMPrimitiveTypes::I32);
  LLVMImmutablePointerType* anotherImmutablePointerType =
    new LLVMImmutablePointerType(anotherPointerType);
  EXPECT_FALSE(mLLVMImmutablePointerType->canCastTo(mContext, anotherImmutablePointerType));
  EXPECT_FALSE(mLLVMImmutablePointerType->canCastTo(mContext, &mConcreteObjectType));
}

TEST_F(LLVMImmutablePointerTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMImmutablePointerType->isPrimitive());
  EXPECT_FALSE(mLLVMImmutablePointerType->isOwner());
  EXPECT_FALSE(mLLVMImmutablePointerType->isReference());
  EXPECT_FALSE(mLLVMImmutablePointerType->isArray());
  EXPECT_FALSE(mLLVMImmutablePointerType->isFunction());
  EXPECT_FALSE(mLLVMImmutablePointerType->isPackage());
  EXPECT_TRUE(mLLVMImmutablePointerType->isNative());
  EXPECT_TRUE(mLLVMImmutablePointerType->isPointer());
  EXPECT_TRUE(mLLVMImmutablePointerType->isImmutable());
}

TEST_F(LLVMImmutablePointerTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMImmutablePointerType->isController());
  EXPECT_FALSE(mLLVMImmutablePointerType->isInterface());
  EXPECT_FALSE(mLLVMImmutablePointerType->isModel());
  EXPECT_FALSE(mLLVMImmutablePointerType->isNode());
}

TEST_F(LLVMImmutablePointerTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new StateField(mLLVMImmutablePointerType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMImmutablePointerType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMImmutablePointerTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMImmutablePointerType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("::llvm::i8::pointer::immutable", stringStream.str().c_str());
}

TEST_F(LLVMImmutablePointerTypeTest, getOwnerTest) {
  EXPECT_NE(nullptr, mLLVMImmutablePointerType->getOwner());
  EXPECT_STREQ("::llvm::i8::pointer::immutable*",
               mLLVMImmutablePointerType->getOwner()->getTypeName().c_str());
}

TEST_F(LLVMImmutablePointerTypeTest, injectDeathTest) {
  ::Mock::AllowLeak(&mConcreteObjectType);
  
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMImmutablePointerType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::i8::pointer::immutable is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
