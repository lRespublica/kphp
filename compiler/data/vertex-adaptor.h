#pragma once

#include "auto/compiler/vertex/is-base-of.h"

#include "compiler/operation.h"
#include "compiler/threading/format.h"

template<Operation Op>
class vertex_inner;

template<Operation Op>
class VertexAdaptor;

class Location;

using VertexPtr = VertexAdaptor<meta_op_base>;

template<Operation op>
inline void set_location(const Location &location, VertexAdaptor<op> &v);

inline const Location &get_location(const VertexPtr &v);

VertexPtr clone_vertex(VertexPtr);

template<Operation Op>
class VertexAdaptor {
  vertex_inner<Op> *impl;
public:

  VertexAdaptor() :
    impl(nullptr) {
  }

  explicit VertexAdaptor(vertex_inner<Op> *impl) :
    impl(impl) {
  }

  template<Operation FromOp>
  VertexAdaptor(const VertexAdaptor<FromOp> &from) :
    impl(static_cast<vertex_inner<Op> *>(from.impl)) {
    static_assert(op_type_is_base_of(Op, FromOp), "Strange cast to not base vertex");
  }

  template<Operation FromOp>
  VertexAdaptor &operator=(const VertexAdaptor<FromOp> &from) {
    static_assert(op_type_is_base_of(Op, FromOp), "Strange assignment to not base vertex");
    impl = static_cast<vertex_inner<Op> *> (from.impl);
    return *this;
  }

  explicit operator bool() const {
    return impl != nullptr;
  }

  vertex_inner<Op> *operator->() {
    assert(impl != nullptr);
    return impl;
  }

  const vertex_inner<Op> *operator->() const {
    assert(impl != nullptr);
    return impl;
  }

  vertex_inner<Op> &operator*() {
    assert(impl != nullptr);
    return *impl;
  }

  const vertex_inner<Op> &operator*() const {
    assert(impl != nullptr);
    return *impl;
  }

  template<Operation to>
  VertexAdaptor<to> as() const {
    if (!impl) {
      return {};
    };
    auto res = try_as<to>();
    dl_assert(res, format("Can't cast VertexAdaptor<%d>(real type %d) to VertexAdaptor<%d>", Op, impl ? impl->type() : -1, to));
    return res;
  }

  template<Operation to>
  VertexAdaptor<to> try_as() const {
    if (!impl) {
      return {};
    };
    static_assert(op_type_is_base_of(Op, to), "Strange downcast to not derived vertex");
    if (op_type_is_base_of(to, impl->type())) {
      return VertexAdaptor<to>{static_cast<vertex_inner<to> *>(impl)};
    }

    return {};
  }

  static void init_properties(OpProperties *x) {
    vertex_inner<Op>::init_properties(x);
  }

  template<typename... Args>
  static VertexAdaptor<Op> create(Args &&... args) {
    return VertexAdaptor<Op>(vertex_inner<Op>::create(std::forward<Args>(args)...));
  }

  VertexAdaptor<Op> clone() const {
    return VertexAdaptor<Op>(clone_vertex(*this).template as<Op>());
  }

  template<Operation Op2>
  friend class VertexAdaptor;

  bool operator==(VertexPtr other) {
    return static_cast<vertex_inner<meta_op_base> *>(impl) == other.impl;
  }

  bool operator!=(VertexPtr other) {
    return !(*this == other);
  }

  VertexAdaptor &set_location(const Location &location) {
    ::set_location(location, *this);
    return *this;
  }

  VertexAdaptor &set_location(VertexPtr v) {
    ::set_location(get_location(v), *this);
    return *this;
  }
};
