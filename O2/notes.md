<!--Author: Roel Deckers -->
# General overview of the dataflow ESD->Timeframe->Event

We take as input old ESDs, for each event (which has tracks) we extract global and ITS tracks as well as the primary vertex. We add a simulation time to each object and store it in what we call a timeframe. Which internally has three arrays, one for vertices and two for the different track types. These arrays are sorted according to the simulated timestamps.

These timeframe objects can then recreate events (one for each primary vertex) but some tracks might get associated to more then one vertex, or possibly the wrong vertex all together.

# General overview of the current structure
 A timeframe is the format/structure in which we store all 'occurences' (tracks, vertices) with a timestamp corresponding to
its simulated time-of-measurement (or the simulated-reconstructed-time-of-occurence).

There are various O2* classes in the tree, these are lightweight classes wrapping and defining POD structs (Data_t) which represent the data that gets stored. Because the analysis framework wants objects which derive from various AliV* classes (AliVEvent, AliVTrack) there are also AliO2* classes corresponding to this. These are a glue layer.

 Once the new design is ready, the split between O2* and AliO2* should disappear mostly. Although from a framework design point it is useful to reconsider the role of AliVVertex and co. as they put very strong requirements on data, some of which is already not honored (in spirit) throughout the code. (in my personal opinion...)

 In general, everything which has the Ali* prefix was made to connect to the existing analysis framework. It is mostly PoC.

# ESDs to timeframes
The Timeframe class is responsible for building a new timeframe from old data.
It has a function for doing so called Timeframe::addEvent(ESDEvent*, timestamp_t). It takes two arguments, an ESDEvent and a timestamp
which is the simulated "real" event time. That is, the time at which the event would *physically* have occurred in our fake model.
 The timeframe class reads tracks from the event (splitting them up into Global and ITSTracks) and the *primary* vertex and adds a simulated timestamp.
 This timestamp is sampled around the event time but manipulated to account for reconstruction and measurement errors. The current models are:
  * Boxed by 5000ns for ITSTracks
  * A Guassian of width 100ns for Global tracks
  * A Guassian of width 100ns/sqrt(number of tracks) for the vertex.
 It stores this data in three std::vectors (arrays), one for each type, consisting of O2Track/O2Vertex class types.
 These arrays are kept sorted according to their simulated time.

# Timeframes to single events
Once a timeframe has been built as described above, an event of type O2Event can be created using timeframe::getEvent(index) (AliO2Event is what is used by the framework, it constructs from O2Event). It is assumed that each primary vertex has a one-to-one mapping with an event. So, the framework picks the *index*'d vertex, looks at its timestamp and the resolution and then selects the subslice of Tracks that can be associated with it.

 Association is currently determined by taking the resolution of a track and the vertex, adding it together to form the variable delta. and the seeing if the track falls within [vertex_time - delta, vertex_time + delta]. The resolution of a guassian is currently several times its width.

After it has obtained this slice, it looks at the preceding and the next vertex (if the exist) and if the can be associated with the start or end resp. of the slice. It then marks how many of the tracks were also associated with a neighbour. These are considered *ambigious* tracks. Those not associated with the neighbours are *unambigous*.

## The bad
For one the way association in time is done right now, using very roughly calculated deltas, can be improved dramatically and be made tuneable by using some serious math instead.

Secondly, there is a fundamental issue with slicing in that it only works if at most one of the two types being associated has a constant resolution (which is the case right now, only vertices have a variable resolution).

Third, we only consider temporal ambiguity. This is documented but it would still be better to also use spatial information (DCA) to reject any associations which are not compatible.

Number one and three on this list look to be the first things that will be fundamentally changed once the new dataformat is pushed. This will require rewritting this part of the code in a more generic way as a result anyway which is a good time to make it less hard-coded & copy-pasted.

# running tasks
Analysis tasks can currently be ran using the AliO2Analysis equivalents of the normal framework (Manager, TaskSE, InputHandler). These can also still handle ESD files. Unfortunately it is not possible to run on the grid: some changes were made to the Ali* classes (to allow us to inherit and overwrite methods) and this version is not on the grid.

 My personal recommendation would be to build parallel to or on top of the old AliAnalysis framework instead. The current method is a pretty forcefully inserted shim layer. As maintaining *full* compatibility is going to be hard and/or suboptimal anyway it might not be an efficient use of resources.

# What  will change
So, while this worked as a PoC. It has become clear that this method will not scale nicely to be as generic as it needs to be in order to be extendable (growing) and allow for pruning effectively. The new system being worked on is as follows:

Instead of defining a timeframe with a vector member for each possible type we will make a generic container consisting
of named-types with attached data. A base of how these types will be defined can be seen in the "Data definitions" folder.
The aim is to make a system which is both flexible and typesafe. This will achieved using template programming and type traits
to force a tree like (CS tree) structure to the data.
This tree might look like this:
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
Endpoints of this tree will be stored in arrays, using a named identifier based on their type. e.g. Track/Position/Spatial will hold an array of struct{float x,y,z;}.
Any knot of the tree is a variadic template type, specifying which children it has but contains no data actual.

This change was needed, at least wanted, because the run-time type information (RTTI) exposed by the standard is not safe to use across different machines. So types needed to be named. (Root's RTTI would be robust enough for sharing information but the exposed interface poses restrictions to what types of optimizations are possible and we only need a typename)

The Tree structure then comes in because it allows a directory style naming for nested types and enforces at compile time a standardized structure. It also allows us to implement pruning and growing in a consistent manner which the old system would not allow us to do all while using templates and traits to resolve data dependencies at compile-time.

## The good
We aim to, soon, have a well defined system which allows for a lightweight flexible representation which is identical on disc or in memory (directory/tree based with some trickery for compression) allowing us to use both mmap (possibly saving on disk reads) and shmem as well as efficiently sharing data across any interconnects (host side pruning/possibly skimming).

 Type safe pruning can then be implemented using for example:
 ```
 getTrack<Track<Kinematics<Pt>,Position<Temporal,Spatial>>()
 ```
which will only request and use the transverse momentum and position. Leaving out the covariance

and orientation other momentum components. This example also shows the weakpoint of this method: giant template lists.  But I have faith that this can be made into less of a nuisance to the user than how it looks like here. By using template specialization and typedefs for common types and/or by using this as a 'set once' setting separate from the analysis task itself.

It is already possible to request members of a track in the following fashion

```
  float pt = track.get<IKinematics>().get<Pt>();
```

 Growing is also natural to this structure. For example, say you want to add a flag to each track to signal if it is a muon candidate.
 Simply define a new template type (Inheriting from `ISingle`, and `ITrack` to show that it is an endpoint of the tree and can be stored in a `Track<...>` template)
 as such:
 ```
 struct MuonFlag : ISingle, ITrack{
  static const char* Name(){return MuonFlag;}
  bool flag;
 }
```
 Skimming is a bit less natural, as it involves a completely different kind of operation, but it should be implementable on top of this system. By using a (multi-resolution) tree as metadata. i.e. store data
 which says "the minimum of these N `ISingle`'s is X and the maximum Y" (with for example N = 256, 1024, 4096).

 The type safety afforded by the templates should make it so that you only have to write your task, using a minimally defined Track type for example and then the compiler can do all the work for you of checking that you are requesting sensible information and generating whatever code is needed for obtaining the data you need, and only which you need, without constantly having to check at run-time if you requested for example the covariance matrix or whether you didn't. That is the core strength.

 A template-less implementation would be able to implement skimming too, and wouldn't need the long list of arguments to specify types but it would need to check whether a piece of data is availible on each access or crash.

## The bad
 In this case the bad is the ugly. Using templates to enforce type safety works but the code is non-intuitive to most I would say. Interacting with a deeply nested template type is not very user friendly.
 (example `Track<Position, Covariance> != Track<Covariance, Position>`, although I believe there is a solution for this example).
 However, it is the expectation that changes to this part of the code will be few and far between compared to the usage in writing analysis tasks and good integration
 with the analysis framework and some type_trait magic we should be able to remove most of these pains for the user.
