//
//  VariableDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef VariableDeclaration_h
#define VariableDeclaration_h

#include "wisey/ArrayType.hpp"
#include "wisey/ArrayOwnerType.hpp"
#include "wisey/IObjectOwnerType.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/IPrimitiveType.hpp"
#include "wisey/IStatement.hpp"
#include "wisey/ITypeSpecifier.hpp"
#include "wisey/Identifier.hpp"

namespace wisey {
  
  /**
   * Represents a variable declaration
   */
  class VariableDeclaration : public IStatement {
    const ITypeSpecifier* mTypeSpecifier;
    Identifier* mIdentifier;
    IExpression* mAssignmentExpression;
    int mLine;
    
  public:
    
    ~VariableDeclaration();
    
    /**
     * Create and instance of VariableDeclaration
     */
    static VariableDeclaration* create(const ITypeSpecifier* typeSpecifier,
                                       Identifier* identifier,
                                       int line);
    
    /**
     * Create and instance of VariableDeclaration with assignment to the given expression
     */
    static VariableDeclaration* createWithAssignment(const ITypeSpecifier* typeSpecifier,
                                                     Identifier* identifier,
                                                     IExpression* assignmentExpression,
                                                     int line);
    
    const ITypeSpecifier* getTypeSpecifier() const;
    
    const Identifier* getIdentifier() const;
    
    llvm::Value* generateIR(IRGenerationContext& context) const override;
    
  private:
    
    VariableDeclaration(const ITypeSpecifier* typeSpecifier,
                        Identifier* identifier,
                        IExpression* assignmentExpression,
                        int line);
    
  };
  
  /**
   * Represents a list of variables
   */
  typedef std::vector<VariableDeclaration*> VariableList;
  
} /* namespace wisey */

#endif /* VariableDeclaration_h */

