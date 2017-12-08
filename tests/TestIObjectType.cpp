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
#include "wisey/IObjectType.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IObjectTypeTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
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
  ON_CALL(toObject, getAccessLevel()).WillByDefault(Return(AccessLevel::PUBLIC_ACCESS));

  EXPECT_TRUE(IObjectType::checkAccess(&fromObject, &toObject));
}

TEST_F(IObjectTypeTest, checkAccessFromAndToAreSameObjectTest) {
  NiceMock<MockObjectType> object;
  ON_CALL(object, getAccessLevel()).WillByDefault(Return(AccessLevel::PRIVATE_ACCESS));

  EXPECT_TRUE(IObjectType::checkAccess(&object, &object));
}

TEST_F(IObjectTypeTest, checkAccessToIsInnerOfFromTest) {
  NiceMock<MockObjectType> toObject;
  ON_CALL(toObject, getAccessLevel()).WillByDefault(Return(AccessLevel::PRIVATE_ACCESS));
  NiceMock<MockObjectType> fromObject;
  ON_CALL(fromObject, getAccessLevel()).WillByDefault(Return(AccessLevel::PUBLIC_ACCESS));
  ON_CALL(fromObject, getInnerObject(_)).WillByDefault(Return(&toObject));
  
  EXPECT_TRUE(IObjectType::checkAccess(&fromObject, &toObject));
}

TEST_F(IObjectTypeTest, checkAccessToIsNotAccessableDeathTest) {
  NiceMock<MockObjectType> toObject;
  ON_CALL(toObject, getAccessLevel()).WillByDefault(Return(AccessLevel::PRIVATE_ACCESS));
  ON_CALL(toObject, getName()).WillByDefault(Return("MToObject"));
  NiceMock<MockObjectType> fromObject;
  ON_CALL(fromObject, getAccessLevel()).WillByDefault(Return(AccessLevel::PUBLIC_ACCESS));
  ON_CALL(fromObject, getName()).WillByDefault(Return("MFromObject"));

  EXPECT_EXIT(IObjectType::checkAccess(&fromObject, &toObject),
              ::testing::ExitedWithCode(1),
              "Error: Object MToObject is not accessable from object MFromObject");
}
