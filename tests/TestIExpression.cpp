//
//  TestIExpression.cpp
//  runtests
//
//  Created by Vladimir Fridman on 9/7/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//
//  Tests {@link IExpression}
//

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <llvm/IR/Instructions.h>

#include "MockExpression.hpp"
#include "TestPrefix.hpp"
#include "IExpression.hpp"
#include "IRGenerationContext.hpp"
#include "UndefinedType.hpp"

using ::testing::_;
using ::testing::Invoke;
using ::testing::Mock;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Test;

using namespace llvm;
using namespace std;
using namespace wisey;

struct IExpressionTest : public Test {
  IRGenerationContext mContext;
  LLVMContext& mLLVMContext;
  
  IExpressionTest() : mLLVMContext(mContext.getLLVMContext()) {
    TestPrefix::generateIR(mContext);
    
    mContext.getScopes().pushScope();
  }
  
  ~IExpressionTest() {
  }

  static void printExpression(IRGenerationContext& context, iostream& stream) {
    stream << "expression";
  }
};

TEST_F(IExpressionTest, checkForUndefinedTest) {
  NiceMock<MockExpression> mockExpression;
  ON_CALL(mockExpression, getType(_)).WillByDefault(Return(UndefinedType::UNDEFINED));
  ON_CALL(mockExpression, getLine()).WillByDefault(Return(1));
  ON_CALL(mockExpression, printToStream(_, _)).WillByDefault(Invoke(printExpression));

  std::stringstream buffer;
  std::streambuf* oldbuffer = std::cerr.rdbuf(buffer.rdbuf());
  
  EXPECT_ANY_THROW(IExpression::checkForUndefined(mContext, &mockExpression));
  EXPECT_STREQ("/tmp/source.yz(1): Error: Undefined expression 'expression'\n",
               buffer.str().c_str());
  std::cerr.rdbuf(oldbuffer);
}
