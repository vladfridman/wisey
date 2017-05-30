//
//  IConcreteObjectType.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IConcreteObjectType_h
#define IConcreteObjectType_h

#include <llvm/IR/Instructions.h>

#include "wisey/Field.hpp"
#include "wisey/IObjectType.hpp"
#include "wisey/Method.hpp"

namespace wisey {
  
class Interface;
  
/**
 * Interface representing a object that has a vTable: controller or a model
 */
class IConcreteObjectType : public IObjectType {
    
public:
  
  virtual ~IConcreteObjectType() { }

  /**
   * Looks for a field with a given name in the object
   */
  virtual Field* findField(std::string fieldName) const = 0;

  /**
   * Returns the name of the vTable global varaible
   */
  virtual std::string getVTableName() const = 0;
  
  /**
   * Returns interfaces that this object implements
   */
  virtual std::vector<Interface*> getInterfaces() const = 0;

  /**
   * Returns a list of all interfaces this model implements including inherited interfaces
   */
  virtual std::vector<Interface*> getFlattenedInterfaceHierarchy() const = 0;

  /**
   * Returns the name of the global varaible containing types that this model implements
   */
  virtual std::string getTypeTableName() const = 0;
  
  /**
   * Finds a method with a given name.
   */
  virtual Method* findMethod(std::string methodName) const = 0;
  
  /**
   * Get list of all methods
   */
  virtual std::vector<Method*> getMethods() const = 0;

  /**
   * Generate vTable global variable for the given object
   */
  static void generateVTable(IRGenerationContext& context, IConcreteObjectType* object);
  
private:
  
  static std::map<std::string, llvm::Function*> generateMethodsIR(IRGenerationContext& context,
                                                                  IConcreteObjectType* object);
  

  static void addTypeListInfo(IRGenerationContext& context,
                              IConcreteObjectType* object,
                              std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void addUnthunkInfo(IRGenerationContext& context,
                             IConcreteObjectType* object,
                             std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void generateInterfaceMapFunctions(IRGenerationContext& context,
                                            IConcreteObjectType* object,
                                            std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void createVTableGlobal(IRGenerationContext& context,
                                 IConcreteObjectType* object,
                                 std::vector<std::vector<llvm::Constant*>> interfaceVTables);
  
  static llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context,
                                                    IConcreteObjectType* object);

  
};
  
} /* namespace wisey */

#endif /* IConcreteObjectType_h */
