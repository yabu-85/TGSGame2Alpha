#pragma once

class GameObject;

class BaseAction {
protected:
    GameObject* pCharacter_;

public:
    BaseAction(GameObject* obj);
    virtual ~BaseAction() {};
    virtual void Update() = 0;
    virtual void Initialize() {}
    virtual void Terminate() {}
};