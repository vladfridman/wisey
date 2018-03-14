//
//  ObjectBuilderArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 1/22/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef ObjectBuilderArgument_h
#define ObjectBuilderArgument_h

#include <string>

#include <llvm/IR/Instructions.h>

#include "wisey/IType.hpp"
#include "wisey/IPrintable.hpp"

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
  class ObjectBuilderArgument : public IPrintable {
    
    std::string mFieldSpecifier;
    IExpression* mFieldExpression;
    
  public:
    
    ObjectBuilderArgument(std::string fieldSpecifier, IExpression* fieldExpression);
    
    ~ObjectBuilderArgument();
    
    /**
     * Checks the legality for the builder argument and prints an error if necessary
     */
    bool checkArgument(const IConcreteObjectType* object);
    
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
  typedef std::vector<ObjectBuilderArgument*> ObjectBuilderArgumentList;
  
} /* namespace wisey */

#endif /* ObjectBuilderArgument_h */

