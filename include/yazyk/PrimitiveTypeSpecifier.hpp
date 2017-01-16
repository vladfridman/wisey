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
  PRIMITIVE_TYPE_INT32,
  PRIMITIVE_TYPE_INT64,
  PRIMITIVE_TYPE_FLOAT32,
  PRIMITIVE_TYPE_FLOAT64,
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
  
  llvm::Type* getType(IRGenerationContext& context) const;
};
  
} /* namespace yazyk */

#endif /* PrimitiveTypeSpecifier_h */
