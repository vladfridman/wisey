//
//  TestIObjectDefinition.cpp
//  runtests
//
//  Created by Vladimir Fridman on 12/8/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IObjectDefinition}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "MockObjectType.hpp"
#include "wisey/IObjectDefinition.hpp"
#include "wisey/IRGenerationContext.hpp"
#include "wisey/ModelTypeSpecifier.hpp"

using namespace std;
using namespace llvm;
using namespace wisey;

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

struct IObjectDefinitionTest : public Test {
  NiceMock<MockObjectType> mMockObject;
  IRGenerationContext mContext;
  ModelTypeSpecifier* mModelTypeSpecifier;
  
  IObjectDefinitionTest() {
    ON_CALL(mMockObject, getTypeName()).WillByDefault(Return("wisey.lang.CObject"));
    
    mModelTypeSpecifier = new ModelTypeSpecifier("", "MSubObject");
    mContext.setObjectType(&mMockObject);
  }
};

TEST_F(IObjectDefinitionTest, getFullNameTest) {
  string name = IObjectDefinition::getFullName(mContext, mModelTypeSpecifier);
  
  ASSERT_STREQ("wisey.lang.CObject.MSubObject", name.c_str());
}
