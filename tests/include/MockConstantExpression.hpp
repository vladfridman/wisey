//
//  MockConstantExpression.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/8/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef MockConstantExpression_h
#define MockConstantExpression_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "IConstantExpression.hpp"
#include "IRGenerationContext.hpp"

/**
 * Defines a mock object for IConstantExpression
 */
class MockConstantExpression : public wisey::IConstantExpression {
public:
  MOCK_CONST_METHOD0(getLine, int ());
  MOCK_CONST_METHOD2(generateIR, llvm::Constant* (wisey::IRGenerationContext&,
                                                  const wisey::IType* assignToType));
  MOCK_CONST_METHOD1(getType, const wisey::IType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD0(isConstant, bool ());
  MOCK_CONST_METHOD0(isAssignable, bool ());
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream&));
};

#endif /* MockConstantExpression_h */
