#pragma once

#include "ofMain.h"
#include "ofxEtherdream.h"
#include "ofxGamepadHandler.h"

#define heightScreen 0.45f

struct Box
{
  public :
  bool enabled = false;
  float pos[2];
  float scale[2];
  float rot;
};

struct SaveFile
{
  public :
  Box boxes[50];
  Box spawns[20];
  float screen_scale_x;
  float screen_scale_y;
  float screen_offset_x;
  float screen_offset_y;
  int PPS;
  int endCount;
  int blankCount;
  int targetPointCount;
  float spacing;
  bool flip_X;
  int pointLineCount;
  float rot;
  float trapezeX;
  float trapezeY;


};

struct polyColor
{
  ofPolyline polyObjSpace;
  ofPolyline polyWorldSpace;
  ofFloatColor color;
};


class Geometry
{
public :


  vector<polyColor> geometry;
  float rot;
  ofVec2f pos;
  ofVec2f scale;
  ofFloatColor color;

  float rot_mat[4];

  float lastBlinkTime = 0.0f;
  float BlinkDuration = 5.0f;

  float pointsPerUnit = 0;

  float input_EditPos[2];
  float input_EditScale[2];
  float input_EditRot;

  // Geometry()
  // {
  //   rot = 0.0f;
  //   scale = ofVec2f(1.0f,1.0f);
  //   UpdateMatrix();
  // }

  void Init(ofVec2f position, float rotation)
  {
    lastBlinkTime = -200.0f;

    input_EditPos[0] = 0.0f;
    input_EditScale[0] = 0.0f;
    input_EditPos[1] = 0.0f;
    input_EditScale[1] = 0.0f;
    input_EditRot = 0.0f;
    rot = rotation;
    pos = position;
    scale = ofVec2f(1.0f,1.0f);
    UpdateMatrix();
  }

  void Geo_ClearGeometry()
  {
    geometry.clear();
  }

  void Geo_SpaceShip(ofFloatColor geo_color)
  {
    color = geo_color;
    polyColor pc1 = polyColor();
    pc1.color = geo_color;
    pc1.polyObjSpace.addVertex(ofPoint(0.0f,0.015f));
    pc1.polyObjSpace.addVertex(ofPoint(0.01f,-0.015f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.01f,-0.015f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.0f,0.015f));
    geometry.push_back(pc1);

    polyColor pc2 = polyColor();
    pc2.color = pc2.color.fromHsb(fmod((geo_color.getHue()+0.1f),1.0), 1.0, 1.0, 1.0);
    pc2.polyObjSpace.addVertex(ofPoint(0.015f,-0.005f));
    pc2.polyObjSpace.addVertex(ofPoint(0.02f,-0.02f));
    geometry.push_back(pc2);

    polyColor pc3 = polyColor();
    pc3.color = pc2.color;
    pc3.color.setHueAngle(fmod((geo_color.getHueAngle()+45.0f),360.0));
    pc3.polyObjSpace.addVertex(ofPoint(-0.015f,-0.005f));
    pc3.polyObjSpace.addVertex(ofPoint(-0.02f,-0.02f));
    geometry.push_back(pc3);
  }

    void Geo_Cross(ofFloatColor geo_color)
    {
      color = geo_color;
      polyColor pc1 = polyColor();
      pc1.color = geo_color;
      pc1.polyObjSpace.addVertex(ofPoint(-0.015f,0.0f));
      pc1.polyObjSpace.addVertex(ofPoint(0.015f,-0.0f));
      geometry.push_back(pc1);

      polyColor pc2 = polyColor();
      pc2.color = geo_color;
      pc2.polyObjSpace.addVertex(ofPoint(-0.0f,0.015f));
      pc2.polyObjSpace.addVertex(ofPoint(0.0f,-0.015f));
      geometry.push_back(pc2);

    }

  void Geo_Square(ofFloatColor geo_color)
  {
    polyColor pc1 = polyColor();
    pc1.color = geo_color;
    pc1.polyObjSpace.addVertex(ofPoint(-0.1f,0.1f));
    pc1.polyObjSpace.addVertex(ofPoint(0.1f,0.1f));
    pc1.polyObjSpace.addVertex(ofPoint(0.1f,-0.1f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.1f,-0.1f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.1f,0.1f));
    geometry.push_back(pc1);
  }

  void UpdateMatrix()
  {
    float radAngleForward = ((rot+90.0f) / 180.0f) * 3.1415f;
    float radAngleRight = ((rot) / 180.0f) * 3.1415f;
    rot_mat[0] = cos(radAngleRight) * scale.x;
    rot_mat[1] = sin(radAngleRight) * scale.x;
    rot_mat[2] = cos(radAngleForward) * scale.y;
    rot_mat[3] = sin(radAngleForward) * scale.y;
  }

  void UpdateWorldSpaceGeometry()
  {
      for (int i=0; i<geometry.size(); i++)
      {
        geometry[i].polyWorldSpace.resize(geometry[i].polyObjSpace.size());
        geometry[i].color = geometry[i].color;

        for (int k=0; k<geometry[i].polyWorldSpace.size(); k++)
        {
          ofPoint worldS = ofPoint(0.0f,0.0f);
          ofPoint objS = geometry[i].polyObjSpace[k];
          //rotaion & scale
          worldS.x = objS.x * rot_mat[0];
          worldS.y = objS.x * rot_mat[1];
          worldS.x += objS.y * rot_mat[2];
          worldS.y += objS.y * rot_mat[3];
          //position
          worldS.x += pos.x;
          worldS.y += pos.y;

          //frame things
          // worldS -= ofPoint(0.5, 0.5);
          // worldS = worldS.x * ofVec2f(rotMat[0],rotMat[1]) + worldS.y * ofVec2f(rotMat[2],rotMat[3]); //rotate
          // float tmpx = worldS.x;
          // worldS.x *= 1.0f + trapezeX * worldS.y;
          // worldS.y *= 1.0f + trapezeY * tmpx;
          // worldS += ofPoint(0.5, 0.5);


          geometry[i].polyWorldSpace[k].set(worldS.x,worldS.y);
        }
      }
  }

  ofVec2f CheckCollision(ofVec2f pos, float radius)
  {
      float bestDistance = -100.0f;
      ofVec2f bestNormal = ofVec2f(0.0f,0.0f);

      for (int i=0; i<geometry.size(); i++)
      {
        vector<ofPoint> verts = geometry[i].polyWorldSpace.getVertices();
        for (int k=0; k<verts.size()-1; k++)
        {
          ofVec2f relativePos = pos - verts[k];
          ofVec2f lineDir = verts[k+1] - verts[k];
          ofVec2f normal = ofVec2f(-lineDir.y,lineDir.x);
          normal.normalize();
          float distance = normal.dot(relativePos);
          if (distance > bestDistance)
          {
            bestDistance = distance;
            bestNormal = normal;
          }
        }
      }

      // ofLog() << bestDistance;

      if (bestDistance < radius)
      {
            return bestNormal;
      }


      return ofVec2f(0.0f,0.0f);
  }

  void InputTransformation(float deltatime)
  {

      pos.x += input_EditPos[0] * deltatime;
      pos.y += input_EditPos[1] * deltatime;
      scale.x += input_EditScale[0] * deltatime;
      scale.y += input_EditScale[1] * deltatime;
      rot += input_EditRot * deltatime;
  }

  void draw(ofxIlda::Frame *ildaFrame, float fTime)
  {

    if (fTime > (lastBlinkTime + BlinkDuration) || (fmod(fTime, 0.1f) > 0.05f))
    {
      for (int i=0; i<geometry.size(); i++)
      {
        ofxIlda::Poly newPoly;
        newPoly.setFromPolyline(geometry[i].polyWorldSpace);
        newPoly.color = geometry[i].color;
        newPoly.pointsPerUnit = pointsPerUnit;

        ildaFrame->addPoly(newPoly);
      }
    }
  }

  //void draw()
};

class SpaceShip : public Geometry
{
  public :
    float acceleration;
    float rotSpeed;

    ofVec2f speed;
    float input_go;
    float input_reverse;
    float input_rotate;
    float fireCooldown;
    float lastTimeFire;

    float radius = 0.014f;



    // SpaceShip()
    // {
    //   fireCooldown = 0.45f;
    //   acceleration = 0.1f;
    //   rotSpeed = 100.0f;
    //   rot = 0.0f;
    //   scale = ofVec2f(1.0f,1.0f);
    //   UpdateMatrix();
    // }

    float GetRadius()
    {
        return radius * scale.x;
    }

    void Init(ofVec2f position, float rotation)
    {
      radius = 0.014f;
      BlinkDuration = 1.0f;
      fireCooldown = 0.45f;
      acceleration = 0.1f;
      rotSpeed = 170.0f;
      rot = rotation;
      pos = position;
      scale = ofVec2f(1.0f,1.0f);
      UpdateMatrix();
    }

    void Update(float deltaTime)
    {
        ofVec2f acc;
        acc.x = rot_mat[2];
        acc.y = rot_mat[3];

        if ((pos.x < 0.0f && speed.x < 0.0f) || (pos.x > 1.0f && speed.x > 0.0f))
          speed.x = -speed.x * 0.3f;
        if ((pos.y < (0.5f-heightScreen) && speed.y < 0.0f) || (pos.y > (0.5f+heightScreen) && speed.y > 0.0f) )
          speed.y = -speed.y * 0.3f;

        speed += acc * deltaTime * (input_go-input_reverse) * acceleration;

        speed -= acceleration * deltaTime * (speed/0.4);

        pos += speed * deltaTime;
        rot += rotSpeed * deltaTime * input_rotate;

        scale.x += deltaTime * 0.033f;
        scale.y += deltaTime * 0.033f;
    }

    bool Fire(float fireTime)
    {
        if (fireTime > (lastTimeFire + fireCooldown))
        {
            lastTimeFire = fireTime;
            return true;
        }
        else
        {
          return false;
        }
    }
};

class Bonus : public Geometry
{
  public :
  int type;
  float lifetime = 1.5f;
  float bornTime;
  bool isDead = false;


  void Init(ofVec2f position, float rotation, int atype, float currentTime)
  {
      type = atype;
      pos = position;
      rot = rotation;
      scale = ofVec2f(1.0f,1.0f);
      UpdateMatrix();
      bornTime = currentTime;
      lifetime = 8.0f;
  }

  void Geo_LSD()
  {
    color = ofFloatColor(1.0,1.0,0.5,1.0);
    polyColor pc1 = polyColor();
    pc1.color = ofFloatColor(1.0,1.0,0.5,1.0);
    pc1.polyObjSpace.addVertex(ofPoint(-0.02f,0.02f));
    pc1.polyObjSpace.addVertex(ofPoint(0.02f,0.02f));
    pc1.polyObjSpace.addVertex(ofPoint(0.02f,-0.02f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.02f,-0.02f));
    pc1.polyObjSpace.addVertex(ofPoint(-0.02f,0.02f));
    geometry.push_back(pc1);

  }


  int Update(float deltaTime, float Ftime, vector<SpaceShip>& ships)
  {
    int result = -1;
    for (int i=0; i<ships.size();i++)
    {
      if ((ships[i].pos-pos).length() < ships[i].GetRadius() * 2.0)
      {
        result = i;
        isDead = true;
      }
    }
    if (Ftime > (lifetime + bornTime))
    {
        isDead = true;
    }
    return result;
  }
};


class Projectile : public Geometry
{
  public :
    float speed = 0.5f;
    float lifetime = 1.5f;
    float bornTime;
    bool isDead = false;

    // Projectile()
    // {
    //   bornTime = (ofGetElapsedTimeMillis()) * 0.001f;
    //   // lifetime = 5.0f;
    //   speed = 0.5f;
    //   rot = 0.0f;
    //   scale = ofVec2f(1.0f,1.0f);
    //   UpdateMatrix();
    // }

    void Init(ofVec2f position, float rotation, ofFloatColor lasercolor)
    {
      isDead = false;
      lifetime = 1.5f;
      bornTime = (ofGetElapsedTimeMillis()) * 0.001f;
      // lifetime = 5.0f;
      speed = 0.5f;
      polyColor pc1 = polyColor();
      pc1.color = lasercolor;
      pc1.polyObjSpace.addVertex(ofPoint(-0.0f,-0.0f));
      pc1.polyObjSpace.addVertex(ofPoint(-0.0f,0.02f));
      geometry.push_back(pc1);

      rot = rotation;
      pos = position;
      scale = ofVec2f(1.0f,1.0f);
      UpdateMatrix();
    }

    void Update(float deltaTime, float Ftime, vector<SpaceShip>& ships, vector<Geometry>& spawns, int &spawnCounter)
    {
      for (int i=0; i<ships.size();i++)
      {
        if ((ships[i].pos-pos).length() < ships[i].GetRadius())
        {
          if (Ftime > ships[i].lastBlinkTime + ships[i].BlinkDuration)
          {
            if (spawns.size() > 0)
            {
              ships[i].pos = spawns[(spawnCounter++)%spawns.size()].pos;
            }
            else
            {
              ships[i].pos = ofVec2f(ofRandom(0.2f, 0.8f),ofRandom(0.5f - heightScreen * 0.8f, 0.5f + heightScreen * 0.8f));
            }
            ships[i].rot = ofRandom(0.0f, 360.0f);
            ships[i].speed = ofVec2f(0.0f,0.0f);
            ships[i].BlinkDuration = 1.0;
            ships[i].lastBlinkTime = Ftime;
            ships[i].scale.x = 1.0f;
            ships[i].scale.y = 1.0f;
          }
          isDead = true;
        }
      }


      pos += deltaTime * speed * ofVec2f(rot_mat[2],rot_mat[3]);


      if ((pos.x < 0.0f && rot_mat[2] < 0.0f) || (pos.x > 1.0f && rot_mat[2] > 0.0f))
        rot_mat[2] = -rot_mat[2];
      if ((pos.y < (0.5f-heightScreen) && rot_mat[3] < 0.0f) || (pos.y > (0.5f+heightScreen) && rot_mat[3] > 0.0f) )
        rot_mat[3] = -rot_mat[3];

      // if (pos.x < 0.0f || pos.x > 1.0f ||
      //   pos.y < 0.0f || pos.y > 1.0f)
      //   {
      //         isDead = true;
      //   }

      if (Ftime > (lifetime + bornTime))
      {
          isDead = true;
      }
    }

};


class testApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseMoved(int x, int y);


    void axisChanged(ofxGamepadAxisEvent& e);
    void buttonPressed(ofxGamepadButtonEvent& e);
    void buttonReleased(ofxGamepadButtonEvent& e);

    void ConfigFromFileData();
    void GeometryFromFileData();
    void SaveToFile();

    void CheckEditSelected(int newID);
    void ChangeEditMode();

    void NewSpawn();
    void NewBox();
    void RemoveItem();

    vector<Geometry> all_geometry;
    vector<SpaceShip> all_spaceship;
    vector<Projectile> all_projectile;
    vector<Bonus> all_bonus;

    vector<Geometry> all_boxgeometry;
    vector<Geometry> all_spawngeometry;

    ofxIlda::Frame ildaFrame;   // stores and manages ILDA frame drawings
    float lastFrameTime;

    SaveFile fileData;
    int editMode; // 0: none 1: boxes 2:spawns
    int editSelectedItem;

    float deltaTime;
    int spawnCounter;

    float lastTimeWin;
    float winDuration;
    int winner;

    float rotMat[4];

    float rot;
    float trapezeX;
    float trapezeY;

    float LSDStartTime;
    float LSDDuration;

    float bonusLastTime;
    float bonusDuration;

    int currentBonusType;
    int bonusTypeCount;

    ofxEtherdream etherdream;   // interface to the etherdream device
};
