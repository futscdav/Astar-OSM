clang++ --std=c++17 -O3 -Isrc -c -o Dataloader.o src/DataLoader.cpp
clang++ --std=c++17 -O3 -Isrc -c -o RoutePlanner.o src/RoutePlanner.cpp
clang++ --std=c++17 -O3 -Isrc -c -o Utils.o src/Utils.cpp
clang++ --std=c++17 -O3 -Isrc -c -o Graph.o src/Graph.cpp
clang++ --std=c++17 -O3 -Isrc -c -o Main.o src/main.cpp
clang++ --std=c++17 -O3 -o Astar.exe Main.o Graph.o Utils.o RoutePlanner.o Dataloader.o

@REM clang++ --std=c++17 -O3 -Isrc -o Astar2.exe src/*.cpp