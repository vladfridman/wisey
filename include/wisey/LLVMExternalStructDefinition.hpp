//
//  LLVMExternalStructDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMExternalStructDefinition_h
#define LLVMExternalStructDefinition_h

#include "wisey/IGlobalStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/LLVMStructType.hpp"

namespace wisey {
  
  /**
   * Definition of an llvm struct in an imported library
   */
  class LLVMExternalStructDefinition : public IGlobalStatement {
    
    std::string mName;
    std::vector<const ITypeSpecifier*> mTypeSpecifiers;
    
  public:
    
    LLVMExternalStructDefinition(std::string name,
                                 std::vector<const ITypeSpecifier*> typesSpecifiers);
    
    ~LLVMExternalStructDefinition();
    
    IObjectType* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMExternalStructDefinition_h */
