//
//  game.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include <iostream>
#include "game.h"
#include "player.h"
#include "pokerAI.h"
#include <random>
#include <assert.h>

static string rankName[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", };
static string suitName[] = {"S", "H", "D", "C", };

ostream& operator << (ostream& Out, const card& Card) {
    return Out << suitName[Card.Suit] << rankName[Card.Rank];
}

game::game() :
    maxNum(9), playerCnt(0), gmCnt(0), lb(2), bb(4),
    deckSize(52), maxBet(bb),
    players(maxNum, nullptr),
    fold(maxNum, true),
    bank(maxNum, 0),
    money(maxNum, 0) {
    for (int i = 0; i < 52; ++i) {
        Deck.push_back(new card(i / 13, i % 13));
    }
}


game::~game() {
    for (auto c : Deck) {
        delete c;
    }
    
    for (auto p : players) {
        delete p;
    }
}


bool game::addPlayer(player * p) {
    assert(p);
    
    for (int i = 0; i < maxNum; ++i) {
        if (!players[i]) {
            p->setId(i);
            bank[i] = p->getBank();
            players[i] = p;
            return ++playerCnt;
        }
    }
    return false;
}


void game::rmvPlayer(player * p) {
    assert(p);
    
    int id = p->getId();
    delete p;
    players[id] = nullptr;
    --playerCnt;
}

void game::shuffleDeck() {
    random_device generator;
    shuffle(Deck.begin(), Deck.end(), default_random_engine(generator()));
}

Cardptr game::pop() {
    assert(deckSize > 0);
    return Deck[--deckSize];
}


void game::addBoard() {
    assert(deckSize > 0);
    boardCards.push_back(Deck[--deckSize]);
}


void game::gameReset() {
    deckSize    =   52;
    maxBet      =   bb;
    boardCards.clear();
    shuffleDeck();
    
    for (int i = 0; i < maxNum; ++i) {
        if (players[i]) {players[i]->clrHand(); fold[i] = false;}
        else            fold[i] = true;
        money[i] = 0;
    }
}


int game::getNextPlayer(int i) const {
    assert(i >= 0 && i <= maxNum);
    i = (i == maxNum) ? 0 : i;
    int count = maxNum;
    while (count > 0) {
        if (!fold[i]) return i;
        i = (i+1 == maxNum) ? 0 : i+1;
        --count;
    }
    assert(false);
    return -1;
}

void game::startPlaying() {
    int finalWinner = -1;
    
    while (playerCnt >= 2) {
        gameReset();
        
        ++gmCnt;
        cout << "game:" << gmCnt << endl;
        
        int id = getNextPlayer((gmCnt-1)%playerCnt);
        int id2 = id;
        
        players[id]->bet(lb);
        id = getNextPlayer(++id);
        players[id]->bet(bb);
        id = getNextPlayer(++id);
        int id0 = id;                                              // each round of query starts from here
        
        for (int i = 0; i < 2*playerCnt; ++i) {
            players[id2]->addHand();
            id2 = getNextPlayer(++id2);
        }
        
        int remRnd = 4;
        while (remRnd > 0) {
            id = getNextPlayer(id0);
            int remQry = nonFoldCnt();
            cout << "Round " << 5-remRnd << ": ";
            while (remQry > 0) {
                if (nonFoldCnt() == 1) break;
                if (id == 0) {
                    cout << "boardCards:";
                    for (auto i : boardCards) {
                        cout << *i << " ";
                    }
                }
                if (!fold[id] && bank[id] != 0) {
                    query(players[id]);
                    if (money[id] > maxBet) {maxBet = money[id]; remQry = nonFoldCnt();}
                }
                id = getNextPlayer(++id);
                --remQry;
            }
            
            if (nonFoldCnt() == 1) break;
            cout << endl;
            switch (remRnd) {
                case 4:
                    pop(); pop(); pop(); addBoard(); addBoard(); addBoard(); break;
                case 3:
                    pop(); addBoard(); break;
                case 2:
                    pop(); addBoard(); break;
                case 1:
                    break;
                default:
                    assert(false);
            }
            --remRnd;
        }
        
        cout << "money:";
        for (auto i : money) {
            cout << " " << i;
        }
        cout << endl;
        
        vector<int> winners;
        if (nonFoldCnt() == 1) {
            cout << "all fold. ";
            for (int i = 0; i < maxNum; ++i) {
                if (!fold[i]) {winners.push_back(i); break;}
            }
        }
        else {
            vector<Cardptr>  whc;                                        // win hand cards (2)
            vector<Cardptr>  wc;                                         // win cards (5)
            cout << "boardCards:";
            for (auto i : boardCards) {
                cout << *i << " ";
            }
            cout << endl;
            
            for (int i = 0; i < maxNum; ++i) {                               // initialize wc
                if (!fold[i]) {whc = players[i]->getHand(); break;}
            }
            wc = pokerAI::findBig(pokerAI::combine(boardCards, whc));
            for (int i = 0; i < maxNum; ++i) {
                if (!fold[i]) {
                    cout << "[" << i << "]:";
                    for (auto j : players[i]->getHand()) {
                        cout << *j << " ";
                    }
                    cout << endl;
                    vector<Cardptr> tb = pokerAI::findBig(pokerAI::combine(boardCards, players[i]->getHand()));
                    if (pokerAI::compare(tb, wc) > 0) wc = tb;
                }
            }
            
            for (int i = 0; i < maxNum; ++i) {
                if (!fold[i]) {
                    vector<Cardptr> tb = pokerAI::findBig(pokerAI::combine(boardCards, players[i]->getHand()));
                    if (pokerAI::compare(tb, wc) == 0) winners.push_back(i);
                }
            }
        }
        
        cout << "the winner is:";
        for (auto i : winners) {
            cout << i << " ";
        }
        cout << endl;
        int totalMoney = 0;
        for (auto i : money) {
            totalMoney += i;
        }
        assert(winners.size() > 0);
        for (auto i : winners) {
            bank[i] += totalMoney/winners.size();
        }
        
        for (int i = 0; i < maxNum; ++i) {                     // print bank
            if (players[i]) cout << i << ":" << bank[i] << " ";
        }
        cout << endl << "*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*" << endl << endl;
        
        for (int i = 0; i < maxNum; ++i) {
            if (players[i] && bank[i] == 0) rmvPlayer(players[i]);
        }
        finalWinner = winners[0];
    }
    cout << "* * *" << endl;
    cout << "* " << finalWinner << " *" << endl;
    cout << "* * *";
}



void game::syncMoney(int i, int b, int m) {
    assert(i >= 0 && i < maxNum && b >= 0 && m >= 0);
    bank[i] = b;
    money[i] = m;
}

void game::syncFold(int i) {
    assert(i >= 0 && i < maxNum);
    fold[i] = true;
}

int game::nonFoldCnt() const {
    int cnt = 0;
    for (int i = 0; i < maxNum; ++i) {
        if (!fold[i]) ++cnt;
    }
    return cnt;
}

void game::query(player * p) {

    int id      =   p->getId();
    int myMony  =   money[id];
    
    if (id == 0) {
        cout << endl << "handCards:";
        for (auto i : p->getHand()) {
            cout << *i << " ";
        }
        cout << "mony:";
        for (auto i : getMoney()) {
            cout << i << " ";
        }
        cout << " bank:" << getBank()[id];
        cout << " >>>1-fold, 2-call, 3-raise:";
        int a, b;
        cin >> a;
        switch (a) {
            case 0: p->allin(); break;
            case 1: p->fold();  break;
            case 2: p->call();  break;
            case 3:
                cout << "enter $: ";
                cin >> b;
                while (b <= maxBet - myMony) {
                    cout << "invalid. re-enter $: ";
                    cin >> b;
                }
                p->raise(b); break;
            default:
                cout << "invalid input -> fold" << endl;
                p->fold(); break;
        }
        
        return;
    }
    
    
    int d = pokerAI::decision();
    
    if      (d < 0)             {p->fold(); cout << "[" << id << "]fold  ";}
    else if (d == 0)            {p->call(); cout << "[" << id << "]call  ";}
    else if (d > 1 && d < 20)   {p->raise(d*maxBet - money[id]); cout << "[" << id << "]raise  ";}
    else if (d >= 20)           {p->allin(); cout << "[" << id << "] allin  ";}
}









