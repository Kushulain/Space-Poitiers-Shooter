#include "testApp.h"
#include "ofxIldaFrame.h"

float intensity = 1.0f;
int lastMouseY = 0;

#define PPU 100.0f

//--------------------------------------------------------------
void testApp::setup(){
    ofBackground(0);

    rotMat[0] = 1.0f;
    rotMat[1] = 0.0f;
    rotMat[2] = 0.0f;
    rotMat[3] = 1.0f;
    trapezeX = 0.0f;
    trapezeY = 0.0f;

    bonusTypeCount = 2;
    currentBonusType = 0;

    bonusDuration = 20.0;
    bonusLastTime = 0.0;

    LSDDuration = 20.0;
    LSDStartTime = -200.0;
    lastTimeWin = -100.0f;
    winDuration = 5.0f;
    winner = -1;

    ofLog(OF_LOG_NOTICE, "expected size %d", sizeof(fileData));
    ofBuffer dataBuffer;
    ofFile file;
    if (file.open("data.dat", ofFile::ReadWrite, true))
    {
      dataBuffer = file.readToBuffer();
      ofLog(OF_LOG_NOTICE, "reading from file");
      ofLog(OF_LOG_NOTICE, "size %d", dataBuffer.size());
      ofLog(OF_LOG_NOTICE, "expected size %d", sizeof(fileData));
      //all_geometry.resize(dataBuffer.size() / sizeof(Geometry));
      memcpy(&fileData,dataBuffer.getData(),dataBuffer.size());


      ofLog(OF_LOG_NOTICE, "set config from file");
      ConfigFromFileData();
      // etherdream.setPPS(fileData.PPS);

      ofLog(OF_LOG_NOTICE, "set geometry from file");
      GeometryFromFileData();

      ofLog(OF_LOG_NOTICE, "done");
    }
    else
    {
      fileData.blankCount = 15;
      fileData.endCount = 15;
      fileData.flip_X = false;
      fileData.screen_scale_x = 1.0f;
      fileData.screen_scale_y = 1.0f;
      fileData.screen_offset_x = 0.0f;
      fileData.screen_offset_y = 0.0f;
      fileData.PPS = 35000;
      fileData.targetPointCount = 0;
      fileData.spacing = 0.01;
      fileData.pointLineCount = 5;
      fileData.rot = 0.0;
      fileData.trapezeX = 0.0;
      fileData.trapezeY = 0.0;

      ConfigFromFileData();

    }


    etherdream.etherDreamIP = "192.168.1.4";
    etherdream.setup(true,2);
    etherdream.setPPS(40000);


    Geometry carre;
    carre.Init(ofVec2f(0.5f,0.5f),0.0f);
    carre.Geo_Square(ofFloatColor(1.0f,1.0f,0.8f,1.0f));
    carre.scale.x = 4.99f;
    carre.scale.y = 4.99f * heightScreen * 2.0f;
    carre.pointsPerUnit = PPU;
    all_geometry.push_back(carre);

//    ofBuffer dataBuffer;

    // dataBuffer.set((char*)all_geometry.data(), (sizeof(Geometry) * all_geometry.size()));
    // bool fileWritten = ofBufferToFile("data.dat", dataBuffer);
    // ofLog(OF_LOG_NOTICE, "wrote to file %d", fileWritten);
    // ofLog(OF_LOG_NOTICE, "size %d", dataBuffer.size());

	ofxGamepadHandler::get()->enableHotplug();

  ofLog(OF_LOG_NOTICE, "gamepads %d", ofxGamepadHandler::get()->getNumPads());


  ildaFrame.params.output.doCapX = true;
  ildaFrame.params.output.doCapY = true;
  // SpaceShip myship = SpaceShip(ofVec2f(ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f)),0.0f);
  // myship.Geo_SpaceShip(ofFloatColor(ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),1.0f));
  // all_spaceship.push_back(myship);


      // if (all_spaceship[0].Fire(ofGetElapsedTimeMillis() * 0.001f))
      // {
      //
      //   Projectile proj = Projectile(all_spaceship[0].pos + ofVec2f(all_spaceship[0].rot_mat[2],all_spaceship[0].rot_mat[3]) * 0.05,
      //   all_spaceship[0].rot,
      //   all_spaceship[0].color);
      //   //myship.Geo_SpaceShip(ofFloatColor(ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),1.0f));
      //   all_projectile.push_back(proj);
      // }

  for (int i=0; i<ofxGamepadHandler::get()->getNumPads(); i++)
  {
    ofLog(OF_LOG_NOTICE, "new SPACESHIP %d", ofxGamepadHandler::get()->getNumPads());


    ofVec2f bonPos = ofVec2f(ofRandom(0.2f, 0.8f),ofRandom(0.5f - heightScreen * 0.8f, 0.5f + heightScreen * 0.8f));
    if (all_spawngeometry.size() > 0)
    {
      bonPos = all_spawngeometry[(spawnCounter++)%all_spawngeometry.size()].pos;
    }

    SpaceShip myship;
    myship.Init(ofVec2f(ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f)),0.0f);
    myship.Geo_SpaceShip(ofFloatColor(ofRandom(0.5f, 0.8f),ofRandom(0.5f, 0.8f),ofRandom(0.5f, 0.8f),1.0f));
    all_spaceship.push_back(myship);

		ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(i);
    ofAddListener(pad->onAxisChanged, this, &testApp::axisChanged);
    ofAddListener(pad->onButtonPressed, this, &testApp::buttonPressed);
    ofAddListener(pad->onButtonReleased, this, &testApp::buttonReleased);
  }
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	// if(ofxGamepadHandler::get()->getNumPads()>0){
	// 		ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
	// 		ofAddListener(pad->onAxisChanged, this, &testApp::axisChanged);
	// 		ofAddListener(pad->onButtonPressed, this, &testApp::buttonPressed);
	// 		ofAddListener(pad->onButtonReleased, this, &testApp::buttonReleased);
	// }
}

void testApp::ConfigFromFileData()
{
        ildaFrame.params.output.endCount = fileData.endCount;
        ildaFrame.params.output.blankCount = fileData.blankCount;
        ildaFrame.polyProcessor.params.targetPointCount = fileData.targetPointCount;
        ildaFrame.polyProcessor.params.spacing = fileData.spacing;
        ildaFrame.params.output.transform.doFlipX = fileData.flip_X;
        // ildaFrame.params.output.transform.offset.y = fileData.screen_offset_y ;
        // ildaFrame.params.output.transform.offset.x = fileData.screen_offset_x;
        // ildaFrame.params.output.transform.scale.y = fileData.screen_scale_y;
        // ildaFrame.params.output.transform.scale.x = fileData.screen_scale_x;

        ildaFrame.params.output.pointLineCount = fileData.pointLineCount;
        // ildaFrame.rot = fileData.rot;
        // ildaFrame.trapezeX = fileData.trapezeX;
        // ildaFrame.trapezeY = fileData.trapezeY;
}

void testApp::GeometryFromFileData()
{
    all_boxgeometry.clear();
    all_spawngeometry.clear();

    for (int i=0; i<50; i++)
    {
      if (fileData.boxes[i].enabled)
      {
          Geometry carre;
          carre.Init(ofVec2f(fileData.boxes[i].pos[0],fileData.boxes[i].pos[1]),fileData.boxes[i].rot);
          carre.Geo_Square(ofFloatColor(1.0f,1.0f,0.8f,1.0f));
          carre.scale.x = fileData.boxes[i].scale[0];
          carre.scale.y = fileData.boxes[i].scale[1];
          carre.UpdateMatrix();
          carre.pointsPerUnit = PPU;
          carre.UpdateWorldSpaceGeometry();
          all_boxgeometry.push_back(carre);
      }
    }

    for (int i=0; i<20; i++)
    {
      if (fileData.spawns[i].enabled)
      {
          Geometry carre;
          carre.Init(ofVec2f(fileData.spawns[i].pos[0],fileData.spawns[i].pos[1]),fileData.spawns[i].rot);
          carre.Geo_Cross(ofFloatColor(1.0f,1.0f,0.0f,1.0f));
          carre.scale.x = fileData.spawns[i].scale[0];
          carre.scale.y = fileData.spawns[i].scale[1];

          //carre.pointsPerUnit = PPU;
          carre.UpdateMatrix();
          carre.UpdateWorldSpaceGeometry();
          all_spawngeometry.push_back(carre);
      }
    }
}


void testApp::CheckEditSelected(int newID)
{

    if (editMode == 1)
    {
      if (newID < 0)
        newID = all_boxgeometry.size()-1;
      if (all_boxgeometry.size() == 0)
        newID = 0;
      else if (newID >= all_boxgeometry.size())
        newID = 0;

      if (all_boxgeometry.size() > 0 && editSelectedItem <= all_boxgeometry.size())
      {
        all_boxgeometry[editSelectedItem].lastBlinkTime = -200.0f;
      }
    }
    else if (editMode == 2)
    {
      if (newID < 0)
        newID = all_spawngeometry.size()-1;
      if (all_spawngeometry.size() == 0)
        newID = 0;
      else if (newID >= all_spawngeometry.size())
        newID = 0;

      if (all_spawngeometry.size() > 0 && editSelectedItem <= all_spawngeometry.size())
      {
        all_spawngeometry[editSelectedItem].lastBlinkTime = -200.0f;
      }
    }

    editSelectedItem = newID;
          ofLog(OF_LOG_NOTICE, "CheckEditSelected %d", newID);
}

//--------------------------------------------------------------
void testApp::update()
{
  float currentTime = (ofGetElapsedTimeMillis()) * 0.001f ;
  deltaTime = currentTime - lastFrameTime;


  if (currentTime > bonusLastTime + bonusDuration)
  {
    bonusLastTime = currentTime;


    ofVec2f bonPos = ofVec2f(ofRandom(0.2f, 0.8f),ofRandom(0.5f - heightScreen * 0.8f, 0.5f + heightScreen * 0.8f));
    if (all_spawngeometry.size() > 0)
    {
      bonPos = all_spawngeometry[rand()%all_spawngeometry.size()].pos;
    }

    int newType = (currentBonusType++)%bonusTypeCount;

    Bonus carre;
    carre.Init(bonPos,0.0,newType,currentTime);
    if (newType == 0)
    {
      carre.Geo_LSD();
    }
    else
    {
      carre.Geo_Cross(ofFloatColor(1.0f,0.0f,0.0f,1.0f));
    }

    carre.UpdateMatrix();
    carre.UpdateWorldSpaceGeometry();
    all_bonus.push_back(carre);
  }

  if (currentTime > lastTimeWin + winDuration && winner != -1)
  {
    winner = -1;

    for (int i=0; i<all_spaceship.size();i++)
    {
      all_spaceship[i].scale.x = 1.0f;
      all_spaceship[i].scale.y = 1.0f;
    }
  }

  if (editMode == 1)
  {
      if (all_boxgeometry.size() > 0 && editSelectedItem <= all_boxgeometry.size())
      {
        all_boxgeometry[editSelectedItem].lastBlinkTime = currentTime;
      }
  }
  else if (editMode == 2)
  {
      if (all_spawngeometry.size() > 0 && editSelectedItem <= all_spawngeometry.size())
      {
        all_spawngeometry[editSelectedItem].lastBlinkTime = currentTime;
      }
  }

  for (int i=all_projectile.size()-1; i>=0;i--)
  {
        if (all_projectile[i].isDead)
          all_projectile.erase(all_projectile.begin()+i);
  }
  for (int i=all_bonus.size()-1; i>=0;i--)
  {
        if (all_bonus[i].isDead)
          all_bonus.erase(all_bonus.begin()+i);
  }

  if (editMode == 0)
  {
    for (int i=0; i<all_spaceship.size();i++)
    {
      for (int k=0; k<all_boxgeometry.size(); k++)
      {
        ofVec2f normal = all_boxgeometry[k].CheckCollision(all_spaceship[i].pos,all_spaceship[i].GetRadius());

        if ((normal.x != 0.0f || normal.y != 0.0f) && normal.dot(all_spaceship[i].speed) < 0.0f)
        {
          all_spaceship[i].speed = (all_spaceship[i].speed - 2.0f * normal * normal.dot(all_spaceship[i].speed));
          float brakeCollision = normal.dot(all_spaceship[i].speed) * 0.7;
          all_spaceship[i].speed -= normal * brakeCollision;
        }

      }
      all_spaceship[i].Update(deltaTime);
      all_spaceship[i].UpdateMatrix();
      all_spaceship[i].UpdateWorldSpaceGeometry();

      if (i == winner)
      {
        all_spaceship[i].lastBlinkTime = currentTime;
      }

      if (all_spaceship[i].scale.x > 2.0 && currentTime > lastTimeWin + winDuration && winner == -1)
      {
        winner = i;
        lastTimeWin = currentTime;
      }
    }

    for (int i=0; i<all_bonus.size();i++)
    {

        int results = all_bonus[i].Update(deltaTime,currentTime,all_spaceship);

        if (results != -1)
        {
          if (all_bonus[i].type == 0)
          {
            LSDStartTime = currentTime;
          }
          else
          {
            all_spaceship[results].BlinkDuration = 10.0;
              all_spaceship[results].lastBlinkTime = currentTime;
          }
        }
    }

    for (int i=0; i<all_projectile.size();i++)
    {
      for (int k=0; k<all_boxgeometry.size(); k++)
      {
        ofVec2f normal = all_boxgeometry[k].CheckCollision(all_projectile[i].pos, 0.01f);
        ofVec2f speed = ofVec2f(all_projectile[i].rot_mat[2],all_projectile[i].rot_mat[3]);
        // speed *= all_projectile[i].speed;
        if ((normal.x != 0.0f || normal.y != 0.0f) && normal.dot(speed) < 0.0f)
        {

          speed = (speed - 2.0f * normal * normal.dot(speed));
          all_projectile[i].rot_mat[2] = speed.x;
          all_projectile[i].rot_mat[3] = speed.y;
        }

      }
          all_projectile[i].Update(deltaTime,currentTime,all_spaceship,all_spawngeometry,spawnCounter);
          //all_projectile[i].UpdateMatrix();
          all_projectile[i].UpdateWorldSpaceGeometry();
    }
  }

  lastFrameTime = (ofGetElapsedTimeMillis()) * 0.001f;
}



//--------------------------------------------------------------
void testApp::draw() {


    ildaFrame.clear();
    float currentTime = (ofGetElapsedTimeMillis() * 0.001f);

    float LSDweight = 0.0;

    if (currentTime < LSDStartTime + LSDDuration)
    {
        if (currentTime-LSDStartTime < 1.0)
        {
          LSDweight = currentTime-LSDStartTime;
        }
        else if (currentTime-LSDStartTime > LSDDuration - 1.0)
        {
          LSDweight = abs(currentTime - (LSDDuration + LSDStartTime));
        }
        else
        {
          LSDweight = 1.0;
        }
        trapezeX = sin(currentTime* 0.4) * 2.0;
        trapezeY = sin(currentTime * 0.6f) * 2.0 ;
        rot = fmod(360.0 * (currentTime-LSDStartTime) / LSDDuration,360.0);
    }


    ildaFrame.rot = fileData.rot + rot;
    ildaFrame.trapezeX = fileData.trapezeX + LSDweight * trapezeX;
    ildaFrame.trapezeY = fileData.trapezeY + LSDweight * trapezeY;

    ildaFrame.params.output.transform.offset.y = fileData.screen_offset_y - LSDweight * 0.1;
    ildaFrame.params.output.transform.offset.x = fileData.screen_offset_x;
    ildaFrame.params.output.transform.scale.y = fileData.screen_scale_y * (1.0-LSDweight * 0.2);
    ildaFrame.params.output.transform.scale.x = fileData.screen_scale_x * (1.0-LSDweight * 0.2);

    //ofLog(OF_LOG_NOTICE, "time %fd", (ofGetElapsedTimeMillis()) * 0.001f);

    // if (winner == -1)
    // {
      for (int i=0; i<all_geometry.size();i++)
      {
        all_geometry[i].UpdateMatrix();
        all_geometry[i].UpdateWorldSpaceGeometry();
        all_geometry[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
      }

      for (int i=0; i<all_bonus.size();i++)
      {
        all_bonus[i].UpdateWorldSpaceGeometry();
        all_bonus[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
      }
    // }

    // if (editMode > 0)
    //   GeometryFromFileData();


    if (winner == -1)
    {
      for (int i=0; i<all_boxgeometry.size();i++)
      {
        if (editMode == 1)
        {
          all_boxgeometry[i].InputTransformation(deltaTime);
          all_boxgeometry[i].UpdateMatrix();
        }
        all_boxgeometry[i].UpdateWorldSpaceGeometry();
        all_boxgeometry[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
      }
    }

    if (editMode == 2)
    {
        for (int i=0; i<all_spawngeometry.size();i++)
        {
          all_spawngeometry[i].InputTransformation(deltaTime);
          all_spawngeometry[i].UpdateMatrix();
          all_spawngeometry[i].UpdateWorldSpaceGeometry();
          all_spawngeometry[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
        }
    }

    if (editMode == 0)
    {
      for (int i=0; i<all_spaceship.size();i++)
      {
        if (winner == -1 || winner == i)
          all_spaceship[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
      }
      for (int i=0; i<all_projectile.size();i++)
      {
        all_projectile[i].draw(&ildaFrame,(ofGetElapsedTimeMillis() * 0.001f));
      }
    }


    // do your thang
    ildaFrame.update();

    // draw to the screen
    ildaFrame.draw(0, 0, ofGetWidth(), ofGetHeight());

    // send points to the etherdream
    etherdream.setPoints(ildaFrame);

    ofSetColor(255);
    ofDrawBitmapString(ildaFrame.getString(), 10, 30);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch(key) {
        case 'f': ofToggleFullscreen(); break;


            // clear the frame
        case 'c': ildaFrame.clear(); break;

            // draw rectangle
        // case 'r': {
        //     ofPolyline p = ofPolyline::fromRectangle(ofRectangle(ofRandomuf()/2, ofRandomuf()/2, ofRandomuf()/2, ofRandomuf()/2));
        //     ildaFrame.addPoly(p);
        // }
        //     break;


        case 'u': ildaFrame.params.output.endCount -= 1; break;
        case 'U': ildaFrame.params.output.endCount += 1; break;


        case 'i': ildaFrame.params.output.blankCount -= 1; break;
        case 'I': ildaFrame.params.output.blankCount += 1; break;

            // change color
        case 'R': ildaFrame.params.output.color.r = 1 - ildaFrame.params.output.color.r; break;
        case 'G': ildaFrame.params.output.color.g = 1 - ildaFrame.params.output.color.g; break;
        case 'B': ildaFrame.params.output.color.b = 1 - ildaFrame.params.output.color.b; break;

            // toggle draw lines (on screen only)
        case 'l': ildaFrame.params.draw.lines ^= true; break;

            // toggle loop for last poly
        // case 'o': ildaFrame.getLastPoly().setClosed(ildaFrame.getLastPoly().isClosed()); break;

            // toggle draw points (on screen only)
        // case 'p': ildaFrame.params.draw.points ^= true; break;

        //     // adjust point count
        // case '.': ildaFrame.polyProcessor.params.targetPointCount++; break;
        // case ',': if(ildaFrame.polyProcessor.params.targetPointCount > 10) ildaFrame.polyProcessor.params.targetPointCount--; break;

            // adjust point count quicker
        case '>': ildaFrame.polyProcessor.params.spacing += 0.001f; break;
        case '<': if(ildaFrame.polyProcessor.params.spacing > 0.001f) ildaFrame.polyProcessor.params.spacing -= 0.001f; break;

            // flip image
        case 'x': ildaFrame.params.output.transform.doFlipX ^= true; break;
        case 'y': ildaFrame.params.output.transform.doFlipY ^= true; break;

        //     // cap image
        // case 'X': ildaFrame.params.output.doCapX ^= true; break;
        // case 'Y': ildaFrame.params.output.doCapY ^= true; break;

//ildaFrame.params.output.transform.offset.y
            // move output around
        case OF_KEY_UP: fileData.screen_offset_y -= 0.005; break;
        case OF_KEY_DOWN: fileData.screen_offset_y += 0.005; break;
        case OF_KEY_LEFT: fileData.screen_offset_x -= 0.005; break;
        case OF_KEY_RIGHT: fileData.screen_offset_x += 0.005; break;

            // scale output
        case 'z': fileData.screen_scale_y += 0.05; break;
        case 's': fileData.screen_scale_y -= 0.05; break;
        case 'q': fileData.screen_scale_x -= 0.05; break;
        case 'd': fileData.screen_scale_x += 0.05; break;

        case 'o': fileData.trapezeX -= 0.01; break;
        case 'O': fileData.trapezeX += 0.01; break;
        case 'p': fileData.trapezeY -= 0.01; break;
        case 'P': fileData.trapezeY += 0.01; break;
        case 'e': fileData.rot -= 0.5; break;
        case 'E': fileData.rot += 0.5; break;

        case 'm': ildaFrame.params.output.pointLineCount -= 1; break;
        case 'M': ildaFrame.params.output.pointLineCount += 1; break;


        case 'C': ildaFrame.drawCalibration(); break;

        case 'k': SaveToFile(); break;
        case 'j' : ChangeEditMode();
    }
}

void testApp::NewSpawn()
{
    Geometry carre;
    carre.Init(ofVec2f(0.5f,0.5f),0.0f);
    carre.Geo_Cross(ofFloatColor(1.0f,1.0f,0.0f,1.0f));
    carre.scale.x = 1.0f;
    carre.scale.y = 1.0f;
    //carre.pointsPerUnit = PPU;
    carre.UpdateMatrix();
    carre.UpdateWorldSpaceGeometry();
    all_spawngeometry.push_back(carre);

    CheckEditSelected(all_spawngeometry.size()-1);
}

void testApp::NewBox()
{
    Geometry carre;
    carre.Init(ofVec2f(0.5f,0.5f),0.0f);
    carre.Geo_Square(ofFloatColor(1.0f,1.0f,0.8f,1.0f));
    carre.scale.x = 0.1f;
    carre.scale.y = 0.1f;
    carre.pointsPerUnit = PPU;
    carre.UpdateMatrix();
    carre.UpdateWorldSpaceGeometry();

        ofLog(OF_LOG_NOTICE, "original rot %f", carre.rot);

    all_boxgeometry.push_back(carre);
            ofLog(OF_LOG_NOTICE, "copied rot %f", all_boxgeometry[all_boxgeometry.size()-1].rot);

    CheckEditSelected(all_boxgeometry.size()-1);
}

void testApp::RemoveItem()
{
  if (editMode == 1)
  {
    if (all_boxgeometry.size() > 0)
      all_boxgeometry.erase(all_boxgeometry.begin() + editSelectedItem);
  }
  else if (editMode == 2)
  {
    if (all_spawngeometry.size() > 0)
      all_spawngeometry.erase(all_spawngeometry.begin() + editSelectedItem);
  }
  CheckEditSelected(editSelectedItem);
}

void testApp::ChangeEditMode()
{
  editMode++;
  if (editMode == 3)
    editMode = 0;

  CheckEditSelected(editSelectedItem);
    ofLog(OF_LOG_NOTICE, "editmode %d", editMode);

}

void testApp::SaveToFile()
{

    for (int i=0; i<50; i++)
    {
      fileData.boxes[i].enabled = false;
      fileData.spawns[i].enabled = false;
    }

    for (int i=0; i<all_boxgeometry.size(); i++)
    {
      fileData.boxes[i].enabled = true;
      fileData.boxes[i].pos[0] = all_boxgeometry[i].pos.x;
      fileData.boxes[i].pos[1] = all_boxgeometry[i].pos.y;
      fileData.boxes[i].scale[0] = all_boxgeometry[i].scale.x;
      fileData.boxes[i].scale[1] = all_boxgeometry[i].scale.y;
      fileData.boxes[i].rot = all_boxgeometry[i].rot;
    }
    for (int i=0; i<all_spawngeometry.size(); i++)
    {
      fileData.spawns[i].enabled = true;
      fileData.spawns[i].pos[0] = all_spawngeometry[i].pos.x;
      fileData.spawns[i].pos[1] = all_spawngeometry[i].pos.y;
      fileData.spawns[i].scale[0] = all_spawngeometry[i].scale.x;
      fileData.spawns[i].scale[1] = all_spawngeometry[i].scale.y;
      fileData.spawns[i].rot = all_spawngeometry[i].rot;
    }

    fileData.endCount = ildaFrame.params.output.endCount;
    fileData.blankCount = ildaFrame.params.output.blankCount;
    fileData.targetPointCount = ildaFrame.polyProcessor.params.targetPointCount;
    fileData.spacing = ildaFrame.polyProcessor.params.spacing;
    fileData.flip_X = ildaFrame.params.output.transform.doFlipX;
    // fileData.screen_offset_y  = ildaFrame.params.output.transform.offset.y;
    // fileData.screen_offset_x = ildaFrame.params.output.transform.offset.x;
    // fileData.screen_scale_y = ildaFrame.params.output.transform.scale.y;
    // fileData.screen_scale_x = ildaFrame.params.output.transform.scale.x;
    fileData.pointLineCount = ildaFrame.params.output.pointLineCount;
    // fileData.rot = ildaFrame.rot;
    // fileData.trapezeX = ildaFrame.trapezeX;
    // fileData.trapezeY = ildaFrame.trapezeY;

    ofLog(OF_LOG_NOTICE, "save scale x %f", ildaFrame.params.output.transform.scale.x);
    ofLog(OF_LOG_NOTICE, "save scale x %f", fileData.screen_scale_x);

        ofBuffer dataBuffer;

    dataBuffer.set((char*)&fileData, sizeof(SaveFile));
    bool fileWritten = ofBufferToFile("data.dat", dataBuffer);
    ofLog(OF_LOG_NOTICE, "wrote to file %d", fileWritten);
    ofLog(OF_LOG_NOTICE, "size %d", dataBuffer.size());
}

void testApp::mouseMoved(int x, int y)
{
  intensity = 1.0f;
  return;
  intensity += (y-lastMouseY) * 0.05f;

  ofLog(OF_LOG_NOTICE, "the number is %d", intensity);

  if (intensity > 1.0f)
    intensity = 1.0f;
  if (intensity < 0.0f)
    intensity = 0.0f;

    lastMouseY = y;
}

void testApp::axisChanged(ofxGamepadAxisEvent& e)
{
  if (editMode > 0)
  {
      if (editMode == 1)
      {
        if (all_boxgeometry.size() > 0)
        {
          if (e.axis == 0)
          {
            if (abs(e.value) > 0.18f)
              all_boxgeometry[editSelectedItem].input_EditPos[0] = e.value * 0.3;
            else
              all_boxgeometry[editSelectedItem].input_EditPos[0] = 0.0f;
          }
          else if (e.axis == 1)
          {
            if (abs(e.value) > 0.18f)
              all_boxgeometry[editSelectedItem].input_EditPos[1] = e.value * 0.3;
            else
              all_boxgeometry[editSelectedItem].input_EditPos[1] = 0.0f;
          }
          else if (e.axis == 3)
          {
            if (abs(e.value) > 0.18f)
              all_boxgeometry[editSelectedItem].input_EditScale[0] = e.value;
            else
              all_boxgeometry[editSelectedItem].input_EditScale[0] = 0.0f;
          }
          else if (e.axis == 4)
          {
            if (abs(e.value) > 0.18f)
              all_boxgeometry[editSelectedItem].input_EditScale[1] = -e.value;
            else
              all_boxgeometry[editSelectedItem].input_EditScale[1] = 0.0f;
          }
          else if (e.axis == 5)
          {

            if ((e.value) > -0.8f)
              all_boxgeometry[editSelectedItem].input_EditRot = (e.value * 0.5f + 1.0f) * 30.0f;
            else
              all_boxgeometry[editSelectedItem].input_EditRot = 0.0f;
          }
        }
      }
      else
      {
        if (all_spawngeometry.size() > 0)
        {
          if (e.axis == 0)
          {
            if (abs(e.value) > 0.18f)
              all_spawngeometry[editSelectedItem].input_EditPos[0] = e.value * 0.3;
            else
              all_spawngeometry[editSelectedItem].input_EditPos[0] = 0.0f;
          }
          else if (e.axis == 1)
          {
            if (abs(e.value) > 0.18f)
              all_spawngeometry[editSelectedItem].input_EditPos[1] = e.value * 0.3;
            else
              all_spawngeometry[editSelectedItem].input_EditPos[1] = 0.0f;
          }
          else if (e.axis == 3)
          {
            if (abs(e.value) > 0.18f)
              all_spawngeometry[editSelectedItem].input_EditScale[0] = e.value;
            else
              all_spawngeometry[editSelectedItem].input_EditScale[0] = 0.0f;
          }
          else if (e.axis == 4)
          {
            if (abs(e.value) > 0.18f)
              all_spawngeometry[editSelectedItem].input_EditScale[1] = e.value;
            else
              all_spawngeometry[editSelectedItem].input_EditScale[1] = 0.0f;
          }
          else if (e.axis == 5)
          {
            if ((e.value) > -0.8f)
              all_spawngeometry[editSelectedItem].input_EditRot = (e.value * 0.5f + 1.0f) * 30.0f;
            else
              all_spawngeometry[editSelectedItem].input_EditRot = 0.0f;
          }
        }
      }
  }
  else
  {

    if (e.axis == 0)
    {
     if (abs(e.value) > 0.15f)
        all_spaceship[e.padID].input_rotate = e.value;
      else
        all_spaceship[e.padID].input_rotate = 0.0f;
    }
    if (e.axis == 5)
    {
     if ((e.value) > -0.8f)
        all_spaceship[e.padID].input_go = (e.value+1.0f) * 0.5f;
      else
        all_spaceship[e.padID].input_go = 0.0f;
    }
    if (e.axis == 2)
    {
     if ((e.value) > -0.8f)
        all_spaceship[e.padID].input_reverse = (e.value+1.0f) * 0.5f;
      else
        all_spaceship[e.padID].input_reverse = 0.0f;
    }
  }
	//cout << "ID " << e.padID << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
}

void testApp::buttonPressed(ofxGamepadButtonEvent& e)
{

  if (editMode > 0)
  {
    if (e.button == 4)
      CheckEditSelected(editSelectedItem-1);
    if (e.button == 5)
      CheckEditSelected(editSelectedItem+1);

    if (e.button == 1)
    {
      RemoveItem();
      CheckEditSelected(editSelectedItem);
    }
    if (e.button == 0)
    {
      if (editMode == 1)
        NewBox();
      else
        NewSpawn();

      CheckEditSelected(editSelectedItem);
    }
  }
  else
  {
    if (e.button == 0)
    {
      if (all_spaceship[e.padID].Fire((ofGetElapsedTimeMillis()) * 0.001f))
      {
          Projectile proj;
          proj.Init(all_spaceship[e.padID].pos + ofVec2f(all_spaceship[e.padID].rot_mat[2],all_spaceship[e.padID].rot_mat[3]) * 0.022,
          all_spaceship[e.padID].rot,
          all_spaceship[e.padID].color);
          //myship.Geo_SpaceShip(ofFloatColor(ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),ofRandom(0.2f, 0.8f),1.0f));
          all_projectile.push_back(proj);
      }
      //all_spaceship[e.padID].input_rotate = e.value;
    }
  }

	//cout << "ID " << e.padID << "BUTTON " << e.button << " PRESSED" << endl;
}

void testApp::buttonReleased(ofxGamepadButtonEvent& e)
{
	//cout << "ID " << e.padID << "BUTTON " << e.button << " RELEASED" << endl;
}


//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    // draw a line to the mouse cursor (normalized coordinates) in the last poly created
    ildaFrame.getLastPoly().lineTo(x / (float)ofGetWidth(), y / (float)ofGetHeight());
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    // create a new poly in the ILDA frame
    ildaFrame.getLastPoly().lineTo(x / (float)ofGetWidth(), y / (float)ofGetHeight());
}
