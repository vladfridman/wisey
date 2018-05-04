//
//  LocalSystemReferenceVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/22/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LocalSystemReferenceVariable_h
#define LocalSystemReferenceVariable_h

#include "wisey/IExpression.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IReferenceVariable.hpp"

namespace wisey {
  
  /**
   * Represents a reference variable to a system object e.g. thread and callstack
   */
  class LocalSystemReferenceVariable : public IReferenceVariable {
    
    std::string mName;
    const IObjectType* mType;
    llvm::Value* mValueStore;
    bool mIsInitialized;
    int mLine;

  public:
    
    LocalSystemReferenceVariable(std::string name,
                                 const IObjectType* type,
                                 llvm::Value* value,
                                 int line);
    
    ~LocalSystemReferenceVariable();
    
    std::string getName() const override;
    
    const IObjectType* getType() const override;
    
    bool isField() const override;
    
    bool isSystem() const override;
    
    int getLine() const override;

    llvm::Value* generateIdentifierIR(IRGenerationContext& context, int line) const override;
    
    llvm::Value* generateIdentifierReferenceIR(IRGenerationContext& context,
                                               int line) const override;
    
    llvm::Value* generateAssignmentIR(IRGenerationContext& context,
                                      IExpression* assignToExpression,
                                      std::vector<const IExpression*> arrayIndices,
                                      int line) override;
    
    void decrementReferenceCounter(IRGenerationContext& context, int line) const override;

  };
  
} /* namespace wisey */

#endif /* LocalSystemReferenceVariable_h */
