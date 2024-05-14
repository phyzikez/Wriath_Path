A brief description of the project

  This project called “Wraith Path” is a first-person shooter game, that represents the implementation of different techniques allowed by UE5. There is about 15 km of playable walkthrough-level, divided into 4 main locations. The project was made as a blank C++ template. Almost whole mechanics and interactions were written in C++ part. Different light-parts such as HUD, animations, effects, etc. were realized via blueprints. The main part of geometry was taken from UE-content library (megascans and free models), the another were made in 3ds Max (including Max Script for some animations). 
 
  For enemies were taken characters from free Paragon-project content, including animations. There are 4 types of enemies, including 1 non-rifle beast enemy. As a model for player character was chosen Wraith character, which mechanics and animations were significantly remade. The weapon arsenal includes 5 types of rifles and 2 types of grenades. The minigun was completely made by self. The other were taken from free-content or borrowed from Paragon characters, significantly remade and adopted according to player kinematics. Also, character has 4 additional abilities: arm power shock wave, hexagonal dome shield, ultimate rocket launcher and ultra-vision. A row of textures and materials were created in UE and PS.

  By the following reference you can find video demonstrations of main techniques, realized in project: 
  https://drive.google.com/drive/folders/1DYF-xJm63aRaTtvGihGr_tBthqZ2Q0V6
  
Here is a list of techniques used in current project:
-	Geometry:
    --	Static And Skeletal meshes
    --	Quixel meshes
    --	Landscapes
    --	Spline Mesh Dynamic Generating
    --	GeoCollections
-	Mechanics:
  --	Components
  --	Transforms/Offsets
  --	Interpolations 
  --	Controllers
  --	Movement System (Jumping, Landing, Crouching, Climbing)
  --	Bind Axis/Actions
  --	Timers
  --	Collision System
  --	Physics System (including Impulse, Force, Mass, Gravity)
  --	Tick And Overlapping System
  --	Triggers
  --	Camera Manipulation
  --	Projection/Tracing
  --	Handle/Event System
-	Animation:
    --	Animation Sequences (including UE/3ds Max made)
    --	Montages (including Section system)
    --	Animation Instances
    --	Physics Assets
    --	State Machines
    --	Locomotion System
    --	Blending System
    --	Aim Offset
    --	Blend Spaces
    --	Skeletal System (including bone manipulations such as attachments, movements, visibility, hits)
    --	Notify System
    --	Slots
    --	Socket System
    --	IK Retargeter
    --	IK Rig
    --	AI Deepmotion technology
-	AI:
    --	Blackboards
    --	Behavior Trees
    --	Services
    --	Decorators
    --	Pawn Sensing
    --	EQ
    --	Tasks (system and user defined)
-	Look And Effects:
    --	Texture2d/Brushes
    --	Material Instance
    --	Niagara Emission Particle System
    --	Post Effects
-	Audio:
    --	Sound Cues (including point and ambient types, randomizing)
-	UI:
    --	Widget System (including dynamic effects and functional binding)
    --	HUD (including adapting crosshair)
    --	Multilayered game menu (including weapon configuration menu)
-	Data:
    --	Datatables (enemies, weapons, pickup-items, HUD texts)
    --	Enumerations (types and states)
    --	UE/STL-containers
    --	Curves
    --	Bit sets
    --	CSV-files
-	Additional:
    --  Kismet System
    --	Math Library
    --	Gameplay Statics
    --	Print/Debug Features
