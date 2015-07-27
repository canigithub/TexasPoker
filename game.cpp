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
#include <map>
#include <assert.h>

static string rankName[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A", };
static string suitName[] = {"S", "H", "D", "C", };

ostream& operator << (ostream& Out, const card& Card) {
    return Out << suitName[Card.Suit] << rankName[Card.Rank];
}

game::game() :
    maxNum(9), playerCnt(0), gmCnt(0), lb(1E5), bb(2E5),
    deckSize(52), maxBet(bb),
    players(maxNum, nullptr),
    fold(maxNum, true),
    bank(maxNum, 2e8),
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
            players[i] = p;
            players[i]->setId(i);
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
    
    for (short i = 0; i < maxNum; ++i) {
        if (players[i])     players[i]->clrHand();
        fold[i] = true;
        money[i] = 0;
    }
}


void game::startPlaying() {
    int finalWinner = -1;
    
    while (playerCnt >= 2) {
        gameReset();
        
        ++gmCnt;
        cout << "game:" << gmCnt << endl;
        
        map<int, player *>   mappedPlayers;
        map<int, int>     mappedId;
        
        int k = 0;
        for (int i = 0; i < maxNum; ++i) {
            if (players[i]) {
                mappedPlayers[k] = players[i];
                mappedId[k] = players[i]->getId();
                fold[i] = false;
                k++;
            }
        }
        
        mappedPlayers[(gmCnt-1) % playerCnt]->bet(lb);                                        // gmCnt-1: little blind
        mappedPlayers[gmCnt % playerCnt]->bet(bb);
        
        for (int i = 0; i < 2 * playerCnt; ++i) {
            mappedPlayers[(gmCnt-1+i) % playerCnt]->addHand();
        }
        
        short remRnd = 4;
        
        while (remRnd > 0) {
            short   remQry = playerCnt;
            int     offset = 1;
            
            cout << "Round " << 5-remRnd << ": ";
            
            while (remQry > 0) {
                
                int id = (gmCnt+offset) % playerCnt;
                
                if (allFold()) {
                    break;
                }
                
                if (mappedId[id] == 0) {
                    cout << "boardCards:";
                    for (auto i : boardCards) {
                        cout << *i << " ";
                    }
                }
    
                if (!fold[mappedId[id]] && bank[mappedId[id]] != 0) {
                    query(mappedPlayers[id]);
                    if (money[mappedId[id]] > maxBet) {
                        maxBet = money[mappedId[id]];
                        remQry = playerCnt;
                    }
                }
                
                ++offset;
                --remQry;
            }
            
            if (allFold())    break;
            
            cout << endl << "Round " << 5 - remRnd << " ends." << endl;
            
            switch (remRnd) {
                case 4:
                    pop();      pop();      pop();
                    addBoard(); addBoard(); addBoard();
                    break;
                case 3:
                    pop();      addBoard();
                    break;
                case 2:
                    pop();      addBoard();
                    break;
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
        
        if (allFold()) {
            cout << "all fold. ";
            for (int i = 0; i < fold.size(); ++i) {
                if (!fold[i]) {
                    winners.push_back(i);
                    break;
                }
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
            
            //
            
            for (int i = 0; i < mappedId.size(); ++i) {
                cout << "[" << mappedId[i] << "]" << ":";
                for (auto j : mappedPlayers[i]->getHand()) {
                    cout << *j << " ";
                }
            }
            cout << endl;
            
            //
            
            for (int i = 0; i < playerCnt; ++i) {                               // initialize wc
                if (!fold[mappedId[i]]) {
                    whc = mappedPlayers[i]->getHand();
                    break;
                }
            }
            
            wc = pokerAI::findBig(pokerAI::combine(boardCards, whc));
            
            for (int i = 0; i < playerCnt; ++i) {
                if (!fold[mappedId[i]]) {
                    cout << "[" << mappedId[i] << "]:";
                    for (auto j : mappedPlayers[i]->getHand()) {
                        cout << *j << " ";
                    }
                    cout << endl;
                    vector<Cardptr> tb = pokerAI::findBig(pokerAI::combine(boardCards, mappedPlayers[i]->getHand()));
                    if (pokerAI::compare(tb, wc) > 0)       wc = tb;
                }
            }
            
            for (int i = 0; i < playerCnt; ++i) {
                if (!fold[mappedId[i]]) {
                    vector<Cardptr> tb = pokerAI::findBig(pokerAI::combine(boardCards, mappedPlayers[i]->getHand()));
                    if (pokerAI::compare(tb, wc) == 0)      winners.push_back(i);
                }
            }
        }
        
        cout << "the winner is:";
        if (allFold()) {
            cout << winners[0];
        }
        else {
            for (auto i : winners) {
                cout << mappedId[i] << " ";
            }
        }
        cout << endl;
        
        int totalMoney = 0;
        for (auto i : money) {
            totalMoney += i;
        }
        
        assert(winners.size() > 0);
        
        if (allFold()) {
            bank[winners[0]] += totalMoney;
        }
        else {
            for (auto i : winners) {
                bank[mappedId[i]] += totalMoney/winners.size();
            }
        }
        
        
        for (int i = 0; i < mappedId.size(); ++i) {
            cout << mappedId[i] << ":" << bank[mappedId[i]] << " ";
        }
        
        cout << endl << "******************************" << endl << endl;
        
        for (int i = 0; i < mappedId.size(); ++i) {
            if (bank[mappedId[i]] == 0) {
                rmvPlayer(mappedPlayers[i]);
            }
        }

        if  (allFold()) finalWinner = winners[0];
        else            finalWinner = mappedId[winners[0]];
    }
    
    cout << "* * *" << endl;
    cout << "* " << finalWinner << " *" << endl;
    cout << "* * *";
}



void game::syncMoney(size_t i, int b, int m) {
    assert(i >= 0 && i < maxNum && b >= 0 && m >= 0);
    bank[i] = b;
    money[i] = m;
}



void game::syncFold(size_t i) {
    assert(i >= 0 && i < maxNum);
    fold[i] = true;
}



bool game::allFold() const {
    
    short   cnt = 0;
    
    for (int i = 0; i < maxNum; ++i) {
        if (!fold[i]) {
            ++cnt;
        }
    }
    
    return (cnt == 1) ? true : false;
}



void game::query(player * p) {

    size_t id   =   p->getId();
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
        cout << " >>1-fold, 2-call, 3-raise:";
        int a, b;
        cin >> a;
        switch (a) {
            case 0:
                p->allin(); break;
                
            case 1:
                p->fold();  break;
                
            case 2:
                p->call();  break;
                
            case 3:
                cout << "enter $: ";
                cin >> b;
                while (b <= maxBet - myMony) {
                    cout << "invalid. re-enter $: ";
                    cin >> b;
                }
                p->raise(b);
                break;
                
            default:
                cout << "invalid input -> fold" << endl;
                p->fold();
                break;
        }
        
        return;
    }
    
    
    int d = pokerAI::decision();
    
    if      (d < 0)             {p->fold(); cout << "[" << id << "]fold  ";}
    else if (d == 0)            {p->call(); cout << "[" << id << "]call  ";}
    else if (d > 1 && d < 20)   {p->raise(d*maxBet - money[id]); cout << "[" << id << "]raise  ";}
    else if (d >= 20)           {p->allin(); cout << "[" << id << "] allin  ";}
}









