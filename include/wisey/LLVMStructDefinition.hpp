//
//  LLVMStructDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/17/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMStructDefinition_h
#define LLVMStructDefinition_h

#include "wisey/ITypeSpecifier.hpp"
#include "wisey/IGlobalStatement.hpp"
#include "wisey/LLVMStructType.hpp"

namespace wisey {
  
  /**
   * Definition of an llvm struct
   */
  class LLVMStructDefinition : public IGlobalStatement {
    
    std::string mName;
    std::vector<const ITypeSpecifier*> mTypeSpecifiers;
    int mLine;
    
  public:
    
    LLVMStructDefinition(std::string name,
                         std::vector<const ITypeSpecifier*> typesSpecifiers,
                         int line);
    
    ~LLVMStructDefinition();
    
    IObjectType* prototypeObject(IRGenerationContext& context) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;

    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMStructDefinition_h */
