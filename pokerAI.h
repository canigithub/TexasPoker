//
//  pokerAI.h
//  TexasPoker_revised
//
//  Created by Nengyun Zhang on 15/7/10.
//  Copyright (c) 2015年 Nengyun Zhang. All rights reserved.
//

#ifndef __TexasPoker_revised__pokerAI__
#define __TexasPoker_revised__pokerAI__

#include <iostream>
#include <vector>

using namespace std;

class card;

class pokerAI {
    
public:
    enum evalType {
        
        HighCard,
        OnePair,
        TwoPair,
        ThreeKind,
        Straight,
        Flush,
        FullHouse,
        FourKind,
        StraightFlush,
    };
    
    pokerAI() {};
    ~pokerAI() {};
    static vector<card *>   combine(const vector<card *> &, const vector<card *> &);
    static int              compare(const vector<card *> &, const vector<card *> &);
    static vector<short>    evaluate(const vector<card *> &);
    static bool             isFlush(const vector<card *> &);
    static vector<card *>   findBig(const vector<card *> &);
    static int              decision();

};

#endif /* defined(__TexasPoker_revised__pokerAI__) */
