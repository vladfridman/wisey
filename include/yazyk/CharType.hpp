//
//  CharType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef CharType_h
#define CharType_h

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

#include "yazyk/IType.hpp"

namespace yazyk {

/**
 * Represents char expression type
 */
class CharType : public IType {
  
public:
  
  CharType() { }
  
  ~CharType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
};

} /* namespace yazyk */

#endif /* CharType_h */
