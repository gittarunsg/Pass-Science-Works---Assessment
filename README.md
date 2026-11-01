# RA2311003020323 Engineering Workbench

Source prototype for the PASS Science Works R&D Internship 2026-2027 common assignment. Backend and computation are C++17. This archive does **not** claim to be a finished, submission-ready assessment: this machine has no C++ compiler/CMake, so no executable, UI, screenshots, machine benchmark, or launch-to-ready evidence is included. The handwritten appendix and candidate-run demo are also left for the candidate.

## Build and run (after installing CMake 3.20+ and a C++17 compiler)

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

The current implementation is a C++ command-line prototype (not the required polished visual UI/one-click packaged app). From the project root, run `build\Release\workbench.exe fea results\fea_N40.csv`, `... inspect data\sample_temperature_synthetic.csv`, `... inspect data\sample_geometry_ascii.stl`, `... batch 1000 80`, `... nn`, or `... devices`. On single-config generators the executable is `build\workbench.exe`.

## Contents

- `src/`: rod FEA, CSV/STL validation, deterministic parallel batch, NN gradient, and simulated device adapters.
- `tests/`: CTest source covering boundary conditions, finite difference, deterministic parallel results and device inventory.
- `data/`: self-created ASCII tetrahedron and synthetic nodal temperature CSV. Neither represents ANSYS output.
- `results/`: analytical FEA reference values and a performance measurement template. The template is intentionally unfilled because the C++ binary could not be run here.
- `docs/`: design/report notes, pin assumptions, and AI-use log.
- `CMakeLists.txt`: dependency-free C++17 build except for standard library threads.

## Known gaps against the brief

1. Build and test the code with the candidate's compiler; repair any compiler-specific issues and include actual logs.
2. Implement the four-view interactive UI, progress/cancellation, 3D rotate/pan/zoom, plots and export in the app.
3. Package a tested Windows 11 x64 `Launch.exe` with redistributables and a matching editor launch configuration.
4. Run the prescribed five-repetition benchmark on the candidate machine; fill CPU/core/RAM/compiler/flags, medians, speedup, efficiency, and launch-to-ready time.
5. Complete the handwritten two-page derivation in the candidate's own hand and record a 3-5 minute demo or timestamped screenshots/logs.
6. Inspect the code, understand it, and replace the AI-use log's candidate reflection placeholder before submission.

The reference CSV is analytical and does not claim to have been emitted by the unbuilt executable. Do not present the package as a successful packaged application until the gaps above are closed.
