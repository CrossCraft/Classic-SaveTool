#include <iostream>
#include <fstream>
#include <zlib.h>

uint8_t* worldData;

void validate_infile(std::string file){
    std::ifstream i(file);

    if(!i.is_open()){
        std::cerr << "File: " << file << " could not be opened!" << std::endl;
        exit(EXIT_FAILURE);
    }
    i.close();
}

void load_world(std::string inFile){
    gzFile save_file = gzopen(inFile.c_str(), "rb");

    uint8_t *temp = (uint8_t *)malloc(256 * 64 * 256);

    gzread(save_file, temp, 256 * 64 * 256);
    gzclose(save_file);

    for (auto x = 0; x < 256; x++)
        for (auto y = 0; y < 64; y++)
            for (auto z = 0; z < 256; z++) {
                auto idx_destiny = (y * 256 * 256) + (z * 256) + x;

                auto idx_source = (x * 256 * 64) + (z * 64) + y;
                worldData[idx_destiny] = temp[idx_source];
            }

    free(temp);
}

void save_data(std::string outFile){
    gzFile save_file = gzopen(outFile.c_str(), "wb");

    uint8_t *temp = (uint8_t *)malloc(256 * 64 * 256);

    for (auto x = 0; x < 256; x++)
        for (auto y = 0; y < 64; y++)
            for (auto z = 0; z < 256; z++) {
                auto idx_source = (y * 256 * 256) + (z * 256) + x;

                auto idx_destiny = (x * 256 * 64) + (z * 64) + y;
                temp[idx_destiny] = worldData[idx_source];
            }

    if (save_file != nullptr) {
        const int save_version = 1;
        gzwrite(save_file, &save_version, 1 * sizeof(int));
        gzwrite(save_file, temp, 256 * 64 * 256);
        gzclose(save_file);
    }

    free(temp);
}



int main(int argc, char** argv){
    if(argc < 4){
        std::cerr << "Invalid usage: ccsavetool <file> <command> <output>" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto inFile = std::string(argv[1]);
    auto cmd = std::string(argv[2]);
    auto outFile = std::string(argv[3]);

    validate_infile(inFile);
    worldData = (uint8_t*)calloc(256 * 64 * 256, sizeof(uint8_t));
    load_world(inFile);

    if(cmd == "drain"){
        for(int x = 0; x < 256; x++)
            for(int y = 0; y < 64; y++)
                for(int z = 0; z < 256; z++){
                    auto idx = (x * 256 * 64) + (z * 64) + y;
                    if(y > 32){
                        auto blk = worldData[idx];

                        if(blk == 8 || blk == 9 || blk == 10 || blk == 11){
                            worldData[idx] = 0;
                        }
                    }
                }
    }
    
    save_data(outFile);

    return EXIT_SUCCESS;
}