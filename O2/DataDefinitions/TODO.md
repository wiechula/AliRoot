1. Force a compile time check that each Single is PoD.
2. Look into improving compile time using extern templates.
3. automate generation using an external build script.
4. Discuss how to store endpoint data (plain old arrays or some more edit-able type)
5. Debug Get and GetRef
7. Finalize the design of generics (e.g. share position between tracks and vertices).
8. Allow an interface for adding functions to multi (e.g. 'length' for position)
9. remove dependence on 'flat_type' in single.
6. Allow the specification of how multi should be stored (SoA, interleaved, packed). This might be useful for e.g.  Position (x*8, y*8, z*8) and other structures which are often accessed together and in parallel. Makes pruning over these substructures more costly though so should be used with good reason.
