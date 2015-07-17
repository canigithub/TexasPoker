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

class game;


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
    game(short);
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
    int                 allFold() const;
    void                gameReset();
    void                query(player *);

};


#endif /* defined(__TexasPoker_revised__game__) */

















