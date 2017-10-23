#pragma once
#include <utility>
#include <set>

using Coord = long;
using Coordinate = std::pair<Coord, Coord>;
using Coords = std::set<Coordinate>;

// Returns or sets the appropriate member of the
// Coordinate pair. Required for consistency.
// This can't be a full class/struct (which
// would be easier to read), because std::set
// requires a hashing function that I don't
// want to write
Coord GetX(const Coordinate c);
Coord GetY(const Coordinate c);
void SetX(Coordinate& c, const Coord x);
void SetY(Coordinate& c, const Coord y);