//
//  BuilderArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef BuilderArgument_h
#define BuilderArgument_h

#include <string>

#include <llvm/IR/Instructions.h>

#include "wisey/IType.hpp"

namespace wisey {

class IExpression;
class IRGenerationContext;
class IConcreteObjectType;
  
/**
 * Represents one argument in the model builder pattern.
 *
 * The model builder is of the form:
 *
 * ModelType modelA = builder(ModelType).withField(1).builder();
 *
 * withField(1) represents one model builder argument
 */
class BuilderArgument {
  
  std::string mFieldSpecifier;
  IExpression* mFieldExpression;
  
public:
  
  BuilderArgument(std::string fieldSpecifier, IExpression* fieldExpression)
    : mFieldSpecifier(fieldSpecifier), mFieldExpression(fieldExpression) { }
  
  ~BuilderArgument();
  
  bool checkArgument(const IConcreteObjectType* object);
  
  /**
   * Derives field name from builder argument by converting 'widthFieldA' to 'fieldA'
   */
  std::string deriveFieldName() const;
  
  /**
   * Computes argument value
   */
  llvm::Value* getValue(IRGenerationContext& context) const;
  
  /**
   * Tells argument type
   */
  const IType* getType(IRGenerationContext& context) const;
  
  /**
   * Releases ownership of objects referred to by argument expression
   */
  void releaseOwnership(IRGenerationContext& context) const;
  
};
  
/**
 * Represents a list of model builder arguments
 */
typedef std::vector<BuilderArgument*> BuilderArgumentList;

} /* namespace wisey */

#endif /* BuilderArgument_h */
