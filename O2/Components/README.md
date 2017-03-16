# Components

Components are what is actually stored. They are simple POD-types (not checked yet at compile time yet but will be). They are grouped by their valid entities and namespaced. Those components which are valid for several entities are defined in `Generic` and added to the namespace in their corresponding entities folder.

Virtual components do not define any data, but inherit from the class `Dependency<A, B, C, ...>`. When this component is added to an entity it will automatically pull in `A, B, C,...` instead. Care has to be taken not to duplicate components as they are currently not filtered. Duplicating component specifications introduces a small overhead, but should not be significant however.
