//
//  game.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include "game.h"
#include "player.h"
#include "pokerAI.h"
#include <random>
#include <map>
#include <assert.h>

static string rankName[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "Jack", "Queen", "King", "Ace", };
static string suitName[] = {"Spade", "Heart", "Diamond", "Club", };

ostream& operator << (ostream& Out, const card& Card) {
    return Out << suitName[Card.Suit] << "_" << rankName[Card.Rank];
}

game::game() : N(9), gmCnt(0), lb(1E5), bb(2E5), deckSize(52), maxBet(0) { // 9 is the max # of players in a game
    
    for (int i = 0; i < 52; ++i) {
        Deck.push_back(new card(i / 13, i % 13));
    }
    
    for (int i = 0; i < N; ++i) {
        players.push_back(nullptr);
        fold.push_back(true);
        bank.push_back(2E8);
        mony.push_back(0);
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
    
    for (int i = 0; i < N; ++i) {
        if (!players[i]) {
            players[i] = p;
            players[i]->setId(i);
            return true;
        }
    }
    
    return false;
}


void game::rmvPlayer(player * p) {
    
    assert(p);
    
    size_t id = p->getId();
    delete p;
    players[id] = nullptr;
    
}

short game::getPlayerCnt() {
    
    short num = 0;
    
    for (int i = 0; i < N; ++i) {
        if (players[i])    ++num;
    }
    
    return num;
}


const card * game::pop() {
    assert(deckSize > 0);
    return Deck[--deckSize];
}


void game::addBoard() {
    assert(deckSize > 0);
    boardCards.push_back(Deck[--deckSize]);
}



void game::shuffleDeck() {
    random_device generator;
    shuffle(Deck.begin(), Deck.end(), default_random_engine(generator()));
}



void game::gameReset() {
    
    deckSize    =   52;
    maxBet      =   bb;

    boardCards.clear();
    shuffleDeck();
    
    for (short i = 0; i < N; ++i) {
        if (players[i])     players[i]->clrHand();
        fold[i] = true;
        mony[i] = 0;
    }
}


void game::startPlaying() {
    
    int n = getPlayerCnt();
    size_t finalWinner = -1;
    
    while (n >= 2) {
        
        gameReset();
        
        ++gmCnt;
        cout << "game:" << gmCnt << endl;
        
        map<size_t, player *>   mappedPlayers;
        map<size_t, size_t>     mappedId;
        
        size_t k = 0;
        for (int i = 0; i < N; ++i) {
            if (players[i]) {
                mappedPlayers[k] = players[i];
                mappedId[k] = players[i]->getId();
                fold[i] = false;
                k++;
            }
        }
        
        mappedPlayers[(gmCnt-1)%n]->bet(lb);                                        // gmCnt-1: little blind
        mappedPlayers[gmCnt%n]->bet(bb);
        
        for (int i = 0; i < 2*n; ++i) {
            mappedPlayers[(gmCnt-1+i)%n]->addHand();
        }
        
        short remRnd = 4;
        
        while (remRnd > 0) {
            short   remQry = n;
            int     offset = 1;
            
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
                
                size_t id = (gmCnt+offset)%n;
                
                if (allFold()) {
                    break;
                }
                
                if (mappedId[id] == 0) {
                    cout << "boardCards:";
                    for (auto i : getBoard()) {
                        cout << *i << " ";
                    }
                }
    
                if (!fold[mappedId[id]] && bank[mappedId[id]] != 0) {
                    query(mappedPlayers[id]);
                    if (mony[mappedId[id]] > maxBet) {
                        maxBet = mony[mappedId[id]];
                        remQry = n;
                    }
                }
                
                ++offset;
                --remQry;
            }
            
            if (allFold())    break;
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
            vector<const card *>  whc;                                        // win hand cards (2)
            vector<const card *>  wc;                                         // win cards (5)
            
            cout << "boardCards:";
            for (auto i : getBoard()) {
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
            
            for (int i = 0; i < n; ++i) {                               // initialize wc
                if (!fold[mappedId[i]]) {
                    whc = mappedPlayers[i]->getHand();
                    break;
                }
            }
            
            wc = pokerAI::findBig(pokerAI::combine(getBoard(), whc));
            
            for (int i = 0; i < n; ++i) {
                if (!fold[mappedId[i]]) {
                    cout << "[" << mappedId[i] << "]:";
                    for (auto j : mappedPlayers[i]->getHand()) {
                        cout << *j << " ";
                    }
                    cout << endl;
                    vector<const card *> tb = pokerAI::findBig(pokerAI::combine(getBoard(), mappedPlayers[i]->getHand()));
                    if (pokerAI::compare(tb, wc) > 0)       wc = tb;
                }
            }
            
            for (int i = 0; i < n; ++i) {
                if (!fold[mappedId[i]]) {
                    vector<const card *> tb = pokerAI::findBig(pokerAI::combine(getBoard(), mappedPlayers[i]->getHand()));
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
        for (auto i : mony) {
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
        
        n = getPlayerCnt();
        if  (allFold()) finalWinner = winners[0];
        else            finalWinner = mappedId[winners[0]];
    }
    
    cout << "*****" << endl;
    cout << "* " << finalWinner << " *" << endl;
    cout << "*****";
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



bool game::allFold() const {
    
    short   cnt = 0;
    
    for (int i = 0; i < N; ++i) {
        if (!fold[i]) {
            ++cnt;
        }
    }
    
    return (cnt == 1) ? true : false;
}



void game::query(player * p) {

    size_t id   =   p->getId();
    int myMony  =   mony[id];
    
    if (id == 0) {
        cout << endl << "handCards:";
        for (auto i : p->getHand()) {
            cout << *i << " ";
        }
        cout << "mony:";
        for (auto i : getMony()) {
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
    else if (d > 1 && d < 20)   {p->raise(d*maxBet - mony[id]); cout << "[" << id << "]raise  ";}
    else if (d >= 20)           {p->allin(); cout << "[" << id << "] allin  ";}

//    else if (d == 0)            {p->fold(); cout << "[" << id << "]fold  ";}
//    else if (d > 1 && d < 20)   {p->fold(); cout << "[" << id << "]fold  ";}
//    else if (d >= 20)           {p->fold(); cout << "[" << id << "]fold  ";}

    
}












