#include "G4FLUKALoader.hh"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
std::string Trim(const std::string& value)
{
  const auto first = std::find_if(
      value.begin(), value.end(),
      [](unsigned char character) { return !std::isspace(character); });
  const auto last = std::find_if(
      value.rbegin(), value.rend(),
      [](unsigned char character) { return !std::isspace(character); })
                        .base();
  return first < last ? std::string(first, last) : std::string();
}

std::string Field(const std::string& line, std::size_t start, std::size_t width)
{
  if (start >= line.size()) {
    return {};
  }
  return Trim(line.substr(start, std::min(width, line.size() - start)));
}
}  // namespace

G4double G4FLUKALoader::Card::WhatAsDouble(std::size_t index) const
{
  if (index >= what.size()) {
    throw std::out_of_range("FLUKA WHAT field index must be less than 6");
  }
  if (what[index].empty()) {
    return std::nan("");
  }

  std::size_t characters = 0;
  const std::string value = what[index];
  const double result = std::stod(value, &characters);
  if (characters != value.size()) {
    throw std::invalid_argument("Invalid FLUKA WHAT value: " + value);
  }
  return result;
}

void G4FLUKALoader::Load(const G4String& filename)
{
  if (filename.empty()) {
    throw std::invalid_argument("G4FLUKALoader requires a FLUKA filename");
  }

  std::ifstream input(filename.c_str());
  if (!input) {
    throw std::runtime_error("Could not open FLUKA file: " +
                             std::string(filename));
  }

  std::ostringstream contents;
  contents << input.rdbuf();
  LoadText(contents.str());
}

void G4FLUKALoader::LoadText(const G4String& text)
{
  fCards.clear();
  fRawLines.clear();

  std::istringstream input(text);
  std::string line;
  std::size_t lineNumber = 0;
  while (std::getline(input, line)) {
    ++lineNumber;
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }
    fRawLines.emplace_back(line);
    ParseLine(line, lineNumber);
  }
}

const std::vector<G4FLUKALoader::Card>& G4FLUKALoader::Cards() const
{
  return fCards;
}

const std::vector<G4String>& G4FLUKALoader::RawLines() const
{
  return fRawLines;
}

void G4FLUKALoader::ParseLine(const G4String& line, std::size_t lineNumber)
{
  const std::string value = line;
  if (value.empty() || value[0] == '*' || value[0] == '#') {
    return;
  }

  const std::string keyword = Field(value, 0, 10);
  if (keyword.empty()) {
    return;
  }

  Card card;
  card.keyword = keyword;
  card.raw = line;
  card.lineNumber = lineNumber;
  for (std::size_t index = 0; index < card.what.size(); ++index) {
    card.what[index] = Field(value, 10 + index * 10, 10);
  }
  card.sDum = Field(value, 70, 10);
  fCards.push_back(card);
}
