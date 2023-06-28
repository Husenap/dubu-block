# dubu-block

## A blocky game built on C++/GLFW/OpenGL

### Prerequisites

Enable `Developer Mode` on windows to be able to symlink the assets directory without administrator privileges.

### Setup

#### Setup and Entering devenv
```bash
meson setup .build
cd .build/
```

#### Compile and run
```bash
meson install
./dubu-block/dubu-block
```

### Screenshots

![](screenshots/screenshot.png)

### Todo-list

#### Todo
- [ ] Rework the chunk generation system (parallelize, add stages, delay mesh generation, neighbour access)
- [ ] Implement a first person avatar that can run around in the world
- [ ] Figure out a way to color leaves and grass using mask (how to differentiate between transparency and color masking?)
- [ ] Refactor code to allow more types of renderers (add deferred renderer?)
- [ ] Implement a basic text renderer (SDF vs simple bitmap font?)
- [ ] Build a UI system?
- [ ] Add Biomes
- [ ] Figure out how to generically implement liquids such as water and lava
- [ ] Add an atmospheric scattering shader
- [ ] Serialize chunks to disk
- [ ] Add support for different world saves on disk
- [ ] Store imgui.ini out of source
- [ ] Add foliage
- [ ] Calculate Lighting data in chunks
- [ ] Serialize chunk generator curves
- [ ] Ambient Occlusion uses vertex shading which creates diagonal spikes, figure out a way to fix it
- [ ] Implement support for entity geometry files and texturing

#### Done
- [x] Implement the possibility of querying a block on specific coordinates
- [x] Fix ambient occlusion between chunks
- [x] Fix github language stats
- [x] Figure out how to add imgui docking branch to meson
- [x] Rewrite ambient occlusion to make less queries (9 instead of 12? Perhaps only 1 if adjacent block is transparent)
- [x] Implement a basic input system (make sure it gives ImGui precedence)
- [x] Figure out how to add seeding
- [x] See if it's a good idea to replace BlockId directly with BlockType