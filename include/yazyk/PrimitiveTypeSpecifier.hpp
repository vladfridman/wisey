//
//  PrimitiveTypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef PrimitiveTypeSpecifier_h
#define PrimitiveTypeSpecifier_h

#include "yazyk/IRGenerationContext.hpp"
#include "yazyk/ITypeSpecifier.hpp"

namespace yazyk {

/**
 * Represents a yazyk primitive type
 */
class PrimitiveTypeSpecifier : public ITypeSpecifier {
  IType* mType;
  
public:
  
  PrimitiveTypeSpecifier(IType* type) : mType(type) { }
  
  ~PrimitiveTypeSpecifier() { }
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  VariableStorageType getStorageType() const override;
};
  
} /* namespace yazyk */

#endif /* PrimitiveTypeSpecifier_h */
