#pragma once
#include <iostream>
#include <memory>

struct Gene_base
{
  virtual std::string AsString() const = 0;
  virtual void SetValue(const std::string line) = 0;
};

using Gene = std::unique_ptr<Gene_base>;