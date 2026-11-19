#pragma once

#include "G4FLUKALoader.hh"
#include "G4VUserDetectorConstruction.hh"

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    explicit DetectorConstruction(const G4String& flukaFile);

    virtual G4VPhysicalVolume* Construct();

private:
    G4VPhysicalVolume* fWorld;
    G4FLUKALoader fFlukaLoader;
};