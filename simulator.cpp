//
//  simulator.cpp
//  mjlogic


#include "simulator.hpp"
#include "mahjong.hpp"

namespace test {
    void dorun(const std::string& str){
        mjGame game;
        game.parse(str);
        
        int clicks = 0;
        mjBlock* last = nullptr;
        while (true) {
            auto it = game.blocks.begin();
            auto rand = std::rand() % game.blocks.size();
            for (auto i = 0; i < rand; ++i) {
                ++it;
            }
            
            mjBlock * blk = *it;
            auto ret = game.onClick(blk, last);
            switch (ret) {
                case SELECT:
                    last = blk;
                    break;
                case MATCH:
                    last = nullptr;
                    break;
                default:
                    break;
            }
            
            ++clicks;
            if (game.isWin()) {
                printf("win: steps = %d.\n", clicks);
                return;
            }
            else if (game.isFail()) {
                printf("fail.\n");
                return;
            }
        }
    }
    
    void run(){
        
        dorun("02602403201202004404603801802:004006135115123127");
        dorun("000002004104");
        dorun("000100200300"); // will fail

        dorun("9<2>:29:24:2A92;92792192>82982482B62@62>62<62:62862662462262062>42942442A32;32732132>229224229029<1>:19:14:1A91;91791191>81981481B61@61>61<61:61861661461261061>41941441A31;31731131>219214219019<0>:09:04:0A90;90790190>80980480B60@60>60<60:60860660460260060>40940440A30;30730130>20920420900");
    }
}
