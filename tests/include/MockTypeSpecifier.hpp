//
//  MockTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/26/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockTypeSpecifier_h
#define MockTypeSpecifier_h

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "wisey/IRGenerationContext.hpp"
#include "wisey/ITypeSpecifier.hpp"

/**
 * Defines a mock object for ITypeSpecifier
 */
class MockTypeSpecifier : public wisey::ITypeSpecifier {
public:
  MOCK_CONST_METHOD1(getType, wisey::IType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream&));
};

#endif /* MockTypeSpecifier_h */
