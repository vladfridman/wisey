//
//  FixedField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef FixedField_h
#define FixedField_h

#include "wisey/IField.hpp"

namespace wisey {
  
  /**
   * Represents a fixed field in a concrete object
   */
  class FixedField : public IField {
    const IType* mType;
    std::string mName;
    int mLine;
    
  public:
    
    FixedField(const IType* type, std::string name, int line);
    
    ~FixedField();
    
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

#endif /* FixedField_h */

