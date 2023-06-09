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