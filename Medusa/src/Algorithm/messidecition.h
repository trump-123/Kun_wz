#ifndef MESSIDECITION_H
#define MESSIDECITION_H
#include "VisionModule.h"
#include "singleton.h"
#include "WorldModel.h"

class CMessiDecision
{
public:
    CMessiDecision();
    void generateAttackDecision(const CVisionModule* pVision);
    int leaderNum() { return _leader; }
    int receiverNum() { return _receiver; }
    bool judgePassModule(const int playernum,CGeoPoint passpos);
    CGeoPoint passPos(){
        return  _passPos;
    }
    CGeoPoint otherPos(int index){
        return _otherPos[index];
    }
    CGeoPoint freeKickPos();
    //CGeoPoint freeKickWaitPos();
    CGeoPoint flatPassPos(){
        return _flatPassPos;
    }
    CGeoPoint leaderPos(){
        return _leaderPos;
    }
    CGeoPoint receiverPos(){
        return _receiverPos;
    }
    CGeoPoint firstChipPos(){
        return _firstChipPos;
    }
    CGeoPoint getBallPos(){
        return _ballPos;
    }
    CGeoPoint goaliePassPos();
    string nextState(){
        return _state;
    }

    bool isFlat() {
        return  _isFlat;
    }
    bool isfreeFlat() {
        return  _isfreeFlat;
    }
    bool needKick() {
        return _canKick;
    }
    bool needChip() {
        return !_isFlat;
    }
    bool isFlytime() {
        return _isFlyTime;
    }
    double passVel() {
        return _passVel;
    }
    double freepassVel() {
        return _freepassVel;
    }
    double firstChipDir(){
        return _firstChipDir;
    }

    void setisflyTime(bool isflyTime){
        _isFlyTime = isflyTime;
    }

    void setfirstChipDir(double firstchipdir){
         _firstChipDir = firstchipdir;
    }

    void setfirstChipPos(CGeoPoint firstchippos){
         _firstChipPos = firstchippos;
    }

    void setNormalPlay(bool isnormalPlay){
        _isNormalPlay = isnormalPlay;
    }

    void setBallPos(CGeoPoint ballPos){
        _ballPos = ballPos;
    }


private:
    int _leader;
    int _receiver;
    int _lastLeader;
    int _cycle;
    int _diff;
    int _stateChangeCycle;
    int _lastRecomputeCycle;
    int _lastChangeReceiverCycle;
    int _lastChangeLeaderCycle;
    int _lastUpdateRunPosCycle;
    int _lastUpdateReceiverPosCycle;
    int inValidPassCnt;
    int inValidShootCnt;
    int otherPosChangeCnt;
    double _passVel;
    double _freepassVel;
    double _firstChipDir;
    float _flatPassQ;
    float _flatShootQ;
    float _chipPassQ;
    float _chipShootQ;
    bool _isFlat;
    bool _isfreeFlat;
    bool _isbackFlat;
    bool _isFlyTime;
    bool _flydiff;
    bool _canKick;
    bool _canKickReceiver;
    bool _isNormalPlay;
    bool _isChange;
    bool _isPassC;
    string _state;
    string _laststate;
    string _leaderState;
    CGeoPoint _passPos;
    CGeoPoint _receiverPos;
    CGeoPoint _leaderPos;
    CGeoPoint _leaderWaitPos;
    CGeoPoint _otherPos[8];
    CGeoPoint _flatPassPos;
    CGeoPoint _flatShootPos;
    CGeoPoint _chipPassPos;
    CGeoPoint _chipShootPos;
    CGeoPoint _firstChipPos;
    CGeoPoint _ballPos;
    const CVisionModule* _pVision;

    void judgeState();
    void judgeLeaderState();
    void generateLeaderPos();
    void generateReceiverPos();
    void generateOtherPos();
    void confirmLeader();
    bool needReceivePos();
    bool needRunPos();
    void generateReceiverAndPos();
    void getPassPos();
    bool judcanuseBack(int robotNum);
    bool _saoActionSwitch = true;
};
typedef NormalSingleton<CMessiDecision> MessiDecision;
#endif // MESSIDECITION_H
