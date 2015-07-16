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

vector<card *> pokerAI::combine(const vector<card *> & c1, const vector<card *> & c2) {
    
    vector<card *> allCards = c1;
    
    for (auto i : c2) {
        allCards.push_back(i);
    }
    
    return allCards;
}



bool pokerAI::isFlush(const vector<card *> & c) {
    
    assert(c.size() == 5);
    
    set<char> suits;
    
    for (int i = 0; i < 5; ++i) {
        suits.insert(c[i]->getSuit());
        if (suits.size() > 1)       return false;
    }
    
    return true;
}



vector<short> pokerAI::evaluate(const vector<card *> & c) {
    
    assert(c.size() == 5);
    
    vector<short>                           cardsType;
    set<short>                              set1, set2, set3;
    pair<set<short>::const_iterator, bool>  ret1, ret2;
    
    for (int i = 0; i < 5; ++i) {
        ret1 = set1.insert(c[i]->getRank());
        if (!ret1.second) {
            ret2 = set2.insert(c[i]->getRank());
            if (!ret2.second)       set3.insert(c[i]->getRank());
        }
    }
    
    switch (set1.size()) {
        case 5:
            if (*(set1.rbegin()) - *(set1.begin()) == 4) {
                if (isFlush(c)) {
                    cardsType.push_back(StraightFlush);
                    cardsType.push_back(*(set1.rbegin()));                                  // rank of the leading card
                }
                else {
                    cardsType.push_back(Straight);
                    cardsType.push_back(*(set1.rbegin()));
                }
            }
            else {
                if (isFlush(c)) {
                    cardsType.push_back(Flush);
                    for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                        cardsType.push_back(*rit);                                          // rank of each card
                    }
                }
                else {
                    cardsType.push_back(HighCard);
                    for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                        cardsType.push_back(*rit);
                    }
                }
            }
            break;
            
        case 4:
            cardsType.push_back(OnePair);
            cardsType.push_back(*(set2.begin()));                                           // rank of the pair
            for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                if (*(rit) != *(set2.begin()))      cardsType.push_back(*rit);              // rank of the rest cards
            }
            break;
            
        case 3:
            if (set2.size() == 1) {
                cardsType.push_back(ThreeKind);
                cardsType.push_back(*(set2.begin()));                                       // rank of the three
                for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*(rit) != *(set2.begin()))      cardsType.push_back(*rit);          // rank of the rest cards
                }
            }
            else if (set2.size() == 2) {
                cardsType.push_back(TwoPair);
                cardsType.push_back(*(set2.rbegin()));                                      // rank of the leading pair
                cardsType.push_back(*(set2.begin()));                                       // rank of the second pair
                for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*(rit) != *(set2.begin()) && *(rit) != *(set2.rbegin())) {
                        cardsType.push_back(*rit);                                          // rank of the single card
                    }
                }
            }
            else {
                assert(false);
            }
            break;
            
        case 2:
            if (set2.size() == 1) {
                cardsType.push_back(FourKind);
                cardsType.push_back(*(set2.begin()));                                       // rank of the four;
                for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*(rit) != *(set2.begin()))      cardsType.push_back(*rit);          // rank of the single card
                }
            }
            else if (set2.size() == 2) {
                cardsType.push_back(FullHouse);
                cardsType.push_back(*(set3.begin()));                                       // rank of the three
                for (set<short>::reverse_iterator rit = set1.rbegin(); rit != set1.rend(); ++rit) {
                    if (*(rit) != *(set3.begin()))      cardsType.push_back(*rit);          // rank of the pair
                }
            }
            else {
                assert(false);
            }
            break;
            
        default:
            assert(false);
    }
    
    return cardsType;
}



int pokerAI::compare(const vector<card *> & c1, const vector<card *> & c2) {
    
    assert(c1.size() == 5 && c2.size() == 5);
    
    vector<short> ev1 = evaluate(c1);
    vector<short> ev2 = evaluate(c2);
    if      (ev1[0] < ev2[0])       return -1;
    else if (ev1[0] > ev2[0])       return 1;
    else {
        for (size_t i = 1; i < ev1.size(); ++i) {
            if      (ev1[i] < ev2[i])       return -1;
            else if (ev1[i] > ev2[i])       return 1;
        }
        return 0;
    }
}


vector<card *> pokerAI::findBig(const vector<card *> & c) {
    
    assert(c.size() >= 5);
    
    vector<card *> big;
    
    big.insert(big.end(), c.begin(), c.begin()+5);              // initialize big.
    
    for (int i = 0; i < c.size(); ++i) {
        for (int j = i+1; j < c.size(); ++j) {
            for (int k = j+1; k < c.size(); ++k) {
                for (int m = k+1; m < c.size(); ++m) {
                    for (int n = m+1; n < c.size(); ++n) {
                        vector<card *> t;
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











