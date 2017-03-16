# EC-System

This part of the code handles the definitions of Entities, Components, and the transfer of data.

## Entities

### `Entity<Components...>`
Entities are defined as an index and a set of pointers corresponding to their component arrays. If a component has a N->N mapping, then the entity index is a direct offset into the corresponding array.

If the component has a N->M mapping then the component array consist of two parts. First there is an array of `<index, size>` pairs (one entry for each entity), followed by the array of actual component data to which `<index,size>` reffers. Access to this slice of component data is exposed through the same function as a the N->N data,: `get<ComponentType>()`.

### `EntityCollection<Entity>`
An entity collection is a wrapper that keeps a pointer to the actual data arrays for the given entity. It is what `Entity` objects referrence. It does not own the data.

### `Handler`

This class is what actually owns the data and is in charge of IO as well as keeping track of what `Entity/Component` pairs are available. By having a single handler which hands out read only access pointers to those which need it, it can make sure that data is shared between tasks and possibly coalesce data requests.

Currently it supports reading and writting from uncompressed files. It uses memory-mapped files to implement pruning (only those components that are used will be read from disk).

### `Other`
All other classes are helper classes. 
