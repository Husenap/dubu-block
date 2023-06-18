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

### Todo

- [x] Implement the possibility of querying a block on specific coordinates
- [ ] See if it's a good idea to replace BlockId directly with BlockType
- [x] Fix ambient occlusion between chunks
- [ ] Rewrite ambient occlusion to make less queries, 9 instead of 12
- [ ] Rework the chunk generation system (parallelize, add stages, delay mesh generation, neighbour access)
- [ ] Implement a first person avatar that can run around in the world
- [ ] Figure out a way to color leaves and grass using mask
- [ ] Refactor code to allow more types of renderers
- [ ] Implement a basic text renderer
- [ ] Build a UI system?
- [ ] Figure out how to add imgui docking branch to meson
- [x] Fix github language stats