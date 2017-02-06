//
//  FloatType.hpp
//  Yazyk
//
//  Created by Vladimir Fridman on 2/4/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FloatType_h
#define FloatType_h

#include "yazyk/IType.hpp"

namespace yazyk {
  
/**
 * Represents float expression type
 */
class FloatType : public IType {
    
public:
  
  FloatType() { }
  
  ~FloatType() { }
  
  std::string getName() const override;

  llvm::Type* getLLVMType(llvm::LLVMContext& llvmcontext) const override;
};
  
} /* namespace yazyk */

#endif /* FloatType_h */
