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

class card {
    
private:
    char                suit;
    short               rank;
    
public:
    card(char c, short s) : suit(c), rank(s) {};
    ~card() {};
    char                getSuit() {return suit;};
    short               getRank() {return rank;};
};


class game {
    
private:
    vector<card *>      Deck;
    vector<player *>    players;
    vector<card *>      boardCards;
    short               N;
    short               deckSize;
    int                 gmCnt;
    int                 lb;
    int                 bb;
    int                 maxBet;
    vector<bool>        fold;
    vector<int>         bank;
    vector<int>         mony;
    
    
public:
    game();
    ~game();
    
    void                shuffleDeck();
    card *              pop();
    void                addBoard();
    vector<card *>      getBoard() const {return boardCards;};
    vector<int>         getBank() const {return bank;};
    vector<int>         getMony() const {return mony;};
    int                 getMax() const {return maxBet;};
    void                startPlaying();
    void                syncMoney(size_t, int, int);
    void                syncFold(size_t);
    bool                allFold() const;
    void                gameReset();
    void                query(player *);
    
    bool                addPlayer(player *);
    void                rmvPlayer(player *);
    short               getPlayerCnt();

};


#endif /* defined(__TexasPoker_revised__game__) */

















