//
//  Constant.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 10/19/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef Constant_h
#define Constant_h

#include "wisey/AccessLevel.hpp"
#include "wisey/IExpression.hpp"
#include "wisey/IObjectElement.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IPrintable.hpp"

namespace wisey {
  
  /**
   * Represents a constant defined within an object
   */
  class Constant : public IPrintable, public IObjectElement {
    bool mIsPublic;
    const IType* mType;
    std::string mName;
    IExpression* mExpression;
    int mLine;
    
  public:
    
    Constant(const AccessLevel accessLevel,
             const IType* type,
             std::string name,
             IExpression* expression,
             int line);
    
    ~Constant();
    
    /**
     * Returns global llvm constant name for this constant
     */
    std::string getConstantGlobalName(const IObjectType* objectType) const;

    /**
     * Generates IR defining a global llvm constant for this constant
     */
    llvm::Value* define(IRGenerationContext& context, const IObjectType* objectType) const;

    /**
     * Declare a constant that is defined externally
     */
    llvm::Value* declare(IRGenerationContext& context, const IObjectType* objectType) const;

    std::string getName() const;
    
    const IType* getType() const;
    
    bool isPublic() const;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
    int getLine() const override;
    
    bool isConstant() const override;
    
    bool isField() const override;
    
    bool isMethod() const override;
    
    bool isStaticMethod() const override;
    
    bool isMethodSignature() const override;
    
    bool isLLVMFunction() const override;

  };
  
} /* namespace wisey */

#endif /* Constant_h */

