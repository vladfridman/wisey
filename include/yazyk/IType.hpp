//
//  IType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/3/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IType_h
#define IType_h

#include <string>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Type.h>

namespace yazyk {

/**
 * Interface representing expression type in Yazyk language
 */
class IType {
  
public:
  
  virtual ~IType() { }
  
  /**
   * Returns type name
   */
  virtual std::string getName() const = 0;
  
  /**
   * Return corresponding LLVM type
   */
  virtual llvm::Type* getLLVMType(llvm::LLVMContext& llvmContext) const = 0;
};

} /* namespace yazyk */

#endif /* IType_h */
