//
//  LLVMExternalStructDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/21/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMExternalStructDefinition_h
#define LLVMExternalStructDefinition_h

#include "IGlobalStatement.hpp"
#include "ITypeSpecifier.hpp"
#include "LLVMStructType.hpp"

namespace wisey {
  
  /**
   * Definition of an llvm struct in an imported library
   */
  class LLVMExternalStructDefinition : public IGlobalStatement {
    
    std::string mName;
    std::vector<const ITypeSpecifier*> mTypeSpecifiers;
    int mLine;
    
  public:
    
    LLVMExternalStructDefinition(std::string name,
                                 std::vector<const ITypeSpecifier*> typesSpecifiers,
                                 int line);
    
    ~LLVMExternalStructDefinition();
    
    IObjectType* prototypeObject(IRGenerationContext& context,
                                 ImportProfile* importProfile) const override;
    
    void prototypeMethods(IRGenerationContext& context) const override;
    
    void generateIR(IRGenerationContext& context) const override;
    
  };
  
} /* namespace wisey */

#endif /* LLVMExternalStructDefinition_h */
