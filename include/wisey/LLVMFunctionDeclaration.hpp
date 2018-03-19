//
//  LLVMFunctionDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/19/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMFunctionDeclaration_h
#define LLVMFunctionDeclaration_h

#include "wisey/ILLVMTypeSpecifier.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/LLVMStructType.hpp"

namespace wisey {
  
  /**
   * Declaration of an llvm function
   */
  class LLVMFunctionDeclaration : public IGlobalStatement {
    
    std::string mName;
    const ILLVMTypeSpecifier* mReturnSpecifier;
    std::vector<const ILLVMTypeSpecifier*> mArgumentSpecifiers;
    
  public:
    
    LLVMFunctionDeclaration(std::string name,
                            const ILLVMTypeSpecifier* returnSpecifier,
                            std::vector<const ILLVMTypeSpecifier*> argumentSpecifiers);
    
    ~LLVMFunctionDeclaration();
    
    IObjectType* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMFunctionDeclaration_h */
