//
//  MockObjectDefinitionStatement.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/29/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockObjectDefinitionStatement_h
#define MockObjectDefinitionStatement_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IObjectDefinitionStatement.hpp"

/**
 * Defines a mock object for IObjectDefinitionStatement
 */
class MockObjectDefinitionStatement : public wisey::IObjectDefinitionStatement {
public:
  MOCK_CONST_METHOD1(prototypeObjects, void (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(generateIR, llvm::Value* (wisey::IRGenerationContext&));
};

#endif /* MockObjectDefinitionStatement_h */
