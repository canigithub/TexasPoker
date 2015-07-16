//
//  game.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include "game.h"
#include "pokerAI.h"
#include <random>
#include <chrono>
#include <assert.h>

game::game(short s) {
    
    N           =   s;
    gmCnt       =   0;
    lb          =   1;
    bb          =   2;
    deckSize    =   52;
    maxBet      =   0;
    
    for (int i = 0; i < 52; ++i) {
        if (i/13 == 0) {
            card * newCard = new card('s',i%13);
            Deck.push_back(newCard);
        }
        else if (i/13 == 1) {
            card * newCard = new card('h',i%13);
            Deck.push_back(newCard);
        }
        else if (i/13 == 2) {
            card * newCard = new card('d',i%13);
            Deck.push_back(newCard);
        }
        else if (i/13 == 3) {
            card * newCard = new card('c',i%13);
            Deck.push_back(newCard);
        }
    }
    
    for (int i = 0; i < N; ++i) {
        players.push_back(new player(this));
        fold.push_back(false);
        bank.push_back(2E8);
        mony.push_back(0);
    }
    
    for (int i = 0; i < N; ++i) {
        players[i]->setId(i);
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



card * game::pop() {
    assert(deckSize > 0);
    return Deck[--deckSize];
}


void game::addBoard() {
    assert(deckSize > 0);
    boardCards.push_back(Deck[--deckSize]);
}



void game::shuffleDeck() {
    unsigned seed = (unsigned) chrono::system_clock::now().time_since_epoch().count();
    shuffle(Deck.begin(), Deck.end(), default_random_engine(seed));
}



void game::gameReset() {
    
    deckSize    =   52;
    maxBet      =   bb;

    boardCards.clear();
    shuffleDeck();
    
    for (short i = 0; i < N; ++i) {
        players[i]->clrHand();
        fold[i] = false;
        mony[i] = 0;
    }
}


void game::startPlaying() {

    gmCnt   =   0;
    
    while (true) {
        ++gmCnt;
        cout << "game:" << gmCnt << endl;
        
        gameReset();
        
        players[(gmCnt-1)%N]->bet(lb);                                  // gmCnt-1: little blind
        players[gmCnt%N]->bet(bb);
        
        
        for (int i = 0; i < N*2; ++i) {
            players[(gmCnt-1+i)%N]->addHand();                          // start dealing from lb
        }
        
        short   remRnd      =   4;
        
        while (remRnd > 0) {
            short   remQry      =   N;
            int     offset      =   1;
            
            switch (remRnd) {
                case 4:
                    cout << "1st round: ";   break;
                case 3:
                    cout << "2nd round: ";   break;
                case 2:
                    cout << "3rd round: ";   break;
                case 1:
                    cout << "final round: "; break;
                default:
                    assert(false);
            }
            
            while (remQry > 0) {
                if (allFold() >= 0)     break;
                
                size_t id = (gmCnt+offset)%N;
                
                if (id == 0) {
                    cout << "boardCards:";
                    for (auto i : getBoard()) {
                        cout << i->getSuit() << i->getRank() << " ";
                    }
                }
                
                if (!fold[id]) {
                    query(players[id]);
                    if (mony[id] > maxBet) {
                        maxBet = mony[id];
                        remQry = N;
                    }
                }
                
                ++offset;
                --remQry;
            }
            
            if (allFold() >= 0)    break;
            
            switch (remRnd) {
                case 4:
                    cout << "1st round ends." << endl;
                    pop();      pop();      pop();
                    addBoard(); addBoard(); addBoard();
                    break;
                case 3:
                    cout << "2nd round ends." << endl;
                    pop();      addBoard();
                    break;
                case 2:
                    cout << "3rd round ends." << endl;
                    pop();      addBoard();
                    break;
                case 1:
                    cout << "final round ends." << endl;
                    break;
                default:
                    assert(false);
            }
            
            --remRnd;
        }
        
        cout << "mony:";
        for (auto i : mony) {
            cout << " " << i;
        }
        cout << endl;
        
        vector<size_t> winners;
        
        if (allFold() >= 0) {
            cout << "all fold. ";
            winners.push_back(allFold());
        }
        else {
            vector<card *>  whc;                                        // win hand cards (2)
            vector<card *>  wc;                                         // win cards (5)
            
            cout << "boardCards:";
            for (auto i : getBoard()) {
                cout << i->getSuit() << i->getRank() << " ";
            }
            cout << endl;
            
            for (int i = 0; i < N; ++i) {                               // initialize wc
                if (!fold[i]) {
                    whc = players[i]->getHand();
                    break;
                }
            }
            
            wc = pokerAI::findBig(pokerAI::combine(getBoard(), whc));
            
            for (int i = 0; i < N; ++i) {
                if (!fold[i]) {
                    cout << "[" << i << "]:";
                    for (auto j : players[i]->getHand()) {
                        cout << j->getSuit() << j->getRank() << " ";
                    }
                    cout << endl;
                    vector<card *> tb = pokerAI::findBig(pokerAI::combine(getBoard(), players[i]->getHand()));
                    if (pokerAI::compare(tb, wc) > 0)       wc = tb;
                }
            }
            
            
            for (int i = 0; i < N; ++i) {
                if (!fold[i]) {
                    vector<card *> tb = pokerAI::findBig(pokerAI::combine(getBoard(), players[i]->getHand()));
                    if (pokerAI::compare(tb, wc) == 0)      winners.push_back(i);
                }
            }
        }
        
        cout << "the winner is:";
        for (auto i : winners) {
            cout << i << " ";
        }
        cout << endl;
        
        int totalMoney = 0;
        for (auto i : mony) {
            totalMoney += i;
        }
        
        assert(winners.size() > 0);
        
        for (auto i : winners) {
            bank[i] += totalMoney/winners.size();
        }
        
        cout << endl << "******************************" << endl << endl;
    }
}



void game::syncMoney(size_t i, int b, int m) {
    assert(i >= 0 && i < N && b >= 0 && m >= 0);
    bank[i] = b;
    mony[i] = m;
}



void game::syncFold(size_t i) {
    assert(i >= 0 && i < N);
    fold[i] = true;
}



int game::allFold() const {
    
    short   cnt = 0;
    int     ret = -1;
    
    for (int i = 0; i < N; ++i) {
        if (!fold[i]) {
            ++cnt;
            ret = i;
        }
    }
    
    ret = (cnt == 1) ? ret : -1;
    return ret;
}



void game::query(player * p) {

    size_t id   =   p->getId();
    int myMony  =   mony[id];
    
    if (id == 0) {
        cout << endl << "handCards:";
        for (auto i : p->getHand()) {
            cout << i->getSuit() << i->getRank() << " ";
        }
        cout << "mony:";
        for (auto i : getMony()) {
            cout << i << " ";
        }
        cout << " bank:" << getBank()[id];
        cout << " ::1-fold, 2-call, 3-raise:";
        int a, b;
        cin >> a;
        switch (a) {
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
    else if (d > 1 && d < 20)   {p->raise(d*maxBet - mony[id]); cout << "[" << id << "]raise  ";}
    else if (d >= 20)           {p->allin(); cout << "[" << id << "] allin  ";}
    
}



void player::addHand() {
    handCards.push_back(Game->pop());
}



void player::bet(int m) {

    assert(m >= 0);
    int myBank  =   Game->getBank()[id];
    int myMony  =   Game->getMony()[id];
    
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
    int myMony  =   Game->getMony()[id];
    bet(max-myMony);
}



void player::raise(int m) {
    bet(m);
}



void player::allin() {
    
    int myBank      =   Game->getBank()[id];
    int myMony      =   Game->getMony()[id];
    Game->syncMoney(id, 0, myMony+myBank);
}









