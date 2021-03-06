//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
// Copyright (c) 2016-19, Lawrence Livermore National Security, LLC.
//
// Produced at the Lawrence Livermore National Laboratory
//
// LLNL-CODE-689114
//
// All rights reserved.
//
// This file is part of RAJA.
//
// For details about use and distribution, please read RAJA/LICENSE.
//
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

///
/// Source file containing tests for atomic operations
///

#include <RAJA/RAJA.hpp>
#include "RAJA_gtest.hpp"
#include <type_traits>

template < typename T >
RAJA_INLINE
RAJA_HOST_DEVICE
typename std::enable_if<sizeof(T) == 1, T>::type np2m1(T val)
{
  val |= val >> 1  ;
  val |= val >> 2  ;
  val |= val >> 4  ;
  return val;
}

template < typename T >
RAJA_INLINE
RAJA_HOST_DEVICE
typename std::enable_if<sizeof(T) == 2, T>::type np2m1(T val)
{
  val |= val >> 1  ;
  val |= val >> 2  ;
  val |= val >> 4  ;
  val |= val >> 8  ;
  return val;
}

template < typename T >
RAJA_INLINE
RAJA_HOST_DEVICE
typename std::enable_if<sizeof(T) == 4, T>::type np2m1(T val)
{
  val |= val >> 1  ;
  val |= val >> 2  ;
  val |= val >> 4  ;
  val |= val >> 8  ;
  val |= val >> 16 ;
  return val;
}

template < typename T >
RAJA_INLINE
RAJA_HOST_DEVICE
typename std::enable_if<sizeof(T) == 8, T>::type np2m1(T val)
{
  val |= val >> 1  ;
  val |= val >> 2  ;
  val |= val >> 4  ;
  val |= val >> 8  ;
  val |= val >> 16 ;
  val |= val >> 32 ;
  return val;
}

template < typename T >
RAJA_INLINE
RAJA_HOST_DEVICE
typename std::enable_if<sizeof(T) == 16, T>::type np2m1(T val)
{
  val |= val >> 1  ;
  val |= val >> 2  ;
  val |= val >> 4  ;
  val |= val >> 8  ;
  val |= val >> 16 ;
  val |= val >> 32 ;
  val |= val >> 64 ;
  return val;
}

template < typename T, typename AtomicPolicy >
struct AndEqOtherOp {
  AndEqOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size()),
    final_min(min), final_max(min)
  { count[0] = np2m1((T)seg.size()); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other &= (T)i; }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct FetchAndOtherOp {
  FetchAndOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max(np2m1((T)seg.size())),
    final_min(min), final_max(min)
  { count[0] = max; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.fetch_and((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct OrEqOtherOp {
  OrEqOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max(np2m1((T)seg.size())),
    final_min(max), final_max(max)
  { count[0] = T(0); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other |= (T)i; }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct FetchOrOtherOp {
  FetchOrOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max(np2m1((T)seg.size())),
    final_min(max), final_max(max)
  { count[0] = T(0); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.fetch_or((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct XorEqOtherOp {
  XorEqOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max(np2m1((T)seg.size())),
    final_min(min), final_max(min)
  { count[0] = T(0);
    for (RAJA::Index_type i = 0; i < seg.size(); ++i) {
      final_min ^= (T)i; final_max ^= (T)i;
    } }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other ^= (T)i; }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct FetchXorOtherOp {
  FetchXorOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max(np2m1((T)seg.size())),
    final_min(min), final_max(min)
  { count[0] = T(0);
    for (RAJA::Index_type i = 0; i < seg.size(); ++i) {
      final_min ^= (T)i; final_max ^= (T)i;
    } }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.fetch_xor((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct LoadOtherOp {
  LoadOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min((T)seg.size()), max(min),
    final_min(min), final_max(min)
  { count[0] = min; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const
    { return other.load(); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct OperatorTOtherOp {
  OperatorTOtherOp(T* count, RAJA::RangeSegment RAJA_UNUSED_ARG(seg))
    : other(count), min(T(0)), max(min),
    final_min(min), final_max(min)
  { count[0] = min; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const
    { return other; }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct StoreOtherOp {
  StoreOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min((T)0), max((T)seg.size() - (T)1),
    final_min(min), final_max(max)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { other.store((T)i); return (T)i; }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct AssignOtherOp {
  AssignOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size() - (T)1),
    final_min(min), final_max(max)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return (other = (T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct CASOtherOp {
  CASOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min((T)0), max((T)seg.size() - (T)1),
    final_min(min), final_max(max)
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    {
      T received, expect = (T)0;
      while ((received = other.CAS(expect, (T)i)) != expect) {
        expect = received;
      }
      return received;
    }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct CompareExchangeWeakOtherOp {
  CompareExchangeWeakOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min((T)0), max((T)seg.size() - (T)1),
    final_min(min), final_max(max)
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    {
      T expect = (T)0;
      while (!other.compare_exchange_weak(expect, (T)i)) {}
      return expect;
    }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct CompareExchangeStrongOtherOp {
  CompareExchangeStrongOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min((T)0), max((T)seg.size() - (T)1),
    final_min(min), final_max(max)
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    {
      T expect = (T)0;
      while (!other.compare_exchange_strong(expect, (T)i)) {}
      return expect;
    }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct PreIncCountOp {
  PreIncCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)seg.size())
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (++counter) - (T)1;
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct PostIncCountOp {
  PostIncCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)seg.size())
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (counter++);
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct AddEqCountOp {
  AddEqCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)seg.size())
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (counter += (T)1) - (T)1;
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct FetchAddCountOp {
  FetchAddCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)seg.size())
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return counter.fetch_add((T)1);
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct PreDecCountOp {
  PreDecCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)0)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (--counter);
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct PostDecCountOp {
  PostDecCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)0)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (counter--) - (T)1;
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct SubEqCountOp {
  SubEqCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)0)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return (counter -= (T)1);
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct FetchSubCountOp {
  FetchSubCountOp(T* count, RAJA::RangeSegment seg)
    : counter(count), min((T)0), max((T)seg.size()-(T)1), final((T)0)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type RAJA_UNUSED_ARG(i)) const {
      return counter.fetch_sub((T)1) - (T)1;
    }
  RAJA::AtomicRef<T, AtomicPolicy> counter;
  T min, max, final;
};

template < typename T, typename AtomicPolicy >
struct MaxEqOtherOp {
  MaxEqOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size() - (T)1),
    final_min(max), final_max(max)
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.max((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct FetchMaxOtherOp {
  FetchMaxOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size() - (T)1),
    final_min(max), final_max(max)
  { count[0] = (T)0; }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.fetch_max((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct MinEqOtherOp {
  MinEqOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size() - (T)1),
    final_min(min), final_max(min)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.min((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template < typename T, typename AtomicPolicy >
struct FetchMinOtherOp {
  FetchMinOtherOp(T* count, RAJA::RangeSegment seg)
    : other(count), min(T(0)), max((T)seg.size()),
    final_min(min), final_max(min)
  { count[0] = (T)seg.size(); }
  RAJA_HOST_DEVICE
    T operator()(RAJA::Index_type i) const
    { return other.fetch_min((T)i); }
  RAJA::AtomicRef<T, AtomicPolicy> other;
  T min, max, final_min, final_max;
};

template <typename ExecPolicy,
         typename AtomicPolicy,
         typename T,
  template <typename, typename> class CountOp>
void testAtomicRefCount(RAJA::RangeSegment seg,
    T* count, T* list, bool* hit)
{
  CountOp<T, AtomicPolicy> countop(count, seg);
  RAJA::forall<ExecPolicy>(seg, [=] RAJA_HOST_DEVICE(RAJA::Index_type i) {
      list[i] = countop.max + (T)1;
      hit[i] = false;
      });
  RAJA::forall<ExecPolicy>(seg, [=] RAJA_HOST_DEVICE(RAJA::Index_type i) {
      T val = countop(i);
      list[i] = val;
      hit[(RAJA::Index_type)val] = true;
      });
#if defined(RAJA_ENABLE_CUDA)
  cudaDeviceSynchronize();
#endif
  EXPECT_EQ(countop.final, count[0]);
  for (RAJA::Index_type i = 0; i < seg.size(); i++) {
    EXPECT_LE(countop.min, list[i]);
    EXPECT_GE(countop.max, list[i]);
    EXPECT_TRUE(hit[i]);
  }
}

template <typename ExecPolicy,
         typename AtomicPolicy,
         typename T,
  template <typename, typename> class OtherOp>
void testAtomicRefOther(RAJA::RangeSegment seg, T* count, T* list)
{
  OtherOp<T, AtomicPolicy> otherop(count, seg);
  RAJA::forall<ExecPolicy>(seg, [=] RAJA_HOST_DEVICE(RAJA::Index_type i) {
      list[i] = otherop.max + (T)1;
      });
  RAJA::forall<ExecPolicy>(seg, [=] RAJA_HOST_DEVICE(RAJA::Index_type i) {
      T val = otherop(i);
      list[i] = val;
      });
#if defined(RAJA_ENABLE_CUDA)
  cudaDeviceSynchronize();
#endif
  EXPECT_LE(otherop.final_min, count[0]);
  EXPECT_GE(otherop.final_max, count[0]);
  for (RAJA::Index_type i = 0; i < seg.size(); i++) {
    EXPECT_LE(otherop.min, list[i]);
    EXPECT_GE(otherop.max, list[i]);
  }
}


template <typename ExecPolicy,
         typename AtomicPolicy,
         typename T,
  RAJA::Index_type N>
void testAtomicRefIntegral()
{
  RAJA::RangeSegment seg(0, N);

  // initialize an array
#if defined(RAJA_ENABLE_CUDA)
  T *count = nullptr;
  cudaMallocManaged((void **)&count, sizeof(T) * 1);
  T *list;
  cudaMallocManaged((void **)&list, sizeof(T) * N);
  bool *hit;
  cudaMallocManaged((void **)&hit, sizeof(bool) * N);
  cudaDeviceSynchronize();
#else
  T *count  = new T[1];
  T *list   = new T[N];
  bool *hit = new bool[N];
#endif

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, LoadOtherOp     >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, OperatorTOtherOp>(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, StoreOtherOp    >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, AssignOtherOp   >(seg, count, list);

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CASOtherOp                  >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CompareExchangeWeakOtherOp  >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CompareExchangeStrongOtherOp>(seg, count, list);

  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PreIncCountOp  >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PostIncCountOp >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, AddEqCountOp   >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, FetchAddCountOp>(seg, count, list, hit);

  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PreDecCountOp  >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PostDecCountOp >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, SubEqCountOp   >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, FetchSubCountOp>(seg, count, list, hit);

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, MaxEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchMaxOtherOp>(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, MinEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchMinOtherOp>(seg, count, list);

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, AndEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchAndOtherOp>(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, OrEqOtherOp    >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchOrOtherOp >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, XorEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchXorOtherOp>(seg, count, list);

#if defined(RAJA_ENABLE_CUDA)
  cudaFree(hit);
  cudaFree(list);
  cudaFree(count);
#else
  delete[] hit;
  delete[] list;
  delete[] count;
#endif
}



template <typename ExecPolicy,
         typename AtomicPolicy,
         typename T,
  RAJA::Index_type N>
void testAtomicRefFloating()
{
  RAJA::RangeSegment seg(0, N);

  // initialize an array
#if defined(RAJA_ENABLE_CUDA)
  T *count = nullptr;
  cudaMallocManaged((void **)&count, sizeof(T) * 1);
  T *list;
  cudaMallocManaged((void **)&list, sizeof(T) * N);
  bool *hit;
  cudaMallocManaged((void **)&hit, sizeof(bool) * N);
  cudaDeviceSynchronize();
#else
  T *count  = new T[1];
  T *list   = new T[N];
  bool *hit = new bool[N];
#endif

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, LoadOtherOp     >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, OperatorTOtherOp>(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, StoreOtherOp    >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, AssignOtherOp   >(seg, count, list);

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CASOtherOp                  >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CompareExchangeWeakOtherOp  >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, CompareExchangeStrongOtherOp>(seg, count, list);

  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PreIncCountOp  >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PostIncCountOp >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, AddEqCountOp   >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, FetchAddCountOp>(seg, count, list, hit);

  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PreDecCountOp  >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, PostDecCountOp >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, SubEqCountOp   >(seg, count, list, hit);
  testAtomicRefCount<ExecPolicy, AtomicPolicy, T, FetchSubCountOp>(seg, count, list, hit);

  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, MaxEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchMaxOtherOp>(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, MinEqOtherOp   >(seg, count, list);
  testAtomicRefOther<ExecPolicy, AtomicPolicy, T, FetchMinOtherOp>(seg, count, list);

#if defined(RAJA_ENABLE_CUDA)
  cudaFree(hit);
  cudaFree(list);
  cudaFree(count);
#else
  delete[] hit;
  delete[] list;
  delete[] count;
#endif
}


  template <typename ExecPolicy, typename AtomicPolicy>
void testAtomicRefPol()
{
  testAtomicRefIntegral<ExecPolicy, AtomicPolicy, int, 10000>();
  #if defined(TEST_EXHAUSTIVE)
  testAtomicRefIntegral<ExecPolicy, AtomicPolicy, unsigned, 10000>();
  testAtomicRefIntegral<ExecPolicy, AtomicPolicy, long long, 10000>();
  testAtomicRefIntegral<ExecPolicy, AtomicPolicy, unsigned long long, 10000>();

  testAtomicRefFloating<ExecPolicy, AtomicPolicy, float, 10000>();
  #endif
  testAtomicRefFloating<ExecPolicy, AtomicPolicy, double, 10000>();
}

