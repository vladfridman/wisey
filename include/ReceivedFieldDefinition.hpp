//
//  ReceivedFieldDefinition.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReceivedFieldDefinition_h
#define ReceivedFieldDefinition_h

#include "IField.hpp"
#include "IObjectElementDefinition.hpp"
#include "ITypeSpecifier.hpp"
#include "InjectionArgument.hpp"

namespace wisey {
  
  class Field;
  
  /**
   * Represents a state field declaration in object definition
   */
  class ReceivedFieldDefinition : public IObjectElementDefinition {
    const ITypeSpecifier* mTypeSpecifier;
    std::string mName;
    bool mIsImplied;
    int mLine;
    
    ReceivedFieldDefinition(const ITypeSpecifier* typeSpecifier,
                            std::string name,
                            bool isImplied,
                            int line);
    
  public:
    
    ~ReceivedFieldDefinition();
    
    /**
     * Creates an instance of ReceivedFieldDefinition
     */
    static ReceivedFieldDefinition* create(const ITypeSpecifier* typeSpecifier,
                                           std::string name,
                                           int line);
    
    /**
     * Creates an instance of ReceivedFieldDefinition where the receive kind is implied
     */
    static ReceivedFieldDefinition* createImplied(const ITypeSpecifier* typeSpecifier,
                                                  std::string name,
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

#endif /* ReceivedFieldDefinition_h */

