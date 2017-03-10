//
//  TestIMethodDescriptor.cpp
//  Yazyk
//
//  Created by Vladimir Fridman on 3/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IMethodDescriptor}
//

#include <gtest/gtest.h>

#include <llvm/IR/Constants.h>
#include <llvm/Support/raw_ostream.h>

#include "yazyk/Method.hpp"
#include "yazyk/MethodArgument.hpp"
#include "yazyk/PrimitiveTypes.hpp"

using namespace llvm;
using namespace std;
using namespace yazyk;

using ::testing::Test;

struct IMethodDescriptorTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  Method* mMethod;
  
public:
  
  IMethodDescriptorTest() : mLLVMContext(mContext.getLLVMContext()) {
    
    MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble");
    MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar");
    std::vector<MethodArgument*> arguments;
    arguments.push_back(doubleArgument);
    arguments.push_back(charArgument);
    mMethod = new Method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  }
};

TEST_F(IMethodDescriptorTest, CompareTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_TRUE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, NameNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::CHAR_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("differentname", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, NumberOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

TEST_F(IMethodDescriptorTest, TypeOfArgumentsNotEqualsTest) {
  MethodArgument* doubleArgument = new MethodArgument(PrimitiveTypes::DOUBLE_TYPE, "argDouble2");
  MethodArgument* charArgument = new MethodArgument(PrimitiveTypes::INT_TYPE, "argChar2");
  std::vector<MethodArgument*> arguments;
  arguments.push_back(doubleArgument);
  arguments.push_back(charArgument);
  Method method("mymethod", PrimitiveTypes::BOOLEAN_TYPE, arguments, 0, NULL);
  
  ASSERT_FALSE(IMethodDescriptor::compare(&method, mMethod));
}

