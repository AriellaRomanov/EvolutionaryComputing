#include "shape.h"

Shape::Shape(const std::string dirs)
  : origin(0, 0), rotation(Dir::UP)
{
  SetDirections(dirs);
}

Shape::Shape(const Shape& copy)
  : max_x(copy.max_x), max_y(copy.max_y), min_x(copy.min_x),
    min_y(copy.min_y), origin(copy.origin), rotation(copy.rotation),
    directions(copy.directions), coords(copy.coords)
{}

void Shape::SetValue(const std::string line)
{
  auto pieces = split(line, " ");
  if (static_cast<long>(pieces.size()) == 3)
  {
    auto ox = atoi(pieces.at(0).c_str());
    auto oy = atoi(pieces.at(1).c_str());
    auto r = atoi(pieces.at(2).c_str());
    SetOrigin(ox, oy, static_cast<Dir>(r));
  }
}

void Shape::SetDirections(const std::string dirs)
{
  directions = dirs;
  coords.clear();

  Coord x = 0;
  Coord y = 0;
  max_x = 0;
	max_y = 0;
	min_x = 0;
	min_y = 0;

  coords.insert(std::make_pair(x, y));

  auto steps = split(dirs, " ");
  for (auto s : steps)
  {
    char dir_c = s.at(0);
    long mag = atoi(s.substr(1).c_str());
    for (long i = 0; i < mag; i++)
    {
      switch (dir_c)
      {
      case 'U': y++; break;
      case 'R': x++; break;
      case 'D': y--; break;
      case 'L': x--; break;
      default: break;
      }
      coords.insert(std::make_pair(x, y));

      if (x < min_x) min_x = x;
      if (x > max_x) max_x = x;
      if (y < min_y) min_y = y;
      if (y > max_y) max_y = y;
    }
  }
}

void Shape::SetOrigin(const Coord x, const Coord y, const Dir rot)
{
  origin = std::make_pair(x, y);
  rotation = rot;
}

Coordinate Shape::GetOrigin() const
{
  return origin;
}

std::set<Coordinate> Shape::GetCoords() const
{
  auto x_org = origin.first;
  auto y_org = origin.second;

  Coords r_coords;
  for (const auto& c : coords)
  {
    auto x = c.first;
    auto y = c.second;
    switch (rotation)
    {
    case Dir::UP:
      r_coords.insert(std::make_pair(x + x_org, y + y_org));
      break;
    case Dir::RIGHT:
      r_coords.insert(std::make_pair(y + x_org, -x + y_org));
      break;
    case Dir::DOWN:
      r_coords.insert(std::make_pair(-x + x_org, -y + y_org));
      break;
    case Dir::LEFT:
      r_coords.insert(std::make_pair(-y + x_org, x + y_org));
      break;
    default:
      break;
    }
  }
  return r_coords;
}

long Shape::GetXDimension() const
{
  return max_x - min_x + 1;
}

long Shape::GetYDimension() const
{
  return max_y - min_y + 1;
}

long Shape::GetXOrigin() const
{
  return origin.first;
}

long Shape::GetYOrigin() const
{
  return origin.second;
}

Dir Shape::GetOrientation() const
{
  return rotation;
}

std::string Shape::GetOriginString(const std::string delim) const
{
  return std::to_string(origin.first) + delim +
        std::to_string(origin.second) + delim +
        std::to_string(static_cast<long>(rotation));
}

std::string Shape::AsString() const
{
  std::string shp = "Shape: (" + std::to_string(GetXDimension()) + " x "
    + std::to_string(GetYDimension()) + ") " + directions
    + "\n  Placed at: " + GetOriginString() + "\n";
  auto coords = GetCoords();
  for (const auto& c : coords)
    shp.append("  (" + std::to_string(c.first) + ", " + std::to_string(c.second) + ")");
  return shp;
}

