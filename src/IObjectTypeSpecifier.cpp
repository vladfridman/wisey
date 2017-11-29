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
  if (context.getImport(shortName) != NULL) {
    return context.getImport(shortName)->getName();
  }
  if (context.getPackage().size() != 0) {
    return context.getPackage() + "." + shortName;
  }
  
  Log::e("Could not identify packge for object " + shortName);
  exit(1);
}

