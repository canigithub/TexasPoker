//
//  player.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/18.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include "player.h"
#include "game.h"
#include <assert.h>

void player::addHand() {
    handCards.push_back(Game->pop());
}



void player::bet(int m) {
    
    assert(m >= 0);
    int myBank  =   Game->getBank()[id];
    int myMony  =   Game->getMoney()[id];
    
    if (m > myBank) {
        cout << "m > myBank." << endl;
        m = myBank;
    }
    
    Game->syncMoney(id, myBank-m, myMony+m);
}



void player::fold() {
    Game->syncFold(id);
}



void player::call() {
    int max     =   Game->getMax();
    int myMony  =   Game->getMoney()[id];
    bet(max-myMony);
}



void player::raise(int m) {
    bet(m);
}



void player::allin() {
    
    int myBank      =   Game->getBank()[id];
    int myMony      =   Game->getMoney()[id];
    Game->syncMoney(id, 0, myMony+myBank);
}
