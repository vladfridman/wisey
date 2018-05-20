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
#include "wisey/IObjectType.hpp"

namespace wisey {
  
  /**
   * Represents a state field in a concrete object
   */
  class StateField : public IField {
    const IType* mType;
    std::string mName;
    int mLine;
    
  public:
    
    StateField(const IType* type, std::string name, int line);
    
    ~StateField();

    /**
     * Checks that if the field is part of a node then it is of node owner type
     */
    void checkType(IRGenerationContext& context, const IObjectType* object) const;

    std::string getName() const override;
    
    const IType* getType() const override;
    
    int getLine() const override;
    
    bool isAssignable(const IConcreteObjectType* object) const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;
    
    std::string getFieldKind() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* StateField_h */

