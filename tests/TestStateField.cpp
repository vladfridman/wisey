//
//  TestStateField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link StateField}
//

#include <gtest/gtest.h>

#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "PrimitiveTypes.hpp"
#include "StateField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct StateFieldTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockType>* mType;
  NiceMock<MockConcreteObjectType>* mObjectType;
  string mName;
  StateField* mField;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  StateFieldTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mType(new NiceMock<MockType>()),
  mObjectType(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mType, getTypeName()).WillByDefault(Return("MObject*"));
    ON_CALL(*mType, getLLVMType(_)).WillByDefault(Return(Type::getInt8Ty(mLLVMContext)));
    EXPECT_CALL(*mType, die());
    
    mField = new StateField(mType, mName, 5);

    mStringStream = new raw_string_ostream(mStringBuffer);
    
    FunctionType* functionType =
    FunctionType::get(Type::getInt32Ty(mLLVMContext), false);
    mFunction = Function::Create(functionType,
                                 GlobalValue::InternalLinkage,
                                 "main",
                                 mContext.getModule());
    BasicBlock* declareBlock = BasicBlock::Create(mLLVMContext, "declare", mFunction);
    mBasicBlock = BasicBlock::Create(mLLVMContext, "entry", mFunction);
    mContext.setDeclarationsBlock(declareBlock);
    mContext.setBasicBlock(mBasicBlock);
  }
  
  ~StateFieldTest() {
    delete mType;
    delete mObjectType;
    delete mField;
  }
};

TEST_F(StateFieldTest, fieldCreationTest) {
  EXPECT_EQ(mField->getType(), mType);
  EXPECT_STREQ(mField->getName().c_str(), "mField");
  EXPECT_TRUE(mField->isAssignable(mObjectType));
  
  EXPECT_FALSE(mField->isInjected());
  EXPECT_FALSE(mField->isReceived());
  EXPECT_TRUE(mField->isState());
}

TEST_F(StateFieldTest, getValueTest) {
  Value* null = ConstantPointerNull::get(mField->getType()->getLLVMType(mContext)->getPointerTo());
  Value* instruction = mField->getValue(mContext, mObjectType, null, 0);
  
  *mStringStream << *instruction;
  string expected = "  %mField = load i8, i8* null";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(StateFieldTest, elementTypeTest) {
  EXPECT_FALSE(mField->isConstant());
  EXPECT_TRUE(mField->isField());
  EXPECT_FALSE(mField->isMethod());
  EXPECT_FALSE(mField->isStaticMethod());
  EXPECT_FALSE(mField->isMethodSignature());
  EXPECT_FALSE(mField->isLLVMFunction());
}

TEST_F(StateFieldTest, fieldPrintToStreamTest) {
  stringstream stringStream;
  mField->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  state MObject* mField;\n", stringStream.str().c_str());
}

TEST_F(StateFieldTest, checkTypeNonNodeTest) {
  EXPECT_NO_THROW(mField->checkType(mContext, mObjectType));
}

TEST_F(StateFieldTest, checkTypeNodeOwnerInNodeTest) {
  ON_CALL(*mObjectType, isNode()).WillByDefault(Return(true));
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));
  ON_CALL(*mType, isNode()).WillByDefault(Return(true));

  EXPECT_NO_THROW(mField->checkType(mContext, mObjectType));
}

TEST_F(StateFieldTest, checkTypeInterfaceOwnerInNodeTest) {
  ON_CALL(*mObjectType, isNode()).WillByDefault(Return(true));
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));
  ON_CALL(*mType, isInterface()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext, mObjectType));
}

TEST_F(StateFieldTest, checkTypeNodeReferenceInNodeTest) {
  ON_CALL(*mObjectType, isNode()).WillByDefault(Return(true));
  ON_CALL(*mType, isNode()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext, mObjectType));
}
