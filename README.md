# dubu-block

## A blocky game built on C++/GLFW/OpenGL

### Prerequisites

Enable `Developer Mode` on windows to be able to symlink the assets directory without administrator privileges.

### Setup

#### Setup and Entering devenv
```bash
meson setup .build
meson devenv -C .build/
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
- [ ] See if it's a good idea to replace BlockId directly with BlockType
- [ ] Rewrite ambient occlusion to make less queries (9 instead of 12? Perhaps only 1 if adjacent block is transparent)
- [ ] Rework the chunk generation system (parallelize, add stages, delay mesh generation, neighbour access)
- [ ] Implement a basic input system (make sure it gives ImGui precedence)
- [ ] Implement a first person avatar that can run around in the world
- [ ] Figure out a way to color leaves and grass using mask (how to differentiate between transparency and color masking?)
- [ ] Refactor code to allow more types of renderers (add deferred renderer?)
- [ ] Implement a basic text renderer (SDF vs simple bitmap font?)
- [ ] Build a UI system?
- [ ] Add Biomes
- [ ] Figure out how to generically implement liquids such as water and lava
- [ ] Add a atmospheric scattering shader
- [ ] Serialize chunks to disk
- [ ] Figure out how to add seeding
- [ ] Add support for different world saves on disk
- [ ] Store imgui.ini out of source

#### Done
- [x] Implement the possibility of querying a block on specific coordinates
- [x] Fix ambient occlusion between chunks
- [x] Fix github language stats
- [x] Figure out how to add imgui docking branch to meson