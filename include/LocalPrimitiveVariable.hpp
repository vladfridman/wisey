//
//  LocalPrimitiveVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 2/9/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef LocalPrimitiveVariable_h
#define LocalPrimitiveVariable_h

#include "IPrimitiveType.hpp"
#include "IVariable.hpp"

namespace wisey {
  
  /**
   * Represents a local variable of primitive type
   */
  class LocalPrimitiveVariable : public IVariable {
    
    std::string mName;
    const IPrimitiveType* mType;
    llvm::Value* mValueStore;
    int mLine;

  public:
    
    LocalPrimitiveVariable(std::string name,
                           const IPrimitiveType* type,
                           llvm::Value* valueStore,
                           int line);
    
    ~LocalPrimitiveVariable();
    
    std::string getName() const override;
    
    const IPrimitiveType* getType() const override;
    
    bool isField() const override;
    
    bool isStatic() const override;
    
    int getLine() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
  };
  
} /* namespace wisey */

#endif /* LocalPrimitiveVariable_h */

