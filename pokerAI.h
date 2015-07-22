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

#include "common.h"

using namespace std;

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
    static vector<Cardptr>     combine(const vector<Cardptr> &, const vector<Cardptr> &);
    static int                      compare(const vector<Cardptr> &, const vector<Cardptr> &);
    static int                      evaluate(const vector<Cardptr> &);
    static bool                     isFlush(const vector<Cardptr> &);
    static vector<Cardptr>     findBig(const vector<Cardptr> &);
    static int                      decision();

};

#endif /* defined(__TexasPoker_revised__pokerAI__) */
