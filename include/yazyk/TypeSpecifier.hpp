//
//  TypeSpecifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef TypeSpecifier_h
#define TypeSpecifier_h

#include "yazyk/IRGenerationContext.hpp"

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
class TypeSpecifier {
  PrimitiveType mType;
  
public:
  TypeSpecifier(PrimitiveType type) : mType(type) { }
  
  ~TypeSpecifier() { }
  
  llvm::Type* getType(llvm::LLVMContext& llvmContext) const;
};
  
} /* namespace yazyk */

#endif /* TypeSpecifier_h */
