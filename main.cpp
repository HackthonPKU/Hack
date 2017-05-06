//
//  main.cpp
//  leapmotion
//
//  Created by Leo Zhang on 17/5/5.
//  Copyright (c) 2017年 Leo Zhang. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <pthread.h>
#include <cmath>
#include "Leap.h"
using namespace Leap;
using namespace std;

bool flag = false;

bool hit = false;

int64_t zeroTime = -1;

int64_t pauseCount = 0;

int64_t prevPause = -1;

Frame current;

pthread_mutex_t mutex_k;

Vector dest[4];

float score;

class SampleListener : public Listener {
public:
    virtual void onConnect(const Controller&);
    virtual void onFrame(const Controller&);
};



void SampleListener::onConnect(const Controller& controller) {
    std::cout << "Connected" << std::endl;
    controller.enableGesture(Gesture::TYPE_SWIPE);
}
bool zeroSpeed(const Hand hand)
{
    const Vector speed = hand.palmVelocity();
    return abs(speed.y) > 5;
}
void PrintHandInformation(const Hand hand)
{
    cout << "X:" << hand.palmPosition().x <<endl;
    cout << "Y:" << hand.palmPosition().y <<endl;
    cout << "Z:" << hand.palmPosition().z <<endl;
    cout << "speedX:" << hand.palmVelocity().x <<endl;
    cout << "speedY:" << hand.palmVelocity().y <<endl;
    cout << "speedZ:" << hand.palmVelocity().z <<endl;
}

int scoreUpdate(Frame now,int64_t pauseCount,int64_t timeLenth)
{
    const HandList hands = now.hands();
    
    for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
    {
        const Hand hand = *hl;
        Vector palm = hand.palmPosition();
        Vector goal = dest[pauseCount%4];
        float distanceX = palm.x - goal.x;
        float distanceY = palm.y - goal.y;
        if (sqrt(distanceX*distanceX + distanceY*distanceY) < 100)
        {
            score += ((float)1000000)/((float)(1000000-timeLenth));
            cout << "Score is :" << score <<endl;
            hit = true;
        }
    }
    
    
    return score;
}

bool onArc(Vector pos)
{
    return true;
}

void SampleListener::onFrame(const Controller& controller) {
    const Frame frame = controller.frame();
    pthread_mutex_lock(&mutex_k);
    current = frame;
    pthread_mutex_unlock(&mutex_k);
    /*std::cout << "Frame id: " << frame.id()
    << ", timestamp: " << frame.timestamp()
    << ", hands: " << frame.hands().count()
    << ", fingers: " << frame.fingers().count()
    << ", tools: " << frame.tools().count()
    << ", gestures: " << frame.gestures().count() << std::endl;*/
    if (frame.hands().count()>1)
        return ;
    HandList hands = frame.hands();
    if (flag && frame.timestamp() - prevPause >= 500000)
    {
        pthread_mutex_lock(&mutex_k);
        Frame mycur = current;
        pthread_mutex_unlock(&mutex_k);
        int64_t mytime = mycur.timestamp();
        pthread_mutex_lock(&mutex_k);
        prevPause = mytime;
        pauseCount ++;
        hit = false;
        pthread_mutex_unlock(&mutex_k);
        if (pauseCount%4 == 0)
            cout << "down" <<endl;
        else if (pauseCount%4 == 1)
            cout << "left" <<endl;
        else if (pauseCount%4 == 2)
            cout << "right" <<endl;
        else
            cout << "up" <<endl;
    }
    for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl)
    {
        const Hand hand = *hl;
        //bool flag = zeroSpeed(hand);
        if (!onArc(hand.palmPosition()))
        {
            score --;
        }
        if (!hit)
            scoreUpdate(frame, pauseCount,frame.timestamp()-prevPause);
        /*for (int i = 1; i < 2; i++)
        {
            const Frame prev = controller.frame(i);
            HandList prev_hands = prev.hands();
            for (HandList::const_iterator hp = prev_hands.begin(); hp != prev_hands.end(); ++hp)
            {
                const Hand prev_hand = *hp;
                if (prev_hand.isLeft()!=hand.isLeft())
                    continue;
                if (prev_hand.palmVelocity().y * hand.palmVelocity().y > 0)
                    flag = false;
            }
        }*/
        //const Hand hand = *hl;
        //HandList::const_iterator hp = hand_prev.begin();
        //const Hand flag = *hp;
        //if (flag.isLeft()!=hand.isLeft())
        //{
         //   hp++;
        //}
        //const Hand prev_hand = *hp;*/
        //PrintHandInformation(hand);
        //if (flag)
           //PrintHandInformation(hand);
    }
}



void synchroTime(int64_t time0)
{
    zeroTime = time0;
}

int main(int argc, char** argv) {
    SampleListener listener;
    Controller controller;
    
    
    controller.addListener(listener);
    
    // Keep this process running until Enter is pressed
    //std::cout << "Press Enter to quit..." << std::endl;
    
    std::cin.get();
    pthread_mutex_lock(&mutex_k);
    dest[0].x = 0;
    dest[1].x = 0;
    dest[2].x = -100;
    dest[3].x = 100;
    dest[0].y = 300;
    dest[1].y = 25;
    dest[2].y = 75;
    dest[3].y = 75;
    pauseCount = 3;
    prevPause = current.timestamp() + 50000;
    pthread_mutex_unlock(&mutex_k);
    flag = true;
    
    string quit;
    while(1)
    {
        if (score > 100)
          break;
    }
    ///while (1)
    //{
        //pthread_mutex_lock(&mutex_k);
        //Frame mycur = current;
        //pthread_mutex_unlock(&mutex_k);
        //int64_t mytime = mycur.timestamp();
        
       // if (mytime - prevPause < 1000000)
            //continue;
       // prevPause = mytime;
        //pauseCount ++;
        //scoreUpdate(mycur,pauseCount);
        //if (score > 20)
          //  break;
        //if (pauseCount%4 == 0)
          //  cout << "down" <<endl;
        //else if (pauseCount%4 == 1)
          //  cout << "left" <<endl;
        //else if (pauseCount%4 == 2)
          //  cout << "right" <<endl;
        //else
          //  cout << "up" <<endl;
        //cin>>quit;
        //if (quit == "QQ")
            //break;
   // }
    
    
    
    
    
    // Remove the sample listener when done
    controller.removeListener(listener);
    
    return 0;
}
