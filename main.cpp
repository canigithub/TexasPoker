//
//  main.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include <iostream>
#include "game.h"
#include "player.h"
#include "pokerAI.h"
#include <map>

int main(int argc, const char * argv[]) {

    
    game * g = new game();
    player * p0 = new player(g);
    player * p1 = new player(g);
    player * p2 = new player(g);
    player * p3 = new player(g);
    player * p4 = new player(g);
    
    g->addPlayer(p0);
    g->addPlayer(p1);
    g->addPlayer(p2);
    g->addPlayer(p3);
    g->addPlayer(p4);
    
    g->startPlaying();
    
    
//    map<int, string> mapString;
//    
//    mapString[0] = "first";
//    mapString[1] = "second";
//    mapString[2] = "third";
//    
//    for (int i = 0; i < 6; ++i) {
//        cout << mapString[i] << endl;
//    }
//    return 0;
}
