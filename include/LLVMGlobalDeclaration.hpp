//
//  LLVMGlobalDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 4/6/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMGlobalDeclaration_h
#define LLVMGlobalDeclaration_h

#include "IGlobalStatement.hpp"
#include "ITypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Things that need to be run before the parsed program code
   */
  class LLVMGlobalDeclaration : public IGlobalStatement {
    
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    
  public:
    
    LLVMGlobalDeclaration(const ITypeSpecifier* typeSpecifier,
                          std::string mName);

    ~LLVMGlobalDeclaration();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;

  };
  
} /* namespace wisey */

#endif /* LLVMGlobalDeclaration_h */
