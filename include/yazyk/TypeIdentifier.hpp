//
//  TypeIdentifier.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 12/12/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef TypeIdentifier_h
#define TypeIdentifier_h

#include <llvm/IR/Type.h>

#include "yazyk/node.hpp"

namespace yazyk {

/**
 * Methods usuful for type manipulation
 */
class TypeIdentifier {
public:
  /**
   * Returns an LLVM type based on the identifier 
   */
  static llvm::Type* typeOf(llvm::LLVMContext &llvmContext, const TypeSpecifier& type);
};

}

#endif /* TypeIdentifier_h */
