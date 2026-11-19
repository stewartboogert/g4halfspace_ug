#include "DetectorConstruction.hh"

#include "G4NistManager.hh"
#include "G4Box.hh"

#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

#include "G4HalfIntersectionSolid.hh"
#include "G4HalfQuadric.hh"
#include "G4HalfUnionSolid.hh"
#include "G4HalfPlane.hh"


G4HalfIntersectionSolid* makeCube(G4double halfLength)
{
    const std::vector<G4HalfSurface*> cubeList({
        new G4HalfPlane(G4ThreeVector(1,0,0), halfLength),
        new G4HalfPlane(G4ThreeVector(0,1,0), halfLength),
        new G4HalfPlane (G4ThreeVector(0,0,1), halfLength),
        new G4HalfPlane (G4ThreeVector(-1,0,0), halfLength),
        new G4HalfPlane (G4ThreeVector(0,-1,0), halfLength),
        new G4HalfPlane (G4ThreeVector(0,0,-1), halfLength)
    });

    return new G4HalfIntersectionSolid(cubeList, {}, "Cube");
}
G4HalfIntersectionSolid* makeQuadric(G4double R)
{
    std::vector<G4HalfSurface*> quadList({new G4HalfQuadric(R)});
    return new G4HalfIntersectionSolid(quadList, {}, "Quadric");
}

DetectorConstruction::DetectorConstruction(const G4String& flukaFile)
{
    fFlukaLoader.Load(flukaFile);
}

G4VPhysicalVolume* DetectorConstruction::Construct() {

  std::cout << "DetectorConstruction::Construct" << std::endl;
  std::cout << "Loaded " << fFlukaLoader.Cards().size()
            << " FLUKA cards from the input deck" << std::endl;

  //////////////////////////////
  // Get nist material manager
  G4NistManager* nist = G4NistManager::Instance();

  //////////////////////////////
  // Envelope parameters
  G4double env_sizeXY = 1000*cm, env_sizeZ = 1000*cm;
  G4Material* env_mat = nist->FindOrBuildMaterial("G4_Al");

  //////////////////////////////
  // World
  G4double world_sizeXY = env_sizeXY;
  G4double world_sizeZ  = env_sizeZ;
  G4Material* world_mat = nist->FindOrBuildMaterial("G4_Galactic");

  auto solidWorld = new G4Box("World",
                              0.5 * world_sizeXY,
                              0.5 * world_sizeXY,
                              0.5 * world_sizeZ);

  auto logicWorld = new G4LogicalVolume(solidWorld,
                                        world_mat,
                                        "World");

  auto physWorld = new G4PVPlacement(nullptr,
                                     G4ThreeVector(),
                                     logicWorld,
                                     "World",
                                     nullptr,
                                     false,
                                     0,
                                     false);

    G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");

    G4ThreeVector shapePosition;

    G4HalfIntersectionSolid* cube = makeCube(0.5 * m);
    G4HalfIntersectionSolid* translatedCube = makeCube(0.5 * m);
    G4HalfIntersectionSolid* quadric = makeQuadric(1000000);

    const G4AffineTransform translation(G4ThreeVector(-0.5 * m, -0.5 * m, -0.5 * m));
    translatedCube->Transform(translation);

    const std::vector<G4HalfSolid*> unionList({cube, translatedCube});
    const std::vector<G4HalfSolid*> quadricUnionList({quadric});
    /*
    const std::vector<G4HalfSolid*> unionList({
        cube,
        translatedCube
    });
    */
    auto* megaCube = new G4HalfUnionSolid(unionList, "megaCube");
    //auto* quadric_sphere = new G4HalfUnionSolid(quadricUnionList, "quadric");
    auto* logicShape = new G4LogicalVolume(megaCube,
                                            shape2_mat,
                                            "Shape");
    new G4PVPlacement(nullptr,  // no rotation
                    shapePosition,  // at position
                    logicShape,  // its logical volume
                    "Shape",  // its name
                    logicWorld,  // its mother  volume
                    false,  // no boolean operation
                    0,  // copy number
                    false);
  return physWorld;
}

