//
//  IObjectWithVTable.hpp
//  Wisey
//
//  Created by Vladimir Fridman on 5/30/17.
//  Copyright © 2017 Vladimir Fridman. All rights reserved.
//

#ifndef IObjectWithVTable_h
#define IObjectWithVTable_h

#include <llvm/IR/Instructions.h>

#include "IObjectType.hpp"

namespace wisey {
  
class Interface;
  
/**
 * Interface representing a object that has a vTable: controller or a model
 */
class IObjectWithVTable : public virtual IObjectType {
    
public:
  
  virtual ~IObjectWithVTable() { }

  /**
   * Returns the name of the vTable global varaible
   */
  virtual std::string getVTableName() const = 0;
  
  /**
   * Returns the vTabla size
   */
  virtual unsigned long getVTableSize() const = 0;
 
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
   * Generate vTable global variable for the given object
   */
  static void generateVTable(IRGenerationContext& context,
                             IObjectWithVTable* object,
                             std::map<std::string, llvm::Function*>& methodFunctionMap);
  
private:
  
  static void addTypeListInfo(IRGenerationContext& context,
                              IObjectWithVTable* object,
                              std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void addUnthunkInfo(IRGenerationContext& context,
                             IObjectWithVTable* object,
                             std::vector<std::vector<llvm::Constant*>>& vTables);
  
  static void generateInterfaceMapFunctions(IRGenerationContext& context,
                                            IObjectWithVTable* object,
                                            std::vector<std::vector<llvm::Constant*>>& vTables,
                                            std::map<std::string, llvm::Function*>&
                                            methodFunctionMap);
  
  static void createVTableGlobal(IRGenerationContext& context,
                                 IObjectWithVTable* object,
                                 std::vector<std::vector<llvm::Constant*>> interfaceVTables);
  
  static llvm::GlobalVariable* createTypeListGlobal(IRGenerationContext& context,
                                                    IObjectWithVTable* object);

  
};
  
} /* namespace wisey */

#endif /* IObjectWithVTable_h */
