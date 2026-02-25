# Adventure Map format
* version: 1.0

In the Fog of War engine maps are consists of 2 files:
- Map definition (MapInfo.xml)
- Map index file (Index.bin)

These files are located in /Maps/(map name) inside the game assets

Maps can be packaged in a game archive or mod.

## Map files
Inside the "maps" folder found in the game's root directory you can find the map files.

These are zip archives with ".fmap" extension, containing assets used by the map.
The file structure is the same as in mods or game archives.

## Data examples

- MapInfo.xml
```xml
<?xml version="1.0" encoding="UTF-8" ?>
<AdvMap index="/Maps/MapName/Index.bin">
    <!-- Display data -->
    <Name>Map's name</Name>
    <Description>This is the map's description you see in the game.</Description>
    <PreviewImage>/Maps/MapName/preview.png</PreviewImage>
    <!--
        List of allowed difficulty levels.
        Difficulty levels defined by the game developer
    -->
    <Difficulties default="NORMAL">
        <Difficulty>EASY</Difficulty>
        <Difficulty>NORMAL</Difficulty>
        <Difficulty>HARD</Difficulty>
        <Difficulty>EXPERT</Difficulty>
        <Difficulty>IMPOSSIBLE</Difficulty>
    </Difficulties>
    <!-- Settings per player -->
    <PlayerInfo maxlevel="20">
        <!-- Player 1 -->
        <Player aionly="false" team="0" needs_town="true">
            <!--
                Limit the allowed factions for this player.
                If not defined, all factions are allowed
            -->
            <AllowedFactions>
                <Faction>FactionName</Faction>
            </AllowedFactions>
            <!-- Main town's tag to be used for goals -->
            <MainTown>Player0MainTown</MainTown>
        </Player>
        <!-- Player 2 -->
        <Player aionly="false" team="1" needs_town="true"/>
    </PlayerInfo>
    <!-- Environment settings -->
    <Environment>
        <!-- Skybox texture -->
        <SkyBox>/Textures/Skybox.texture.xml</SkyBox>
        <!-- R, G, B, intensity -->
        <SunLight>1.0, 0.75, 0.125, 10.0</SunLight>
        <!-- Sun angle in degrees -->
        <SunAngle>30</SunAngle>
    </Environment>
    <!-- Win and lose conditions -->
    <Goals>
        <Victory>
            <!-- Victory conditions defined by the game developer -->
            <Condition>DEFEAT_ALL</Condition>
        </Victory>
        <Defeat>
            <!-- Defeat conditions defined by the game developer -->
            <Condition>LOSE_ALL</Condition>
        </Defeat>
    </Goals>
    <!-- White lists -->
    <!-- Hero white list. If not defined, all heroes are allowed. -->
    <AllowedHeroes>
        <!-- Name of allowed heroes -->
        <AllowedHero>JohnSmith</AllowedHero>
        <AllowedHero>JaneDoe</AllowedHero>
    </AllowedHeroes>
    <!-- Artifact white list. If not defined, all artifacts are allowed. -->
    <AllowedArtifacts>
        <!-- Name of allowed artifacts -->
        <AllowedArtifact>NiceHat</AllowedArtifact>
        <AllowedArtifact>CubeOfDoom</AllowedArtifact>
    </AllowedArtifacts>
    <!-- Spell white list. If not defined, all skills are allowed. -->
    <AllowedSpells>
        <!-- Name of allowed spells -->
        <AllowedSpell>MagicMissile</AllowedSpell>
        <AllowedSpell>FireBall</AllowedSpell>
    </AllowedSpells>
</AdvMap>
```
- Index.bin
### File structure
#### Header

| Entry name | Data size | Description                                                         |
|------------|-----------|---------------------------------------------------------------------|
| Identifier | 4 bytes   | 4 ASCII characters used to identify the file. It's value is 'FMAP'. |
| Version    | 2 bytes   | 1 byte for major and 1 byte for minor version.                      |
| Map size   | 2 bytes   | 1 byte for width and 1 byte for height                              |

####  Tile palette

First 4 bytes for the tile count, then list of tiles used on the map, 
these are null terminated strings pointing to the AdvMapTile definition.

#### Tile indices

List of entries, the count is equals to map width * height.

| Entry name        | Data size | Description               |
|-------------------|-----------|---------------------------|
| Tile index        | 2 bytes   | Index to the tile palette |
| Tile height       | 1 byte    | Tile height               |
| Tile terrain type | 1 byte    | Terrain type index        |

Tile terrain types:
- Flat = 0
- River, straight = 1
- River, turn = 2
- River, junction = 2
- River, cross = 3
- Cliff, straight = 4
- Cliff, ramp = 5
- Cliff, inner corner = 6
- Cliff, outer corner = 7
- Cliff, straight, waterfall = 8
- Shore, straight = 9
- Shore, straight to river = 10
- Shore, inner corner = 11
- Shore, outer corner = 12
- Shore, outer corner to river (x axis) = 13
- Shore, outer corner to river (z axis) = 14
- Shore, outer corner to river cross = 15

#### Objects

First 2 bytes used for object count.<br/>
After that a list of object parameters defined:

Each entry has the following entries:

| Entry name                  | Data size | Description                                                      |
|-----------------------------|-----------|------------------------------------------------------------------|
| Object definiton asset path | x bytes   | Null terminated string, path to the AdvMapObj xml definition.    |
| Tag                         | x bytes   | Null terminated string, used as a unique identifier for scripts. |
| X position                  | 1 byte    | Position along the X axis.                                       |
| Y position                  | 1 byte    | Position along the Y axis.                                       |
| Z position                  | 1 byte    | Position along the Z axis.                                       |
| Angle                       | 2 bytes   | Rotation angle in degrees.                                       |
| Parameter count             | 2 bytes   | Amount of user defined parameters                                |

User defined parameter list is followed by the header table, each contains the following entries:

| Entry name     | Data size | Description                                          |
|----------------|-----------|------------------------------------------------------|
| Parameter name | x bytes   | Null terminated string, used for the parameter name. |
| Parameter type | 1 byte    | Type of the parameter value.                         |

Parameter value size varies depending on its type:

- bool (0), size: 1 byte
- int (1), size: 4 bytes
- uint (2), size: 4 bytes
- float (3), size: 4 bytes
- vec2 (4), size: 8 bytes (2 float values)
- vec3 (5), size: 12 bytes (3 float values)
- vec4 (6), size: 16 bytes (4 float values)
- string (7), null terminated string
- asset (8), two null terminated strings, first one used for class name, second is the asset path.