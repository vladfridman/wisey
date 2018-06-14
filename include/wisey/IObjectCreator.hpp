//
//  IObjectCreator.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 6/14/18.
//  Copyright © 2018 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectCreator_h
#define IObjectCreator_h

#include "wisey/IExpression.hpp"
#include "wisey/IBuildableObjectType.hpp"
#include "wisey/ObjectBuilderArgument.hpp"

namespace wisey {
  
  /**
   * Represents object creator expression such as allocator or builder expressions
   */
  class IObjectCreator : public IExpression {
  
  public:

    /**
     * Checks that all creation arguments are there and that they have correct names
     */
    static void checkArguments(IRGenerationContext& context,
                               const IBuildableObjectType* buildable,
                               const ObjectBuilderArgumentList& objectBuilderArgumentList,
                               int line);


  private:
    
    static void checkArgumentsAreWellFormed(IRGenerationContext& context,
                                            const IBuildableObjectType* buildable,
                                            const ObjectBuilderArgumentList& objectBuilderArguments,
                                            int line);
    
    static void checkAllFieldsAreSet(IRGenerationContext& context,
                                     const IBuildableObjectType* buildable,
                                     const ObjectBuilderArgumentList& objectBuilderArguments,
                                     int line);
    
    static std::vector<std::string> getMissingFields(const IBuildableObjectType* buildable,
                                                     std::set<std::string> givenFields);

  };
  
} /* namespace wisey */


#endif /* IObjectCreator_h */
