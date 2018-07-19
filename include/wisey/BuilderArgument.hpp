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
#include "wisey/IPrintable.hpp"

namespace wisey {
  
  class IExpression;
  class IRGenerationContext;
  class IConcreteObjectType;
  
  /**
   * Represents one argument in an object builder pattern.
   *
   * The object builder is of the form:
   *
   * ModelType* modelA = build(ModelType).withField(1).onHeap();
   * or
   * NodeType* nodeB = build(NodyType).withField(1).onPool(pool);
   *
   * withField(1) represents one object builder argument
   */
  class BuilderArgument : public IPrintable {
    
    std::string mFieldSpecifier;
    IExpression* mFieldExpression;
    
  public:
    
    BuilderArgument(std::string fieldSpecifier, IExpression* fieldExpression);
    
    ~BuilderArgument();
    
    /**
     * Checks the legality for the builder argument and prints an error if necessary
     */
    bool checkArgument(IRGenerationContext& context,
                       const IConcreteObjectType* object,
                       int line);
    
    /**
     * Derives field name from builder argument by converting 'widthFieldA' to 'fieldA'
     */
    std::string deriveFieldName() const;
    
    /**
     * Computes argument value
     */
    llvm::Value* getValue(IRGenerationContext& context, const IType* assignToType) const;
    
    /**
     * Tells argument type
     */
    const IType* getType(IRGenerationContext& context) const;
    
    void printToStream(IRGenerationContext& context, std::iostream& stream) const override;
    
  };
  
  /**
   * Represents a list of model builder arguments
   */
  typedef std::vector<BuilderArgument*> BuilderArgumentList;
  
} /* namespace wisey */

#endif /* BuilderArgument_h */

