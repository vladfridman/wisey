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
 * Prmitive types used in yazyk
 */
typedef enum PrimitiveTypeEnum {
  PRIMITIVE_TYPE_BOOLEAN,
  PRIMITIVE_TYPE_INT,
  PRIMITIVE_TYPE_LONG,
  PRIMITIVE_TYPE_FLOAT,
  PRIMITIVE_TYPE_DOUBLE,
  PRIMITIVE_TYPE_VOID,
} PrimitiveType;

/**
 * Represents a yazyk primitive type
 */
class PrimitiveTypeSpecifier : public ITypeSpecifier {
  PrimitiveType mType;
  
public:
  PrimitiveTypeSpecifier(PrimitiveType type) : mType(type) { }
  
  ~PrimitiveTypeSpecifier() { }
  
  llvm::Type* getLLVMType(IRGenerationContext& context) const override;
  
  VariableStorageType getStorageType() const override;
};
  
} /* namespace yazyk */

#endif /* PrimitiveTypeSpecifier_h */
