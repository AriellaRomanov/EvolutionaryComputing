#include "coordinate.h"

Coord GetX(const Coordinate c)
{
  return std::get<0>(c);
}
Coord GetY(const Coordinate c)
{
  return std::get<1>(c);
}
void SetX(Coordinate& c, const Coord x)
{
  std::get<0>(c) = x;
}

void SetY(Coordinate& c, const Coord y)
{
  std::get<1>(c) = y;
}