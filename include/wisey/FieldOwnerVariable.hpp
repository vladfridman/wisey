//
//  FieldOwnerVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef FieldOwnerVariable_h
#define FieldOwnerVariable_h

#include "wisey/IConcreteObjectType.hpp"
#include "wisey/IFieldVariable.hpp"
#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IOwnerVariable.hpp"

namespace wisey {
  
  /**
   * Represents an object field that is of owner type
   */
  class FieldOwnerVariable : public IFieldVariable, public IOwnerVariable {
    
    std::string mName;
    const IConcreteObjectType* mObject;
    int mLine;

  public:
    
    FieldOwnerVariable(std::string name, const IConcreteObjectType* object, int line);
    
    ~FieldOwnerVariable();
    
    std::string getName() const override;
    
    bool isField() const override;
    
    const IOwnerType* getType() const override;
    
    bool isStatic() const override;
    
    int getLine() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void free(IRGenerationContext& context, llvm::Value* exception, int line) const override;
    
    void setToNull(IRGenerationContext& context, int line) override;
    
  };
  
} /* namespace wisey */

#endif /* FieldOwnerVariable_h */

