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
            else if (clicks >= 99999){
                printf("too many tries.\n");
                return;
            }
        }
    }
    
    void run(){
        
        dorun("02602403201202004404603801802:004006135115123127");
        dorun("000002004104");
        dorun("000100200300"); // will fail
        dorun("000100200300302"); //  fail, or too many tries
        dorun("000100200300301");// overlap

        dorun("00902402902>03103703;03A04404904>06006206406606806:06<06>06@06B08408908>09109709;09A0:40:90:>0<910912412912>13113713;13A14414914>16016216416616816:16<16>16@16B18418918>19119719;19A1:41:91:>1<920922422922>23123723;23A24424924>26026226426626826:26<26>26@26B28428928>29129729;29A2:42:92:>2<9");
    }
}
