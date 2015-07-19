//
//  game.h
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#ifndef __TexasPoker_revised__game__
#define __TexasPoker_revised__game__

#include <iostream>
#include <vector>

using namespace std;

class player;

struct card {
    card(char suit, char rank) : Suit(suit), Rank(rank) {}
    const char Suit;
    const char Rank;
    friend ostream& operator << (ostream& Out, const card& Card);
};

class game {
    
private:
    vector<const card *>      Deck;
    vector<const card *>      boardCards;
    const int           maxNum;
    int                 deckSize;
    int                 playerCnt;
    int                 gmCnt;
    int                 lb;
    int                 bb;
    int                 maxBet;
    vector<player *>    players;
    vector<bool>        fold;
    vector<int>         bank;
    vector<int>         money;
    
    
public:
    game();
    ~game();
    
    void                shuffleDeck();
    const card *        pop();
    void                addBoard();
    vector<int>         getBank() const {return bank;};
    vector<int>         getMoney() const {return money;};
    int                 getMax() const {return maxBet;};
    void                startPlaying();
    void                syncMoney(size_t, int, int);
    void                syncFold(size_t);
    bool                allFold() const;
    void                gameReset();
    void                query(player *);
    
    bool                addPlayer(player *);
    void                rmvPlayer(player *);
};


#endif /* defined(__TexasPoker_revised__game__) */

















