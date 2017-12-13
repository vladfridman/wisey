//
//  IObjectTypeSpecifier.cpp
//  Wisey
//
//  Created by Vladimir Fridman on 9/25/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#include "wisey/IObjectTypeSpecifier.hpp"
#include "wisey/Log.hpp"

using namespace std;
using namespace wisey;

std::string IObjectTypeSpecifier::getFullName(IRGenerationContext& context,
                                              string shortName,
                                              string package) {
  if (package.length() > 0) {
    return package + "." + shortName;
  }
  
  const IObjectType* objectType = context.getObjectType();
  
  if (objectType && !objectType->getShortName().compare(shortName)) {
    return objectType->getTypeName();
  }
  
  const IObjectType* innerObject = objectType ? objectType->getInnerObject(shortName) : NULL;
  if (innerObject) {
    return innerObject->getTypeName();
  }
  
  ImportProfile* importProfile = objectType
    ? objectType->getImportProfile()
    : context.getImportProfile();
  
  return importProfile->getFullName(shortName);
}

