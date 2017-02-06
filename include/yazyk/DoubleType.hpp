//
//  DoubleType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef DoubleType_h
#define DoubleType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents double expression type
 */
class DoubleType : public IType {
    
public:
  
  DoubleType() { }
  
  ~DoubleType() { }
  
  std::string getName() const override;
  
  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
  
  TypeKind getTypeKind() const override;
};
  
} /* namespace yazyk */

#endif /* DoubleType_h */
