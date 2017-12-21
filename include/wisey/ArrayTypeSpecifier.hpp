//
//  ArrayTypeSpecifier.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/11/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ArrayTypeSpecifier_h
#define ArrayTypeSpecifier_h

#include "wisey/IRGenerationContext.hpp"
#include "wisey/IType.hpp"
#include "wisey/ITypeSpecifier.hpp"

namespace wisey {
  
/**
 * Represents a wisey array type
 */
class ArrayTypeSpecifier : public ITypeSpecifier {
  ITypeSpecifier* mBaseTypeSpecifier;
  unsigned long mSize;
  
public:
  
  ArrayTypeSpecifier(ITypeSpecifier* baseTypeSpecifier, unsigned long arraySize);
  
  ~ArrayTypeSpecifier();
  
  ArrayType* getType(IRGenerationContext& context) const override;
  
  void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
  
};

} /* namespace wisey */

#endif /* ArrayTypeSpecifier_h */


