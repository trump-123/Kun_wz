#ifndef _BESTPOSCALCULATE_H
#define _BESTPOSCALCULATE_H

#include <math.h>
#include <time.h>
#include <thread>
#include <mutex>
#include "GetBestUtils.h"
#include "ThreadPool.h"



class BestPosCalculate
{
public:
    BestPosCalculate();
    ~BestPosCalculate(void);

    //void initialize(const CVisionModule *);
    void BestPass(const CVisionModule *pVision,Player *players, CGeoPoint *ball, nType *result, int leader, double theirFlatresponseTime,bool isSim);
    //void calculateAllInterInfoV1(int start,int end,const CVisionModule *pVision,Player *players, CGeoPoint *ballPos, nType *bestPass, int leader, double theirFlatResponseTime,bool isSim,CGeoPoint &bestflatpos,int &flatpassnum,float &bestflatpassq,float &bestflatvel);
    void calculateAllInterInfoV1(int start,int end,int index,const CVisionModule *pVision,Player *players, CGeoPoint *ballPos, nType *bestPass, int leader, double theirFlatResponseTime,bool isSim);
    //void getBestInfo(const CVisionModule *pVision,nType *bestPass,int leader,CGeoPoint &bestflatpos,int &flatpassnum,float &bestflatpassq,float &bestflatvel);
    void initialize(const CVisionModule *pVision);
    //double calculateFlatVel(double distance, double interTime, bool isSim);
private:
    int len = 0;
    //thread t1;
   // thread t2;
   // thread t3;
   // thread t4;
    std::mutex mtx;
    ThreadPool pool;
    std::vector<std::future<void>>results;
    const CVisionModule* _pVision;
};
typedef Singleton<BestPosCalculate> ZBestPosCalculate;

#endif // BESTPOSCALCULATE_H
