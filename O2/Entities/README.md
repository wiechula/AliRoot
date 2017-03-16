# Entities

There are currently 3 types of entities:
 * Vertices
 * (reconstructed-)Tracks
 * Particles

Vertices and Tracks are present in both reconstructed data and MonteCarlo truths. We currently only store reconstructed vertices.
 Particles are only a part of the MC Truth.

Note how we define optional member functions on entities such as
```
template <typename U = Particle<Components...>,
          typename std::enable_if<U::template Contains<
              particle::Px, particle::Py>()>::type * = nullptr>
float pt() const {
  float px = this->template get<particle::Px>();
  float py = this->template get<particle::Py>();
  return sqrt(px * px + py * py);
}
```
The  `U::Contains` function checks if the entity `U` contains all of the components passed as an argument.
The `enable_if` will make sure the function will only be compiled if the `Contains` function returns true. The end result is that one can call `particle.pt()` iff `particle` was defined as containing both `Px` and `Py`.
