//
//  LLVMVariableDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 3/20/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef LLVMVariableDeclaration_h
#define LLVMVariableDeclaration_h

#include "ITypeSpecifier.hpp"
#include "IStatement.hpp"
#include "Identifier.hpp"

namespace wisey {
  
  /**
   * Represents a variable declaration
   */
  class LLVMVariableDeclaration : public IStatement {
    const ITypeSpecifier* mTypeSpecifier;
    Identifier* mIdentifier;
    IExpression* mAssignmentExpression;
    int mLine;
    
  public:
    
    ~LLVMVariableDeclaration();
    
    /**
     * Create and instance of LLVMVariableDeclaration
     */
    static LLVMVariableDeclaration* create(const ITypeSpecifier* typeSpecifier,
                                           Identifier* identifier,
                                           int line);

    /**
     * Create and instance of LLVMVariableDeclaration with assignment to the given expression
     */
    static LLVMVariableDeclaration* createWithAssignment(const ITypeSpecifier* typeSpecifier,
                                                         Identifier* identifier,
                                                         IExpression* assignmentExpression,
                                                         int line);

    const ITypeSpecifier* getTypeSpecifier() const;
    
    const Identifier* getIdentifier() const;
    
    void generateIR(IRGenerationContext& context) const override;
    
  private:
    
    LLVMVariableDeclaration(const ITypeSpecifier* typeSpecifier,
                            Identifier* identifier,
                            IExpression* assignmentExpression,
                            int line);

  };
  
  /**
   * Represents a list of variables
   */
  typedef std::vector<LLVMVariableDeclaration*> LLVMVariableList;
  
} /* namespace wisey */

#endif /* LLVMVariableDeclaration_h */
