#ifndef ECS_HANDLER_H
#define ECS_HANDLER_H
#include "Slice.h"
#include "VariableComponent.h"
#include <boost/iostreams/device/mapped_file.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <map>
#include <unistd.h>

namespace ecs {
/// Handles the fetching and sharing of data. This is the class which actually
/// owns all the allocated data and open files.
/// Currently only supports reeading data from a file or direct insertion of
/// arrays. The latter should only be used for converting old ESDs. It should be
/// rewritten before going into production
class Handler {
  // The identifiers used for Entities and Components
  using EntityKey = uint16_t;
  using ComponentKey = uint16_t;
  // What we store for each component, a Slice char is just a size+ptr pair.
  using ComponentData = Slice<char>;
  // what we store for each entity
  struct EntityData {
    EntityData(uint64_t count = 0) : mCount(count) {}
    // How many of this entity are there?
    uint64_t mCount;
    // A mapping for the associated components to their data.
    std::map<ComponentKey, ComponentData> mMapping;
  };
  // Map entities to data.
  std::map<EntityKey, EntityData> mMapping;
  // We can efficiently add files using mmap, this means that we have to keep
  // files open until we terminate however, so we keep track of them here. Close
  // *should* be called automatically as the file handles get their dtor called.
  boost::iostreams::mapped_file_source mMappedFile;

public:
  Handler() {}
  Handler(const std::string &filename) { readFile(filename); }
  // serialize to file.
  void toFile(const std::string &filename) {
    std::ofstream file;
    file.open(filename.c_str(), std::ios::binary | std::ios::out);
    uint32_t nEntities = mMapping.size();
    std::cout << "Writting " << nEntities << " entities\n";
    file.write(reinterpret_cast<const char *>(&nEntities), sizeof(nEntities));
    for (auto entityIt = mMapping.begin(); entityIt != mMapping.end();
         ++entityIt) {
      uint64_t count = entityIt->second.mCount;
      EntityKey eKey = entityIt->first;
      uint32_t nComponents = entityIt->second.mMapping.size();
      file.write(reinterpret_cast<const char *>(&eKey), sizeof(eKey));
      file.write(reinterpret_cast<const char *>(&count), sizeof(count));
      file.write(reinterpret_cast<const char *>(&nComponents),
                 sizeof(nComponents));
      std::cout << "Wrote entity : " << eKey << ", " << count << ", "
                << nComponents << std::endl;
      for (auto componentIt = entityIt->second.mMapping.begin();
           componentIt != entityIt->second.mMapping.end(); ++componentIt) {
        ComponentKey cKey = componentIt->first;
        uint64_t size = componentIt->second.size();
        file.write(reinterpret_cast<const char *>(&cKey), sizeof(cKey));
        file.write(reinterpret_cast<const char *>(&size), sizeof(size));
        std::cout << "wrote " << cKey << ", " << size << "\n";
      }
    }
    for (auto entityIt = mMapping.begin(); entityIt != mMapping.end();
         ++entityIt) {
      // size_t count = entityIt->second.mCount;
      for (auto componentIt = entityIt->second.mMapping.begin();
           componentIt != entityIt->second.mMapping.end(); ++componentIt) {
        uint64_t size = componentIt->second.size();
        file.write(reinterpret_cast<const char *>(componentIt->second.data()),
                   size);
      }
    }
    file.close();
  }

  /// readback data from file. Use Boost's memory mapped files so that data
  /// isn't actually read from disk unless it is accessed.
  void readFile(const std::string &filename) {
    boost::iostreams::mapped_file_source file;
    file.open(filename.c_str());
    if (file.is_open()) {
      mMappedFile = file;
      // Get pointer to the data
      char *bytestream = (char *)file.data();
      uint32_t nEntities = *((uint32_t *)bytestream);
      std::cout << "reading " << nEntities << " entities" << std::endl;
      bytestream += 4;
      size_t componentBlockSize = sizeof(ComponentKey) + sizeof(uint64_t);
      for (unsigned u = 0; u < nEntities; u++) {
        bytestream += sizeof(EntityKey) + sizeof(uint64_t);
        uint32_t nComponents = *((uint32_t *)bytestream);
        bytestream += sizeof(uint32_t) + nComponents * componentBlockSize;
      }
      char *datastart = bytestream;
      bytestream = (char *)file.data() + 4;
      for (unsigned u = 0; u < nEntities; u++) {
        EntityKey eKey = *((EntityKey *)bytestream);
        bytestream += sizeof(EntityKey);
        uint64_t count = *((uint64_t *)bytestream);
        bytestream += sizeof(count);
        uint32_t nComponents = *((uint32_t *)bytestream);
        bytestream += sizeof(uint32_t);
        std::cout << "Read entity : " << eKey << ", " << count << ", "
                  << nComponents << std::endl;
        EntityData &eData = mMapping[eKey];
        eData.mCount = count;
        for (uint32_t c = 0; c < nComponents; c++) {
          ComponentKey cKey = *((ComponentKey *)bytestream);
          bytestream += sizeof(cKey);
          uint64_t cSize = *((uint64_t *)bytestream);
          bytestream += 8;
          std::cout << "read " << cKey << ", " << cSize << "\n";
          ComponentData cData(datastart, cSize);
          eData.mMapping[cKey] = cData;
          datastart += cSize;
        }
      }
    } else {
      std::cerr << "could not map the file " << filename << std::endl;
    }
  }

  // Returns ComponentData for a given entity and component. Throws if those
  // don't exist.
  template <typename Entity, typename Component>
  const ComponentData getUntypedComponentData() const {
    // std::cout << "someone requested " << typeid(Entity).name() << "/"
    //           << typeid(Component).name() << std::endl;
    return mMapping.at(Entity::Id()).mMapping.at(Component::Id());
  }

  template <typename Entity, typename Component,
            typename std::enable_if<!std::is_base_of<
                IVariableComponent, Component>::value>::type * = nullptr>
  const Slice<Component> getTypedComponentData() const {
    // std::cout << "someone requested " << typeid(Entity).name() << "/"
    //           << typeid(Component).name() << std::endl;
    auto untyped = getUntypedComponentData<Entity, Component>();
    return Slice<Component>((Component *)untyped.data(),
                            untyped.size() / sizeof(Component));
  }

  /// sets the Entity count for the given entity to count.
  template <typename Entity> void forceInsertEntity(uint64_t count) {
    mMapping[Entity::Id()].mCount = count;
  }
  /// adds the component data to the given entity. throws if the entity is not
  /// present
  template <typename Entity, typename Component,
            typename std::enable_if<!std::is_base_of<
                IVariableComponent, Component>::value>::type * = nullptr>
  void forceInsertComponentData(Component *data) {
    EntityData &entityData = mMapping.at(Entity::Id());
    entityData.mMapping[Component::Id()] =
        ComponentData((char *)data, sizeof(Component) * entityData.mCount);
  }

  /// Inserts N->M component data. Takes a map consiting of entityCount
  /// index,size pairs and the size of the data in addition to a pointer to the
  /// data. TODO: THIS IS A TERRIBLE METHOD AND SHOULD BE REDESIGNED
  template <typename Entity, typename Component,
            typename std::enable_if<std::is_base_of<
                IVariableComponent, Component>::value>::type * = nullptr>
  void forceInsertComponentData(uint64_t *map, size_t dataCount,
                                Component *data) {
    auto &entityData = mMapping.at(Entity::Id());
    size_t allocationSize = sizeof(uint64_t) * 2 * entityData.mCount +
                            dataCount * sizeof(Component);
    // BUG: leaks memory.
    ComponentData cData((char *)malloc(allocationSize), allocationSize);
    memcpy(cData.data(), map, sizeof(uint64_t) * 2 * entityData.mCount);
    Component *dataDest =
        (Component *)(((uint64_t *)cData.data()) + 2 * entityData.mCount);
    memcpy(dataDest, data, dataCount * sizeof(Component));
    entityData.mMapping[Component::Id()] = cData;
  }

  template <typename Entity> uint64_t getCount() const {
    try {
      return mMapping.at(Entity::Id()).mCount;
    } catch (...) {
      return 0;
    }
  }
};
}
#endif
