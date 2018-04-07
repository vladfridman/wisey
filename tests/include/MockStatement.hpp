//
//  MockStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockStatement_h
#define MockStatement_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IStatement.hpp"

/**
 * Defines a mock object for IStatement
 */
class MockStatement : public wisey::IStatement {
public:
  MOCK_CONST_METHOD1(generateIR, void (wisey::IRGenerationContext&));
};

#endif /* MockStatement_h */
