#ifndef _DATALOADER_GUARD
#define _DATALOADER_GUARD

#include <memory>
#include <string>
#include "Graph.h"

class DataLoader {
    public:
        DataLoader(std::string Directory) : Directory(std::move(Directory)) { }
        std::unique_ptr<Graph> LoadFromFiles();
    private:
        std::string Directory;
};

#endif