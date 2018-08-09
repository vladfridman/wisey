//
//  ConstantReference.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/20/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ConstantReference_h
#define ConstantReference_h

#include "wisey/IConstantExpression.hpp"
#include "wisey/IObjectTypeSpecifier.hpp"

namespace wisey {
  
  /**
   * Represents a constant reference that looks like my.packge.MMyObject.MYCONSTANT
   */
  class ConstantReference : public IConstantExpression {
    
    const IObjectTypeSpecifier* mObjectTypeSpecifier;
    std::string mConstantName;
    int mLine;
    
  public:
    ConstantReference(const IObjectTypeSpecifier* objectTypeSpecifier,
                      std::string constantName,
                      int line);
    
    ~ConstantReference();
    
    int getLine() const override;

    llvm::Constant* generateIR(IRGenerationContext& context,
                               const IType* assignToType) const override;
    
    const IType* getType(IRGenerationContext& context) const override;
    
    bool isConstant() const override;
    
    bool isAssignable() const override;

    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  private:
    
    const IObjectType* getObjectType(IRGenerationContext& context) const;
  };
  
} /* namespace wisey */

#endif /* ConstantReference_h */

