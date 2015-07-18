//
//  player.h
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/18.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#ifndef __TexasPoker_revised__player__
#define __TexasPoker_revised__player__

#include <iostream>
#include <vector>

class card;
class game;

using namespace std;

class player {
    
private:
    game *              Game;
    vector<card *>      handCards;
    size_t              id;
    
public:
    player(game * g) : Game(g) {};
    ~player() {};
    void                addHand();
    void                clrHand() {handCards.clear();};
    vector<card *>      getHand() const {return handCards;};
    size_t              getId() const {return id;};
    void                setId(size_t s) {id = s;};
    
    void                bet(int m);
    void                fold();
    void                call();
    void                raise(int m);
    void                allin();
    
};

#endif /* defined(__TexasPoker_revised__player__) */