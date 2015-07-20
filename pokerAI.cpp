//
//  pokerAI.cpp
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#include "pokerAI.h"
#include "game.h" 
#include <set>
#include <chrono>
#include <stdlib.h>
#include <assert.h>
#include <random>

vector<const card *> pokerAI::combine(const vector<const card *> & c1, const vector<const card *> & c2) {
    vector<const card *> allCards = c1;
    allCards.insert(allCards.end(), c2.begin(), c2.end());
    return allCards;
}


bool pokerAI::isFlush(const vector<const card *> & c) {
    assert(c.size() == 5);
    
    set<char> suits;
    
    for (int i = 0; i < 5; ++i) {
        suits.insert(c[i]->Suit);
        if (suits.size() > 1)   return false;
    }
    return true;
}


int pokerAI::evaluate(const vector<const card *> & c) {
    
    assert(c.size() == 5);
    
    int                                    power = 0;
    int                                    shift = 20;
    set<char>                              set1, set2, set3;
    pair<set<char>::const_iterator, bool>  ret1, ret2;
    
    for (int i = 0; i < 5; ++i) {
        ret1 = set1.insert(c[i]->Rank);
        if (!ret1.second) {
            ret2 = set2.insert(c[i]->Rank);
            if (!ret2.second)       set3.insert(c[i]->Rank);
        }
    }
    
    switch (set1.size()) {                                                              // T:20, 1: 16, 2:12, 3:8; 4:4, 5:0
        case 5:
            if (*(set1.rbegin()) - *(set1.begin()) == 4) {
                if (isFlush(c)) {
                    power = (StraightFlush << 20) | (*(set1.rbegin()) << (shift -= 4));
                }
                else {
                    power = (Straight << 20) | (*(set1.rbegin()) << (shift -= 4));
                }
            }
            else {
                if (isFlush(c)) {
                    power = (Flush << 20);
                    for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
                else {
                    // high card - no need to shift Type
                    for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
            }
            break;
            
        case 4:
            power = (OnePair << 20) | (*(set2.begin()) << (shift -= 4));
            for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                if (*rit != *(set2.begin())) {
                    power = power | (*rit << (shift -= 4));
                }
            }
            break;
            
        case 3:
            if (set2.size() == 1) {
                power = (ThreeKind << 20) | (*(set2.begin()) << (shift -= 4));
                for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*rit != *(set2.begin())) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
            }
            else if (set2.size() == 2) {
                power = (TwoPair << 20) | (*(set2.rbegin()) << (shift -= 4));
                power = power | (*(set2.begin()) << (shift -= 4));
                for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*rit != *(set2.begin()) && *rit != *(set2.rbegin())) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
            }
            else {
                assert(false);
            }
            break;
            
        case 2:
            if (set2.size() == 1) {
                power = (FourKind << 20) | (*(set2.begin()) << (shift -= 4));
                for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*rit != *(set2.begin())) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
            }
            else if (set2.size() == 2) {
                power = (FullHouse << 20) | (*(set3.begin()) << (shift -= 4));
                for (set<char>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*rit != *(set3.begin())) {
                        power = power | (*rit << (shift -= 4));
                    }
                }
            }
            else {
                assert(false);
            }
            break;
            
        default:
            assert(false);
    }
    
    return power;
}



int pokerAI::compare(const vector<const card *> & c1, const vector<const card *> & c2) {
    assert(c1.size() == 5 && c2.size() == 5);
    int power1 = evaluate(c1);
    int power2 = evaluate(c2);
    if      (power1 > power2)   return 1;
    else if (power1 < power2)   return -1;
    else                        return 0;
}


vector<const card *> pokerAI::findBig(const vector<const card *> & c) {
    assert(c.size() >= 5);
    
    vector<const card *> big;
    big.insert(big.end(), c.begin(), c.begin()+5);              // initialize big.
    for (int i = 0; i < c.size(); ++i) {
        for (int j = i+1; j < c.size(); ++j) {
            for (int k = j+1; k < c.size(); ++k) {
                for (int m = k+1; m < c.size(); ++m) {
                    for (int n = m+1; n < c.size(); ++n) {
                        vector<const card *> t;
                        t.push_back(c[i]);
                        t.push_back(c[j]);
                        t.push_back(c[k]);
                        t.push_back(c[m]);
                        t.push_back(c[n]);
                        if (compare(t, big) > 0)    big = t;
                    }
                }
            }
        }
    }

    return big;
}


int pokerAI::decision() {
    
    random_device generator;
    uniform_int_distribution<int> distribution(0, 9);
    switch (distribution(generator)) {
        case 0:
            return -1;      // fold
        case 1:
            return 2;       // raise
        default:
            return 0;       // call
    }
}











