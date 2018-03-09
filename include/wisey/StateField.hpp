//
//  StateField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef StateField_h
#define StateField_h

#include "wisey/IField.hpp"

namespace wisey {
  
  /**
   * Represents a state field in a concrete object
   */
  class StateField : public IField {
    const IType* mType;
    std::string mName;
    
  public:
    
    StateField(const IType* type, std::string name);
    
    ~StateField();
    
    std::string getName() const override;
    
    const IType* getType() const override;
    
    bool isAssignable() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isFixed() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* StateField_h */
