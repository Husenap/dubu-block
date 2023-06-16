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

- [ ] Implement the possibility of querying a block on specific coordinates
- [ ] Fix ambient occlusion between chunks
- [ ] Implement a firs person avatar that can run around in the world
- [ ] Fix github language stats