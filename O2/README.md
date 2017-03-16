# O2

This folder contains the O2 work on the new AOD design. The design is based on an Entity-Component system (note: not an Entity-Component-System system).
The entities and components are defined in the `Entities` and `Components` folder respectively. The system itself is contained in the `ecs` folder. Details for each are contained in a `README.md` in the corresponding folder.

Both entities and components have to implement a static `const char* Id()` function in order to identify them in the datasets. 

This folder contains files relevant to the conversion of ESD files to the "new AOD" format (temporary working version) and subsequent analysis of this format.
Examples of how to use these files are contained in the [sister-repository](https://github.com/RDeckers/AliO2ConversionMacros).
