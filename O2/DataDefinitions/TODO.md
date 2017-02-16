1. Force a compile time check that each Single is PoD.
2. Look into improving compile time using extern templates.
3. automate generation using an external build script.
4. Discuss how to store endpoint data (plain old arrays or some more edit-able type)
5. Debug Get and GetRef
7. Finalize the design of generics (e.g. share position between tracks and vertices).
8. Allow an interface for adding functions to multi (e.g. 'length' for position)
9. remove dependence on 'flat_type' in single.
6. Allow the specification of how multi should be stored (SoA, interleaved, packed). This might be useful for e.g.  Position (x*8, y*8, z*8) and other structures which are often accessed together and in parallel. Makes pruning over these substructures more costly though so should be used with good reason.

https://en.wikipedia.org/wiki/Expression_templates
http://en.cppreference.com/w/cpp/numeric/valarray

Q. Do we need a parent for each type?
A. parent type is needed to strictly enforce placement guarentees. That is, Pt is a part of Kinematics, not track.
While not neccesary from a buildability standpoint, it is good to have as it enforces certain patterns which will reduce the overhead in checking code which is associated with having many users.

Q. How to implement both parents and generics? (i.e. reuse position)?
A. Not sure. Not that important r.n.
