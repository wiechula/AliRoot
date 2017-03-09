#ifndef ECS_SLICE_H
#define ECS_SLICE_H
#include <sstream>
#include <stddef.h>
namespace ecs {
/// A helper class that wraps a sized array. Does not own the data it points
/// too. NOTE: I believe a better implementation already exists in the O2
/// repository and that should replace this one.
template <typename T> class Slice {
  size_t mSize;
  T *mData;

public:
  Slice<T>(T *data = nullptr, size_t size = 0) : mSize(size), mData(data) {}
  // Slice<T>(Slice<T> &other) : mData(other.mData), mSize(other.mSize){};
  T *data() { return mData; }
  const T *data() const { return mData; }
  size_t size() const { return mSize; }
  T &operator[](size_t index) { return mData[index]; }
  T &at(size_t index) {
    if (index > size()) {
      std::stringstream msg;
      msg << "requested index " << index << " out of range of size " << size();
      throw std::out_of_range(msg.str());
    }
    this[index];
  }
  T *begin() { return data(); }
  T *end() { return data() + mSize; }
};
}
#endif
