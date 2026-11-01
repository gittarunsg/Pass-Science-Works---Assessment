# Engineering Workbench - Report Draft

Candidate ID: RA2311003020323  
Status: source prototype; not packaged or machine-verified

## Design and boundaries

The C++17 core isolates numerical routines and file validation in `src/engine.cpp`. `src/main.cpp` is a small command-line adapter. The code uses only the C++ standard library, including `std::thread`. This is a small auditable prototype. The required responsive multi-view UI and one-click Windows application are not implemented in this environment.

## Thermal FEA

For each linear element, the stiffness is `(k A / h) [[1,-1],[-1,1]]` and the consistent uniform source load is `q A h / 2 [1,1]`. Dirichlet values are imposed by elimination. With constant properties, insulated sides and prescribed end temperatures, the exact solution is `T(x)=100-80x+500x(1-x)` in SI units over `L=1 m`. The source terms are `q=10,000 W/m^3`, `k=10 W/(m K)`, `A=1e-4 m^2`. Linear interpolation at each element midpoint gives error `q h^2/(8k)`; expected max errors are in `results/fea_convergence_reference.csv`. This is an analytical reference, not executable verification. The key implementation detail to check during candidate verification is elimination of both endpoint terms while retaining a symmetric interior tridiagonal system.

## File formats

`data/sample_geometry_ascii.stl` is a self-created tetrahedron in arbitrary length units (no scale is encoded in STL). `data/sample_temperature_synthetic.csv` is a self-created synthetic point set; x is in metres and temperature in Celsius. It is not an ANSYS export. The code checks the exact CSV header, row width, parseability and finite numeric coordinates/temperatures; STL support currently counts ASCII facets only. Mesh registration is not attempted.

## Parallel execution

Batch cases use a fixed seed, independently generated positive conductivity and heat generation, a bounded worker count, and result slots indexed by input order. The included command times a single run and compares values to serial; it does not yet implement five repetitions, median, CPU/RAM/compiler reporting, or launch-to-ready timing. No benchmark numbers are claimed. Complete `results/benchmark_template.csv` on the target machine.

## Embedded design

Mocks represent a 3.3 V MCU, an FPGA bank assumed to be 1.8 V, and a target-referenced JTAG adapter. These are design assumptions, not board-specific pinout facts. See `embedded_interfaces.md`. A JTAG adapter is a debug transport that drives protocol signals to a target; it is not a general compute module. Do not wire hardware until exact board revisions, pinout, power direction, and voltage tolerance are checked against manufacturer documentation.

## Handwritten backpropagation

With `h=tanh(wx+b)`, `ŷ=vh+c`, `e=ŷ-y`, `J=e²/2`, the derivatives are `dJ/dv=e h`, `dJ/dc=e`, `dJ/dw=e v (1-h²)x`, `dJ/db=e v (1-h²)`. At the initial parameters, `h≈0.462117`, `ŷ≈0.462117`, `e≈-1.537883`, and the gradients are approximately `dw=db=-1.2095`, `dv=-0.7108`, `dc=-1.5379`. One simultaneous step at `η=.1` uses only these original gradients: `w≈.62095, b≈.12095, v≈1.07108, c≈.15379`. Negative gradients move parameters upward under gradient descent. The candidate must produce and attach the requested handwritten pages; none are fabricated here.

## Verification and limitations

No C++ compiler or CMake was available in the authoring environment. The test suite is authored but unexecuted. The candidate must compile, run CTest, verify finite-difference errors for all four parameters, add the 100-step loss plot, and capture real UI/benchmark evidence. No score, shortlist result, physical-device operation, or standalone launch is claimed.
