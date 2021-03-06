//
//  InjectedFieldDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef InjectedFieldDefinition_h
#define InjectedFieldDefinition_h

#include "IField.hpp"
#include "IObjectElementDefinition.hpp"
#include "ITypeSpecifier.hpp"
#include "InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a field in controller definition
   */
  class InjectedFieldDefinition : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    InjectionArgumentList mInjectionArguments;
    bool mIsImmediate;
    int mLine;
    
    InjectedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                            std::string name,
                            InjectionArgumentList injectionArguments,
                            bool isImmediate,
                            int line);
    
  public:
    
    ~InjectedFieldDefinition();
    
    /**
     * Create an instance of InjectedFieldDefinition where the field is dalyed injected
     */
    static InjectedFieldDefinition* createDelayed(const ITypeSpecifier* typeSpecifier,
                                                  std::string name,
                                                  InjectionArgumentList injectionArguments,
                                                  int line);
    
    /**
     * Create an instance of InjectedFieldDefinition where the field is immediately injected
     */
    static InjectedFieldDefinition* createImmediate(const ITypeSpecifier* typeSpecifier,
                                                    std::string name,
                                                    InjectionArgumentList injectionArguments,
                                                    int line);

    IField* define(IRGenerationContext& context, const IObjectType* objectType) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* InjectedFieldDefinition_h */

