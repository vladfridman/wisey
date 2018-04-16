//
//  InjectedField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef InjectedField_h
#define InjectedField_h

#include "wisey/IField.hpp"
#include "wisey/InjectionArgument.hpp"

namespace wisey {
  
  /**
   * Represents an injected field in a concrete object
   */
  class InjectedField : public IField {
    const IType* mType;
    const IType* mInjectedType;
    bool mIsArrayType;
    std::string mName;
    InjectionArgumentList mInjectionArgumentList;
    std::string mSourceFileName;
    int mLine;
    
  public:
    
    InjectedField(const IType* type,
                  const IType* injectedType,
                  std::string name,
                  InjectionArgumentList injectionArgumentList,
                  std::string sourceFileName,
                  int line);
    
    ~InjectedField();
    
    /**
     * Injects the field type and returns the injected value
     */
    llvm::Value* inject(IRGenerationContext& context) const;
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    int getLine() const override;
    
    bool isAssignable() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

    bool isFixed() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* InjectedField_h */

