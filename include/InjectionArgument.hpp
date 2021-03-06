//
//  InjectionArgumentArgument.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 12/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef InjectionArgumentArgument_h
#define InjectionArgumentArgument_h

#include <string>

#include <llvm/IR/Instructions.h>

#include "IPrintable.hpp"

namespace wisey {
  
  class IRGenerationContext;
  class IConcreteObjectType;
  class IExpression;
  class IType;
  
  /**
   * Represents one argument for controller injection.
   *
   * The controller injection is of the form:
   *
   * MController* myController = injector(MController).withField(1).inject();
   *
   * withField(1) represents initialization for controller's field of type 'receive'
   */
  class InjectionArgument : public IPrintable {
    
    std::string mFieldSpecifier;
    IExpression* mFieldExpression;
    
  public:
    
    InjectionArgument(std::string fieldSpecifier, IExpression* fieldExpression);
    
    ~InjectionArgument();
    
    /**
     * Checks the legality of this argument and prints an error if argument is not well formed
     */
    bool checkArgument(IRGenerationContext& context,
                       const IConcreteObjectType* object,
                       int line);
    
    /**
     * Derives field name from injection argument by converting 'widthFieldA' to 'fieldA'
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
   * Represents a list of injection arguments
   */
  typedef std::vector<InjectionArgument*> InjectionArgumentList;
  
} /* namespace wisey */

#endif /* InjectionArgumentArgument_h */

