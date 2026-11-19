#include "G4HalfQuadric.hh"

#include "G4BooleanSolid.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4RunManagerFactory.hh"
#include "G4TransportationManager.hh"
#include "G4Types.hh"
#include "G4UIExecutive.hh"
#include "G4UImanager.hh"
#include "G4VisExecutive.hh"
#include "G4ios.hh"
#include "FTFP_BERT.hh"
#include "G4SystemOfUnits.hh"

#include "ActionInitialization.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    G4cerr << "Usage: " << argv[0] << " <fluka-input> [Geant4 arguments]"
           << G4endl;
    return 1;
  }

  auto* runManager = G4RunManagerFactory::CreateRunManager();

    //G4cout << "BEGIN SPHERE TEST" << G4endl;
    auto sphere = G4HalfQuadric();
    sphere.set_R(-3);
    G4RotationMatrix rotation;
    rotation.rotateZ(30*deg);
    //G4cout << "MID SPHERE TEST" << G4endl;
    G4AffineTransform rotate(rotation);
    sphere.Transform(rotate);
    //G4ThreeVector eigenvals;
    //sphere.GetRotationMatrix(eigenvals);
    //G4cout << "END SPHERE TEST" << G4endl;
  runManager->SetUserInitialization(new DetectorConstruction(argv[1]));
  runManager->SetUserInitialization(new ActionInitialization);
  runManager->SetUserInitialization(new FTFP_BERT);

  runManager->Initialize();

  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  runManager->BeamOn(0);

  if (argc == 5)  // batch mode
  {
      G4String command = "/control/execute ";
      G4String fileName = argv[4];
      UImanager->ApplyCommand(command + fileName);
  }
  else  // interactive mode
  {
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
      UImanager->ApplyCommand("/control/execute vis.mac");
      ui->SessionStart();
      delete ui;
  }

  delete visManager;
  delete runManager;

  return 0;
}