//
//  IFieldVariable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 8/6/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IFieldVariable_h
#define IFieldVariable_h

#include "IConcreteObjectType.hpp"
#include "IField.hpp"

namespace wisey {
  
  /**
   * Contains helper methods for dealing with field variables
   */
  class IFieldVariable {
    
  public:
    
    /**
     * Returns an object's field given an object and field name
     *
     * Logs an error if the field is not assinable or does not exist
     */
    static IField* checkAndFindFieldForAssignment(IRGenerationContext& context,
                                                  const IConcreteObjectType* object,
                                                  std::string fieldName,
                                                  int line);
    
    /**
     * Returns an llvm pointer to the object's field given an object and field name
     */
    static llvm::GetElementPtrInst* getFieldPointer(IRGenerationContext& context,
                                                    const IConcreteObjectType* object,
                                                    std::string fieldName,
                                                    int line);
    
    /**
     * Returns an object's field given an object and field name
     *
     * Logs an error if the field is not found and exits
     */
    static IField* checkAndFindField(IRGenerationContext& context,
                                     const IConcreteObjectType* object,
                                     std::string fieldName,
                                     int line);
    
  };
  
} /* namepsace wisey */

#endif /* IFieldVariable_h */

