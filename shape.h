#pragma once
#include <vector>
#include <set>
#include "evAPI.h"
#include "coordinate.h"
#include "Gene.h"

// use an enumeration class for consistency and
// readability. Including NUM_DIRS on the end
// makes it easy to do modulo on the number of
// directions
enum class Dir { UP, RIGHT, DOWN, LEFT, NUM_DIRS };

// Describes an individual shape in the problem set
// Moves are stored in their original orientation.
class Shape : public Gene_base
{
	Coord max_x;
	Coord max_y;
	Coord min_x;
	Coord min_y;
	Coordinate origin;
	Dir rotation;
	std::string directions;
	Coords coords;

public:
	Shape(const std::string dirs);
	Shape(const Shape& copy);

	void SetValue(const std::string line);

	void SetDirections(const std::string dirs);

	void SetOrigin(const Coord x, const Coord y, const Dir rot);

	Coordinate GetOrigin() const;

	Coords GetCoords() const;

	long GetXDimension() const;

	long GetYDimension() const;
	
	long GetXOrigin() const;

	long GetYOrigin() const;

	Dir GetOrientation() const;
  
  std::string GetOriginString(const std::string delim = "\t") const;

	std::string AsString() const;
};

using Shapes = std::vector<std::unique_ptr<Shape>>;