//
//  MockObjectTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/10/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef MockObjectTypeSpecifier_h
#define MockObjectTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

/**
 * Defines a mock object for IObjectTypeSpecifier
 */
class MockObjectTypeSpecifier : public wisey::IObjectTypeSpecifier {
public:
  MOCK_CONST_METHOD0(getShortName, std::string ());
  MOCK_CONST_METHOD1(getName, std::string (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD1(getType, wisey::IObjectType* (wisey::IRGenerationContext&));
  MOCK_CONST_METHOD2(printToStream, void (wisey::IRGenerationContext&, std::iostream&));
};

#endif /* MockObjectTypeSpecifier_h */
