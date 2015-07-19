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

inline int add(int a, int b) {
    return a + b;
}

template <class T, class R>
bool func( T t, R r) {
    return t < r;
}

int main(int argc, const char * argv[]) {
    
    game * g = new game();
    for (int i = 0; i < 5; ++i) {
        g->addPlayer(new player(g));
    }
    
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
