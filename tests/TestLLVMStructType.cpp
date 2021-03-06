//
//  TestLLVMStructType.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/18/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link LLVMStructType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Instructions.h>
#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "TestPrefix.hpp"
#include "IRGenerationContext.hpp"
#include "LLVMPrimitiveTypes.hpp"
#include "LLVMStructType.hpp"
#include "ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct LLVMStructTypeTest : public Test {
  
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  StructType* mStructType;
  LLVMStructType* mLLVMStructType;
  LLVMStructType* mAnotherLLVMStructType;
  BasicBlock* mBasicBlock;
  
  LLVMStructTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    StructType* anotherStructType = StructType::create(mLLVMContext, "anotherstruct");
    mAnotherLLVMStructType = LLVMStructType::newLLVMStructType(anotherStructType);
    
    mStructType = StructType::create(mLLVMContext, "mystruct");
    vector<const IType*> structBodyTypes;
    structBodyTypes.push_back(LLVMPrimitiveTypes::I8);
    structBodyTypes.push_back(LLVMPrimitiveTypes::I64);
    structBodyTypes.push_back(mAnotherLLVMStructType);
    mLLVMStructType = LLVMStructType::newLLVMStructType(mStructType);
    mLLVMStructType->setBodyTypes(mContext, structBodyTypes);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mContext.getLLVMContext()), false);
    Function* function = Function::Create(functionType,
                                          GlobalValue::InternalLinkage,
                                          "main",
                                          mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", function);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", function);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
    mContext.getScopes().pushScope();
  }
};

TEST_F(LLVMStructTypeTest, getTypeNameTest) {
  ASSERT_STREQ("::llvm::struct::mystruct", mLLVMStructType->getTypeName().c_str());
}

TEST_F(LLVMStructTypeTest, getLLVMTypeTest) {
  ASSERT_EQ(mStructType, mLLVMStructType->getLLVMType(mContext));
}

TEST_F(LLVMStructTypeTest, isTypeKindTest) {
  EXPECT_FALSE(mLLVMStructType->isPrimitive());
  EXPECT_FALSE(mLLVMStructType->isOwner());
  EXPECT_FALSE(mLLVMStructType->isReference());
  EXPECT_FALSE(mLLVMStructType->isArray());
  EXPECT_FALSE(mLLVMStructType->isFunction());
  EXPECT_FALSE(mLLVMStructType->isPackage());
  EXPECT_TRUE(mLLVMStructType->isNative());
  EXPECT_FALSE(mLLVMStructType->isPointer());
  EXPECT_FALSE(mLLVMStructType->isImmutable());
}

TEST_F(LLVMStructTypeTest, isObjectTest) {
  EXPECT_FALSE(mLLVMStructType->isController());
  EXPECT_FALSE(mLLVMStructType->isInterface());
  EXPECT_FALSE(mLLVMStructType->isModel());
  EXPECT_FALSE(mLLVMStructType->isNode());
}

TEST_F(LLVMStructTypeTest, createFieldVariableTest) {
  NiceMock<MockConcreteObjectType> concreteObjectType;
  IField* field = new ReceivedField(mLLVMStructType, "mField", 0);
  ON_CALL(concreteObjectType, findField(_)).WillByDefault(Return(field));
  mLLVMStructType->createFieldVariable(mContext, "mField", &concreteObjectType, 0);
  IVariable* variable = mContext.getScopes().getVariable("mField");
  
  EXPECT_NE(variable, nullptr);
}

TEST_F(LLVMStructTypeTest, canCastToTest) {
  LLVMStructType* anotherType = LLVMStructType::newLLVMStructType(mStructType);
  
  EXPECT_TRUE(mLLVMStructType->canCastTo(mContext, anotherType));
}

TEST_F(LLVMStructTypeTest, canAutoCastToTest) {
  LLVMStructType* anotherType = LLVMStructType::newLLVMStructType(mStructType);

  EXPECT_TRUE(mLLVMStructType->canAutoCastTo(mContext, anotherType));
}

TEST_F(LLVMStructTypeTest, getPointerTypeTest) {
  const IType* pointerType = mLLVMStructType->getPointerType(mContext, 0);
  EXPECT_EQ(mStructType->getPointerTo(), pointerType->getLLVMType(mContext));
}

TEST_F(LLVMStructTypeTest, printToStreamTest) {
  stringstream stringStream;
  mLLVMStructType->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("external ::llvm::struct mystruct {\n"
               "  ::llvm::i8,\n"
               "  ::llvm::i64,\n"
               "  ::llvm::struct::anotherstruct,\n"
               "}\n",
               stringStream.str().c_str());
}

TEST_F(LLVMStructTypeTest, injectDeathTest) {
  InjectionArgumentList arguments;
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mLLVMStructType->inject(mContext, arguments, 3));
  EXPECT_STREQ("/tmp/source.yz(3): Error: type ::llvm::struct::mystruct is not injectable\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
