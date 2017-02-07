# Data Definitions

## Overview
Here we define data types which we allow to be stored in `O2DataContainers` these include things such as tracks and vertices and all their
included types (such as *p_t* or the fit covariance).  

The gist of it is that we define our types as a tree. The definition of a track might look like this:
```
                      Pt ->float
                    /
        Kinematics---Orientation->struct{float sine, dip;}
       /
Track ---Position---Temporal->float
       \           \
        \           Spatial->{struct float x,y,z;}
          Covariance->float[15]
```
By clever use of templates, what actually gets stored are just the end-points of the tree (e.g. `Pt` and `Spatial` but not `Kinematics`).
The endpoints are each stored in a seperate array to reduce the required bandwidth when an analysis task only
requires a subsection of the tree.
 When we want to perform an analysis which only requires the Position and Pt of a track we would have to define a type, and then request this
 type from the grid. The backed will handle the rest. That is, if we write (psuedo-syntax):
```
  using MyTrackType = Track<PositionAll, Kinematics<Pt>>;
  InputHandler.setDataType<MyTrackType>();
```
Then the tracks we will get in our analysis object corresponds to the tree
```
        Kinematics-Pt ->float
       /
Track ---Position---Temporal->float
                   \
                    Spatial->{struct float x,y,z;}      
```
and only this data will be fetched from disk/memory/network, greatly speeding up our analysis for the case where we are bandwidth-limited (often) and reducing the total memory consumption.

 In the backend, this tree will look like three arrays identified by their full name, that is one: array of floats named `Track/Kinematics/Pt`, one array of floats named `Track/Position/Temporal`, and one array of `struct{float x,y,z;}` named `Track/Position/Spatial`. Because these objects are stored in separate arrays it is also much easier to vectorize algorithms for use with SIMD instructions.

 Due to the use of templates, it is also impossible to now try and use the covariance of your track because this does not exist for your requested type. This means the more errors will be caught at compile time instead of at runtime (or not at all).

## Examles
See the `../Examples` on how to use the defined types in code.

## How to add or extend data types
**NOTE:** this part of the README is still under active development, hence it is quite sparse.

### adding
 In order to add data, simply add the corresponding .h file at the right part of the directory tree. For nodes (IMulti types) one should create a folder and a .h file at the right level. End-points only need their .h file.

 If the added data type is not a top-level type (i.e. it is a child of an already existing type), add an include statemen to its parent. This only has to be done for direct parents.

### Modifying existing data
In order to maintain backwards compatibility, it is for now not possible to change the format of stored data.

**TODO:** this is a feature that should be added in the future.

## Design Rational
The goal of the DataDefinitions design is three-fold:

1. we need a maintainable, extendable, way to define all the data that will be stored and handled by the O2 analysis.
2. This design needs to allow for easy & efficient pruning/skimming/growing in order to facilitate fast analysis.
3. It needs to behave as similarlly as possible to the old analyis facilities as previously used.


Number 1 Is implemented by using a tree to define all possible data types and their relationships. A tree is flexible enough to be able to store all data types needed and by using a codified method for extending and reading the tree we have a clear cut path for future extensions to follow.

Number 2 is then achieved by using an SoA backend where only each end-point of the tree is stored with a full path-like identifier attached. Pruning is implemented by only requesting a subsection of the whole tree which can be efficiently combined with the network code and mmap to only ever transfer as much data as is needed. Growing could be performed by simply defining a new branch on the tree. Skimming can be implemented on a per-endpoint basis. With the endpoints now being arrays of a single type many predesigned algorithms exist for quick skimming. (e.g. mapping the min-max range for every 1000 elements, and first doing an inclusion test over these before loading the data).

Number 3 has two main issues:
1. The old analysis framework assumes it will be fed a tree of events. Because with the new timeframe format we do not store single events nor is there a single fixed definition of what an event entails parts of the backend will have to be rewritten to deal with this new format.
2. The old framework has a concept of an AliVVertex and AliVTrack (among others) which is extensively used. However, supporting the full range of required virtual functions would fly in the face of optimizing via pruning. Fortunately a workaround exists, in the following form:
```
template<typename... T>
class AliO2Track : AliVTrack, Track<T> {
  [...]
  ENABLE_IF(Track::covariance)
  double32_t *getCovarianceMatrix(){return this->Get<Track::covariance>();}
  ENABLE_IF_NOT(Track::covariance)
  double32_t *getCovarianceMatrix(){return nullptr;}
  [...]
}
```
While this is not ideal it should be a relatively painless workaround for issue 2. It remains to be determined wether default behaviour should be to return a default value on non-defined `get`s or to crash. **NOTE:** there are several places in the AliPhysics codebase already where types derive from `AliVTrack` for which several pure virtual functions return values which are not well defined for those types. Given that the new tracks will aready have build in type-safety from their types I would propose to move away from the `AliVTrack` interface and let the compiler infere which functions are available based on the type (and template arguments) of the track instead. This would still involve some ENABLE_IF magic in the Track classes, but the end result should be such that in an analysis task there is no longer a need to cast to and from different `AliVTrack` pointers: The user would specify which type of track (and which parts of it) they would like to get from the file and if a valid pointer is returned all requested data will be well defined, usage of any functions or variables which are not part of this type will automatically trigger a compile time error.
