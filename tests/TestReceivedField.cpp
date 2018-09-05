//
//  TestReceivedField.cpp
//  runtests
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link ReceivedField}
//

#include <gtest/gtest.h>

#include <llvm/Support/raw_ostream.h>

#include "MockConcreteObjectType.hpp"
#include "MockType.hpp"
#include "TestFileRunner.hpp"
#include "TestPrefix.hpp"
#include "wisey/PrimitiveTypes.hpp"
#include "wisey/ReceivedField.hpp"

using namespace llvm;
using namespace std;
using namespace wisey;

using ::testing::_;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct ReceivedFieldTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  NiceMock<MockType>* mType;
  NiceMock<MockConcreteObjectType>* mObject;
  string mName;
  ReceivedField* mField;
  BasicBlock* mBasicBlock;
  Function* mFunction;
  string mStringBuffer;
  raw_string_ostream* mStringStream;

public:
  
  ReceivedFieldTest() :
  mLLVMContext(mContext.getLLVMContext()),
  mType(new NiceMock<MockType>()),
  mObject(new NiceMock<MockConcreteObjectType>()),
  mName("mField") {
    TestPrefix::generateIR(mContext);
    
    ON_CALL(*mType, getTypeName()).WillByDefault(Return("MObject*"));
    ON_CALL(*mType, getLLVMType(_)).WillByDefault(Return(Type::getInt8Ty(mLLVMContext)));
    EXPECT_CALL(*mType, die());
    
    mField = new ReceivedField(mType, mName, 7);

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
  
  ~ReceivedFieldTest() {
    delete mType;
    delete mField;
    delete mObject;
  }
};

TEST_F(ReceivedFieldTest, fieldCreationTest) {
  EXPECT_EQ(mField->getType(), mType);
  EXPECT_STREQ(mField->getName().c_str(), "mField");
  
  EXPECT_FALSE(mField->isInjected());
  EXPECT_TRUE(mField->isReceived());
  EXPECT_FALSE(mField->isState());
}

TEST_F(ReceivedFieldTest, isAssignableTest) {
  ON_CALL(*mObject, isModel()).WillByDefault(Return(false));
  EXPECT_TRUE(mField->isAssignable(mObject));

  ON_CALL(*mObject, isModel()).WillByDefault(Return(true));
  EXPECT_FALSE(mField->isAssignable(mObject));
}

TEST_F(ReceivedFieldTest, elementTypeTest) {
  EXPECT_FALSE(mField->isConstant());
  EXPECT_TRUE(mField->isField());
  EXPECT_FALSE(mField->isMethod());
  EXPECT_FALSE(mField->isStaticMethod());
  EXPECT_FALSE(mField->isMethodSignature());
  EXPECT_FALSE(mField->isLLVMFunction());
}

TEST_F(ReceivedFieldTest, getValueTest) {
  Value* null = ConstantPointerNull::get(mField->getType()->getLLVMType(mContext)->getPointerTo());
  Value* instruction = mField->getValue(mContext, mObject, null, 0);
  
  *mStringStream << *instruction;
  string expected = "  %mField = load i8, i8* null";
  
  ASSERT_STREQ(expected.c_str(), mStringStream->str().c_str());
}

TEST_F(ReceivedFieldTest, checkTypePrimitiveTypeTest) {
  ON_CALL(*mType, isPrimitive()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeImmutableTypeTest) {
  ON_CALL(*mType, isImmutable()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeInterfaceTypeTest) {
  ON_CALL(*mType, isInterface()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeArrayTypeTest) {
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));
  ON_CALL(*mType, isImmutable()).WillByDefault(Return(true));
  ON_CALL(*mType, isOwner()).WillByDefault(Return(true));
  
  EXPECT_NO_THROW(mField->checkType(mContext));
}

TEST_F(ReceivedFieldTest, checkTypeNonImmutableTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Model receive fields must be of primitive or immutable type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ReceivedFieldTest, checkTypeNonImmutableArrayTypeDeathTest) {
  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  ON_CALL(*mType, isArray()).WillByDefault(Return(true));
  
  EXPECT_ANY_THROW(mField->checkType(mContext));
  EXPECT_STREQ("/tmp/source.yz(7): Error: Model receive fields must be of primitive or immutable type\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}

TEST_F(ReceivedFieldTest, fieldPrintToStreamTest) {
  stringstream stringStream;
  mField->printToStream(mContext, stringStream);
  
  EXPECT_STREQ("  receive MObject* mField;\n", stringStream.str().c_str());
}

TEST_F(TestFileRunner, modelImmutableArrayReferenceFieldRunTest) {
  runFile("tests/samples/test_model_immutable_array_reference_field.yz", 5);
}
