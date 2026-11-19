#ifndef GEANT4_G4FLUKALOADER_HH
#define GEANT4_G4FLUKALOADER_HH

#include "G4String.hh"
#include "G4Types.hh"

#include <array>
#include <cstddef>
#include <vector>

class G4FLUKALoader
{
public:
  struct Card
  {
    G4String keyword;
    std::array<G4String, 6> what;
    G4String sDum;
    G4String raw;
    std::size_t lineNumber = 0;

    G4double WhatAsDouble(std::size_t index) const;
  };

  void Load(const G4String& filename);
  void LoadText(const G4String& text);

  const std::vector<Card>& Cards() const;
  const std::vector<G4String>& RawLines() const;

private:
  void ParseLine(const G4String& line, std::size_t lineNumber);

  std::vector<Card> fCards;
  std::vector<G4String> fRawLines;
};

#endif
