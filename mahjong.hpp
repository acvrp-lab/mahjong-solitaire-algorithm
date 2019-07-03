//
//  mahjong.hpp
//  mjlogic
//


#ifndef mahjong_hpp
#define mahjong_hpp

#include <set>
#include <vector>
#include <string>

enum {
    NONE,
    FAIL,
    SELECT,
    MATCH,
    
    TOTAL_MAHJONG_TILES = 99,
};

struct mjBlock {
    // for convenience, a block occupies 2 rows and 2 columns in this project
    int layer, row, column;
    int tileIndex;
    
    // in usual game, a block can be selected when no block is atop of it,
    // and no block is blocking at least one side
    std::set<mjBlock*> lefts, rights, tops, downs;
    bool isFree()const{return tops.empty() && (rights.empty() || lefts.empty());}
};

class mjGame {
    
    std::set<mjBlock*> blocks;

    void attachBlock(mjBlock*);
    void detachBlock(mjBlock*);
    
public:
    ~mjGame();

    void parse(const std::string& str);
    
    void shuffle();
    
    std::vector<mjBlock*> getTip()const;
    
    int onClick(mjBlock* blk, mjBlock* last);

    bool isWin()const{return blocks.empty();}
    bool isFail()const;
};

#endif /* mahjong_hpp */
