//
//  OwnerTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef OwnerTypeSpecifier_h
#define OwnerTypeSpecifier_h

#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents an object owner type specifier that is denoted by '*' in the wisey language
 */
class OwnerTypeSpecifier : public ITypeSpecifier {
  
  const IObjectTypeSpecifier* mObjectTypeSpecifier;
  
public:
  
  OwnerTypeSpecifier(const IObjectTypeSpecifier* objectTypeSpecifier)
  : mObjectTypeSpecifier(objectTypeSpecifier) { }
  
  ~OwnerTypeSpecifier();
  
  const IObjectOwnerType* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;

};
  
} /* namespace wisey */

#endif /* OwnerTypeSpecifier_h */
