//
//  TestIObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IObjectType}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectType.hpp"
#include "TestPrefix.hpp"
#include "IObjectType.hpp"
#include "IRGenerationContext.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    llvm::Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, "test");
    new GlobalVariable(*mContext.getModule(),
                       stringConstant->getType(),
                       true,
                       GlobalValue::LinkageTypes::LinkOnceODRLinkage,
                       stringConstant,
                       "test");

    mStringStream = new raw_string_ostream(mStringBuffer);
  }
};

TEST_F(IObjectTypeTest, getObjectNamePointerTest) {
  NiceMock<MockObjectType> mockObject;
  ON_CALL(mockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("test"));

  llvm::Constant* constant = IObjectType::getObjectNamePointer(&mockObject, mContext);
  
  *mStringStream << *constant;
  string expected = "i8* getelementptr inbounds ([5 x i8], [5 x i8]* @test, i32 0, i32 0)";
  
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}

TEST_F(IObjectTypeTest, checkAccessToObjectIsPublicTest) {
  NiceMock<MockObjectType> fromObject;
  NiceMock<MockObjectType> toObject;
  ON_CALL(toObject, isPublic()).WillByDefault(Return(true));

  EXPECT_TRUE(IObjectType::checkAccess(&mContext, "", &fromObject, &toObject, 0));
}

TEST_F(IObjectTypeTest, checkAccessFromAndToAreSameObjectTest) {
  NiceMock<MockObjectType> object;
  ON_CALL(object, isPublic()).WillByDefault(Return(false));

  EXPECT_TRUE(IObjectType::checkAccess(&mContext, "", &object, &object, 0));
}

TEST_F(IObjectTypeTest, checkAccessToIsInnerOfFromTest) {
  NiceMock<MockObjectType>* toObject = new NiceMock<MockObjectType>();
  ON_CALL(*toObject, isPublic()).WillByDefault(Return(false));
  ON_CALL(*toObject, getTypeName()).WillByDefault(Return("wisey.test.MFromObject.MToObject"));
  NiceMock<MockObjectType>* fromObject = new NiceMock<MockObjectType>();
  ON_CALL(*fromObject, isPublic()).WillByDefault(Return(true));
  ON_CALL(*fromObject, getTypeName()).WillByDefault(Return("wisey.test.MFromObject"));
  mContext.addModel((Model*) fromObject, 0);
  mContext.addModel((Model*) toObject, 0);

  EXPECT_TRUE(IObjectType::checkAccess(&mContext, "", fromObject, toObject, 0));
}

TEST_F(IObjectTypeTest, checkAccessToIsNotAccessableDeathTest) {
  NiceMock<MockObjectType>* toObject = new NiceMock<MockObjectType>();
  ON_CALL(*toObject, isPublic()).WillByDefault(Return(false));
  ON_CALL(*toObject, getTypeName()).WillByDefault(Return("wisey.test.MToObject"));
  NiceMock<MockObjectType>* fromObject = new NiceMock<MockObjectType>();
  ON_CALL(*fromObject, isPublic()).WillByDefault(Return(true));
  ON_CALL(*fromObject, getTypeName()).WillByDefault(Return("wisey.test.MFromObject"));
  mContext.addModel((Model*) fromObject, 0);
  mContext.addModel((Model*) toObject, 0);

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(IObjectType::checkAccess(&mContext,
                                            "/tmp/source.yz",
                                            fromObject,
                                            toObject,
                                            1));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Object wisey.test.MToObject is not accessable from object wisey.test.MFromObject\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
