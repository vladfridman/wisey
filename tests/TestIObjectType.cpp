//
//  TestIObjectType.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/13/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IObjectType}

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectWithMethodsType.hpp"
#include "wisey/IObjectWithMethodsType.hpp"
#include "wisey/IRGenerationContext.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IObjectTypeTest : public Test {
  NiceMock<MockObjectWithMethodsType> mMockObject;
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  string mStringBuffer;
  raw_string_ostream* mStringStream;
  
  IObjectTypeTest() : mLLVMContext(mContext.getLLVMContext()) {
    ON_CALL(mMockObject, getObjectNameGlobalVariableName()).WillByDefault(Return("test"));
    
    Constant* stringConstant = ConstantDataArray::getString(mLLVMContext, "test");
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
  Constant* constant = IObjectType::getObjectNamePointer(&mMockObject, mContext);
  
  *mStringStream << *constant;
  string expected = "i8* getelementptr inbounds ([5 x i8], [5 x i8]* @test, i32 0, i32 0)";
  
  ASSERT_STREQ(mStringStream->str().c_str(), expected.c_str());
}
