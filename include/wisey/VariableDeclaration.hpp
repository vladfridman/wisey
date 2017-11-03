//
//  VariableDeclaration.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/18/16.
//  Copyright © 2016 Vladimir Fridman. All rights reserved.
//

#ifndef VariableDeclaration_h
#define VariableDeclaration_h

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
  Identifier* mId;
  IExpression* mAssignmentExpression;

public:
  VariableDeclaration(const ITypeSpecifier* typeSpecifier, Identifier* id);

  VariableDeclaration(const ITypeSpecifier* typeSpecifier,
                      Identifier* id,
                      IExpression* assignmentExpression);

  ~VariableDeclaration();
  
  const ITypeSpecifier* getTypeSpecifier() const;
  
  const Identifier* getId() const;

  llvm::Value* generateIR(IRGenerationContext& context) const override;
  
private:
  
  void allocatePrimitive(IRGenerationContext& context,
                         const IPrimitiveType* type) const;
  
  void allocateOwner(IRGenerationContext& context,
                     const IObjectOwnerType* type) const;
  
  void allocateReference(IRGenerationContext& context,
                         const IObjectType* type) const;
};

/**
 * Represents a list of variables
 */
typedef std::vector<VariableDeclaration*> VariableList;

} /* namespace wisey */

#endif /* VariableDeclaration_h */
