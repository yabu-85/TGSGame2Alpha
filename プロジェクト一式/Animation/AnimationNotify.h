#pragma once
#include <vector>
#include <string>

class GameObject;
enum AUDIO_TYPE;
enum VFX_TYPE;

class OneFrame {
protected:
    int time;
public:
    OneFrame(int t) : time(t) {};
    virtual void CalcFrame(int t, GameObject* obj = nullptr);
    virtual void OnFrame(GameObject* obj) {};
};

class DoubleFrame : public OneFrame {
    int endTime;
public:
    DoubleFrame(int first, int end) : OneFrame(first), endTime(end) {};
    void CalcFrame(int t, GameObject* obj = nullptr) override;
    virtual void OnEnter(GameObject* obj) {};
    virtual void OnExit(GameObject* obj) {};
};

class CreatFrame : public OneFrame {
protected:
    VFX_TYPE type;
public:
    CreatFrame(int t, VFX_TYPE _type) : OneFrame(t), type(_type) {};
    void CalcFrame(int t, GameObject* obj = nullptr) override;
};

class CreatFrameBone : public CreatFrame {
    int hModel;
    int boneIndex;
    int partIndex;
public:
    CreatFrameBone(int t, VFX_TYPE _type, int model, std::string name);
    void CalcFrame(int t, GameObject* obj = nullptr) override;
};

class PlaySoundFrame : public OneFrame{
    AUDIO_TYPE type;
public:
    PlaySoundFrame(int t, AUDIO_TYPE _type) : OneFrame(t), type(_type) {};
    void CalcFrame(int t, GameObject* obj = nullptr) override;
};
