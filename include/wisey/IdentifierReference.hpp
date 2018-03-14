//
//  IdentifierReference.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IdentifierReference_h
#define IdentifierReference_h

#include "wisey/IExpression.hpp"
#include "wisey/IMethodDescriptor.hpp"
#include "wisey/IVariable.hpp"

namespace wisey {
  
  /**
   * Represents an identifier reference. It is only used for native code calling.
   */
  class IdentifierReference : public IExpression {
    
    const std::string mName;
    
  public:
    
    IdentifierReference(const std::string& name);
    
    ~IdentifierReference();
    
    const std::string& getIdentifierName() const;
    
    IVariable* getVariable(IRGenerationContext& context,
                           std::vector<const IExpression*>& arrayIndices) const override;
    
    llvm::Value* generateIR(IRGenerationContext& context, const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    IMethodDescriptor* getMethod(IRGenerationContext& context) const;
    
  };
  
} /* namespace wisey */

#endif /* IdentifierReference_h */

