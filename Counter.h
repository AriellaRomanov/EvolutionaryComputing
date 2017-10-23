#pragma once
#include <deque>

template <typename T>
class Counter
{
  std::deque<T> history;
  long size;
public:
  Counter<T>(const long s = 5);
  void AddItem(const T item);
  bool HasChanged();
};

#include "Counter.hpp"