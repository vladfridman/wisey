//
//  ReceivedField.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/9/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef ReceivedField_h
#define ReceivedField_h

#include "wisey/IField.hpp"

namespace wisey {
  
  /**
   * Represents a receive field in a concrete object
   */
  class ReceivedField : public IField {
    const IType* mType;
    std::string mName;
    int mLine;
    
  public:
    
    ReceivedField(const IType* type, std::string name, int line);
    
    ~ReceivedField();
    
    /**
     * Checks that field type is of primitive, model, or immutable array type for models
     */
    void checkType(IRGenerationContext& context) const;

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

    bool isFixed() const override;
    
    bool isInjected() const override;
    
    bool isReceived() const override;
    
    bool isState() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
} /* namespace wisey */

#endif /* ReceivedField_h */

