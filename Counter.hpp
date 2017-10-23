template <typename T>
Counter<T>::Counter(const long s)
  : size(s)
{}

template <typename T>
void Counter<T>::AddItem(const T item)
{
  history.push_back(item);
  while (static_cast<long>(history.size()) > size)
    history.pop_front();
}

template <typename T>
bool Counter<T>::HasChanged()
{
  if (static_cast<long>(history.size()) == 0
    || static_cast<long>(history.size()) < size)
    return true;

  for (long i = 1; i < size; i++)
  {
    if (i >= static_cast<long>(history.size()))
      return false;

    if (history.at(i) != history.at(i - 1))
      return true;
  }
  return false;
}