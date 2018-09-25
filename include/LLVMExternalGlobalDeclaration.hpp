//
//  LLVMExternalGlobalDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/16/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMExternalGlobalDeclaration_h
#define LLVMExternalGlobalDeclaration_h

#include "IGlobalStatement.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Externally defined global variable
   */
  class LLVMExternalGlobalDeclaration : public IGlobalStatement {
    
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    
  public:
    
    LLVMExternalGlobalDeclaration(const ITypeSpecifier* typeSpecifier,
                                  std::string mName);
    
    ~LLVMExternalGlobalDeclaration();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMExternalGlobalDeclaration_h */
