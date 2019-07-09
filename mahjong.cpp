//
//  mahjong.cpp
//  mjlogic
//


#include "mahjong.hpp"
#include <random>

namespace {
    int MAX(int a, int b){return a > b ? a : b;}
    int MIN(int a, int b){return a < b ? a : b;}
    
    std::default_random_engine& getRandomEngine(){
        static std::random_device seed_gen;
        static std::default_random_engine engine(seed_gen());
        return engine;
    }
}

mjGame::~mjGame()
{
    for (auto foo : blocks) {
        delete foo;
    }
}

void mjGame::parse(const std::string& str){
    int minlay = 999, maxlay = -999;
    int minrow = 999, maxrow = -999;
    int mincol = 999, maxcol = -999;

    // load game from a string
    // like "000002004006"
    // this is just an example
    for (size_t i = 0; i + 2 < str.size(); i += 3) {
        auto foo = new mjBlock();
        foo->layer = str[i] - '0';
        foo->row = str[i + 1] - '0';
        foo->column = str[i + 2] - '0';
        blocks.insert(foo);
        
        maxlay = MAX(maxlay, foo->layer);
        minlay = MIN(minlay, foo->layer);
        maxrow = MAX(maxrow, foo->row);
        minrow = MIN(minrow, foo->row);
        mincol = MIN(mincol, foo->column);
        maxcol = MAX(maxcol, foo->column);
    }
    
    // attach adjacent tiles
    // assign each tile a placeholder index, shuffle later
    {
        auto layers = maxlay - minlay + 1;
        auto rows = maxrow - minrow + 1;
        auto columns = maxcol - mincol + 1;
        std::vector<mjBlock*> theBoard(layers * rows * columns, nullptr);
        
        int index = 0;
        for (auto foo : blocks) {
            foo->layer -= minlay;
            foo->row -= minrow;
            foo->column -= mincol;
            
            foo->tileIndex = index / 2 % TOTAL_MAHJONG_TILES;
            ++index;
            
            theBoard[columns * (foo->layer * rows + foo->row)  + foo->column] = foo;
        }
        
        
        auto getBlock = [&](int lay, int row, int col) -> mjBlock*
        {
            if (lay < 0 || lay >= layers || col < 0 || row < 0 || row >= rows || col >= columns) {
                return nullptr;
            }
            return theBoard[(rows *lay+row ) * columns + col];
        };
        
        // validate game data
        {
            if (blocks.size() % 2 != 0) {
                printf("bad game data: odd number of blocks.\n");
            }
            if (blocks.size() < 2) {
                printf("bad game data: too few blocks.\n");
            }
            for (auto blk : blocks) {
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        if (x==0 && y==0)continue;
                        auto foo = getBlock(blk->layer, blk->row + x, blk->column + y);
                        if (foo != nullptr) {
                            printf("bad game data: overlapping blocks.\n");
                        }
                    }
                }
            }
        }
        
        for (auto blk : blocks) {
            for (int i = -1; i <= 1; ++i) {
                auto foo = getBlock(blk->layer, blk->row + i, blk->column + 2);
                if (foo) {
                    blk->rights.insert(foo);
                    foo->lefts.insert(blk);
                }
            }
            if (blk->layer < layers - 1) {
                for (int x = -1; x <= 1; ++x) {
                    for (int y = -1; y <= 1; ++y) {
                        auto foo = getBlock(blk->layer + 1, blk->row + x, blk->column + y);
                        if (foo) {
                            blk->tops.insert(foo);
                            foo->downs.insert(blk);
                        }
                    }
                }
            }
        }
    }
    
    shuffle();
}

void mjGame::detachBlock(mjBlock *blk)
{
    for (auto foo : blk->lefts) {
        foo->rights.erase(blk);
    }
    for (auto foo : blk->rights) {
        foo->lefts.erase(blk);
    }
    for (auto foo : blk->downs) {
        foo->tops.erase(blk);
    }
    for (auto foo : blk->tops) {
        foo->downs.erase(blk);
    }
    blocks.erase(blk);
}

void mjGame::attachBlock(mjBlock *blk)
{
    for (auto foo : blk->lefts) {
        foo->rights.insert(blk);
    }
    for (auto foo : blk->rights) {
        foo->lefts.insert(blk);
    }
    for (auto foo : blk->downs) {
        foo->tops.insert(blk);
    }
    for (auto foo : blk->tops) {
        foo->downs.insert(blk);
    }
    blocks.insert(blk);
}

void mjGame::shuffle()
{
    std::vector<mjBlock*> sequence;
    
    std::function<bool()> findseq = [&]{
        if (blocks.empty()) {
            return true;
        }
        
        std::vector<mjBlock*> frees;
        for (auto foo : blocks) {
            if (foo->isFree()) {
                frees.push_back(foo);
            }
        }
        std::shuffle(frees.begin(), frees.end(), getRandomEngine());
        
        for (size_t i = 0; i < frees.size(); ++i) {
            detachBlock(frees[i]);
            sequence.push_back(frees[i]);
            
            for (size_t j = i + 1; j < frees.size(); ++j) {
                detachBlock(frees[j]);
                sequence.push_back(frees[j]);
                
                if (findseq()) {
                    return true;
                }
                
                attachBlock(frees[j]);
                sequence.pop_back();
            }
            
            attachBlock(frees[i]);
            sequence.pop_back();
        }
        
        return false;
    };
    
    auto result = findseq();
    
    for (auto foo : sequence) {
        attachBlock(foo);
    }
    
    if (result)
    {
        std::vector<int> backup;
        {
            std::vector<int> temp;
            for (auto foo : blocks) {
                temp.push_back(foo->tileIndex);
            }
            std::sort(temp.begin(), temp.end());
            for (size_t i=0; i< temp.size(); i+=2) {
                backup.push_back(temp[i]);
            }
        }
        std::shuffle(backup.begin(), backup.end(), getRandomEngine());
        
        for (size_t i = 0; i < sequence.size() / 2; ++i) {
            sequence[i * 2]->tileIndex = backup[i];
            sequence[i * 2 + 1]->tileIndex = backup[i];
        }
    }
    else {
        printf("bad game data: no solution.\n");
    }
}

std::vector<mjBlock*> mjGame::getTip()const{
    std::vector<mjBlock*> tips;
    // todo
    // if tips.size == 2, return matchable blocks
    // else fail to find 2 blocks with same tile, need to shuffle
    return tips;
}

int mjGame::onClick(mjBlock* blk, mjBlock* last){
    if (blk == nullptr) {
        return NONE;
    }
    
    if (false == blk->isFree()) {
        return FAIL;
    }
    
    if (last == nullptr) {
        return SELECT;
    }
    
    if (blk == last) {
        return NONE;
    }
    
    if (blk->tileIndex == last->tileIndex) {
        detachBlock(blk);
        detachBlock(last);
        delete blk;
        delete last;
        return MATCH;
    }
    
    return SELECT;
}

bool mjGame::isFail()const{
    int frees = 0;
    for (auto foo : blocks) {
        if (foo->isFree()) {
            ++frees;
        }
    }
    return frees < 2;
}
