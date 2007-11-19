/*
===============================================================================

  FILE:  lasview.cpp
  
  CONTENTS:
  
    This little tool is just a quick little hack to visualize an LAS files.
  
  PROGRAMMERS:
  
    martin isenburg@cs.unc.edu
  
  COPYRIGHT:
  
    copyright (C) 2005  martin isenburg@cs.unc.edu
    
    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  
  CHANGE HISTORY:
  
    9 May 2007 -- adapted from my streaming point viewer
  
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>

#include "lasreader.h"

#ifdef _WIN32
extern "C" FILE* fopenGzipped(const char* filename, const char* mode);
#endif

// MOUSE INTERFACE

static int LeftButtonDown=0;
static int MiddleButtonDown=0;
static int RightButtonDown=0;
static int OldX,OldY;
static float Elevation=0;
static float Azimuth=0;
static float DistX=0;
static float DistY=0;
static float DistZ=2;

// VISUALIZATION SETTINGS

static float boundingBoxMin[3];
static float boundingBoxMax[3];
static float boundingBoxHeight;
static float boundingBoxScale=1.0f;
static float boundingBoxTranslateX = 0.0f;
static float boundingBoxTranslateY = 0.0f;
static float boundingBoxTranslateZ = 0.0f;

// GLOBAL CONTROL VARIABLES

static int WindowW=1024, WindowH=768;
static int InteractionMode=0;
static int AnimationOn=0;
static int WorkingOn=0;

// COLORS

static float colours_diffuse[10][4];
static float colours_white[4];
static float colours_light_blue[4];

// DATA STORAGE FOR STREAM VISUALIZER OUTPUT

static FILE* file = 0;
static char* file_name = 0;
static LASreader* lasreader = 0;

static int p_count = 0;
static int npoints = 0;
static float* point_buffer = 0;
static int point_buffer_alloc = 0;

static bool only_first = false;
static bool only_last = false;

static int EXACTLY_N_STEPS = 100;
static int EVERY_NTH_STEP = -1;
static int NEXT_STEP;

static int EXACTLY_N_POINTS = 1000000;
static int EVERY_NTH_POINT = 0;
static int NEXT_POINT;

static int DIRTY_POINTS=1;
static int REPLAY_IT=0;
static int REPLAY_COUNT=0;
static int STREAM_COLORING = 3;
static int POINT_SIZE = 2;
static int RENDER_BOUNDINGBOX = 1;
static int EXTRA_Z_SCALE = 1;

static void InitColors()
{
  colours_diffuse[0][0] = 0.0f; colours_diffuse[0][1] = 0.0f; colours_diffuse[0][2] = 0.0f; colours_diffuse[0][3] = 1.0f; // black
  colours_diffuse[1][0] = 0.6f; colours_diffuse[1][1] = 0.0f; colours_diffuse[1][2] = 0.0f; colours_diffuse[1][3] = 1.0f; // red
  colours_diffuse[2][0] = 0.0f; colours_diffuse[2][1] = 0.8f; colours_diffuse[2][2] = 0.0f; colours_diffuse[2][3] = 1.0f; // green
  colours_diffuse[3][0] = 0.0f; colours_diffuse[3][1] = 0.0f; colours_diffuse[3][2] = 0.6f; colours_diffuse[3][3] = 1.0f; // blue
  colours_diffuse[4][0] = 0.6f; colours_diffuse[4][1] = 0.6f; colours_diffuse[4][2] = 0.0f; colours_diffuse[4][3] = 1.0f; // yellow
  colours_diffuse[5][0] = 0.6f; colours_diffuse[5][1] = 0.0f; colours_diffuse[5][2] = 0.6f; colours_diffuse[5][3] = 1.0f; // purple
  colours_diffuse[6][0] = 0.0f; colours_diffuse[6][1] = 0.6f; colours_diffuse[6][2] = 0.6f; colours_diffuse[6][3] = 0.3f; // cyan (tranparent)
  colours_diffuse[7][0] = 0.7f; colours_diffuse[7][1] = 0.7f; colours_diffuse[7][2] = 0.7f; colours_diffuse[7][3] = 1.0f; // white
  colours_diffuse[8][0] = 0.2f; colours_diffuse[8][1] = 0.2f; colours_diffuse[8][2] = 0.6f; colours_diffuse[8][3] = 1.0f; // light blue
  colours_diffuse[9][0] = 0.9f; colours_diffuse[9][1] = 0.4f; colours_diffuse[9][2] = 0.7f; colours_diffuse[9][3] = 0.5f; // violett
  
  colours_white[0] = 0.7f; colours_white[1] = 0.7f; colours_white[2] = 0.7f; colours_white[3] = 1.0f; // white
  colours_light_blue[0] = 0.2f; colours_light_blue[1] = 0.2f; colours_light_blue[2] = 0.6f; colours_light_blue[3] = 1.0f; // light blue
}

static void InitLight()
{
  float intensity[] = {1,1,1,1};
  float position[] = {1,1,5,0}; // directional behind the viewer
  glLightfv(GL_LIGHT0,GL_DIFFUSE,intensity);
  glLightfv(GL_LIGHT0,GL_SPECULAR,intensity);
  glLightfv(GL_LIGHT0,GL_POSITION,position);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,GL_FALSE);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,GL_TRUE);
}

static void usage()
{
  fprintf(stderr,"usage:\n");
  fprintf(stderr,"lasview -i terrain.las\n");
  fprintf(stderr,"lasview -i terrain.las -win 1600 1200 -steps 10 -points 200000\n");
  fprintf(stderr,"lasview -h\n");
  fprintf(stderr,"\n");
}

static void vizBegin()
{
  REPLAY_IT = 0; // just making sure
  DIRTY_POINTS = 1;

  if (file_name)
  {
    fprintf(stderr,"loading '%s'...\n",file_name);
    if (strstr(file_name, ".gz"))
    {
#ifdef _WIN32
      file = fopenGzipped(file_name, "rb");
#else
      fprintf(stderr,"ERROR: cannot open gzipped file %s\n",file_name);
      exit(1);
#endif
    }
    else
    {
      file = fopen(file_name, "rb");
    }
    if (file == 0)
    {
      fprintf(stderr,"ERROR: cannot open %s\n",file_name);
      exit(1);
    }
    lasreader = new LASreader();
    if (lasreader->open(file) == false)
    {
      fprintf(stderr,"ERROR: no input\n");
      exit(1);
    }
  }
  else
  {
    fprintf(stderr,"ERROR: no input\n");
    exit(1);
  }
  
  // scale and translate bounding box for rendering

  boundingBoxMin[0] = lasreader->header.min_x;
  boundingBoxMin[1] = lasreader->header.min_y;
  boundingBoxMin[2] = lasreader->header.min_z;

  boundingBoxMax[0] = lasreader->header.max_x;
  boundingBoxMax[1] = lasreader->header.max_y;
  boundingBoxMax[2] = lasreader->header.max_z;

  boundingBoxHeight = boundingBoxMax[2]-boundingBoxMin[2];

  if ((boundingBoxMax[1]-boundingBoxMin[1]) > (boundingBoxMax[0]-boundingBoxMin[0]))
  {
    if ((boundingBoxMax[1]-boundingBoxMin[1]) > (boundingBoxMax[2]-boundingBoxMin[2]))
    {
      boundingBoxScale = 1.0f/(boundingBoxMax[1]-boundingBoxMin[1]);
    }
    else
    {
      boundingBoxScale = 1.0f/(boundingBoxMax[2]-boundingBoxMin[2]);
    }
  }
  else
  {
    if ((boundingBoxMax[0]-boundingBoxMin[0]) > (boundingBoxMax[2]-boundingBoxMin[2]))
    {
      boundingBoxScale = 1.0f/(boundingBoxMax[0]-boundingBoxMin[0]);
    }
    else
    {
      boundingBoxScale = 1.0f/(boundingBoxMax[2]-boundingBoxMin[2]);
    }
  }
  boundingBoxTranslateX = - boundingBoxScale * (boundingBoxMin[0] + 0.5f * (boundingBoxMax[0]-boundingBoxMin[0]));
  boundingBoxTranslateY = - boundingBoxScale * (boundingBoxMin[1] + 0.5f * (boundingBoxMax[1]-boundingBoxMin[1]));
  boundingBoxTranslateZ = - boundingBoxScale * (boundingBoxMin[2] + 0.5f * (boundingBoxMax[2]-boundingBoxMin[2]));

  p_count = 0;
  npoints = lasreader->npoints;

  fprintf(stderr,"number of points in file %d\n", npoints);

  if (EVERY_NTH_STEP == -1)
  {
    EVERY_NTH_STEP = npoints / EXACTLY_N_STEPS;
  }
  if (EVERY_NTH_STEP == 0)
  {
    EVERY_NTH_STEP = 1;
  }
  NEXT_STEP = EVERY_NTH_STEP;

  if (EXACTLY_N_POINTS)
  {
    EVERY_NTH_POINT = npoints/EXACTLY_N_POINTS;
  }
  if (EVERY_NTH_POINT == 0)
  {
    EVERY_NTH_POINT = 1;
  }
  NEXT_POINT = 0;

  // make sure we have enough memory

  if (point_buffer_alloc < ((npoints / EVERY_NTH_POINT) + 500))
  {
    if (point_buffer) free(point_buffer);
    point_buffer_alloc = ((npoints / EVERY_NTH_POINT) + 500);
    point_buffer = (float*)malloc(sizeof(float)*3*point_buffer_alloc);
  }
}

static void vizEnd()
{
  REPLAY_IT = 0; // just making sure
  REPLAY_COUNT = p_count;
  DIRTY_POINTS = 0;

  fprintf(stderr,"number of points sampled %d\n",p_count);

  lasreader->close();
  fclose(file);
  delete lasreader;
  lasreader = 0;
}

static int vizContinue()
{
  int more;
  REPLAY_IT = 0; // just making sure

  while (more = lasreader->read_point(&(point_buffer[p_count*3])))
  {
    if (lasreader->p_count > NEXT_POINT)
    {
      p_count++;
      NEXT_POINT += ((EVERY_NTH_POINT/2) + (rand()%EVERY_NTH_POINT) + 1);
    }

    if (lasreader->p_count > NEXT_STEP)
    {
      NEXT_STEP += EVERY_NTH_STEP;
      break;
    }
  }

  return more;
}

static void myReshape(int w, int h)
{
  glutReshapeWindow(WindowW,WindowH);
}

static void myIdle()
{
  if (AnimationOn)
  {
    AnimationOn = vizContinue();
    if (!AnimationOn)
    {
      WorkingOn = 0;
      vizEnd();
    }
    glutPostRedisplay();
  }
  else if (REPLAY_IT)
  {
    REPLAY_COUNT += NEXT_STEP;
    glutPostRedisplay();
  }
}

static void full_resolution_rendering()
{
  if (file_name == 0)
  {
    fprintf(stderr,"ERROR: no input file\n");
  }

  int p_count;

  if (lasreader)
  {
    p_count = lasreader->p_count;
    fprintf(stderr,"out-of-core rendering of %d points ... \n",p_count);
    lasreader->close();
    fclose(file);
    delete lasreader;
    lasreader = 0;
  }
  else
  {
    p_count = 2000000000;
    fprintf(stderr,"out-of-core rendering of all points ... \n");
  }

  if (strstr(file_name, ".gz"))
  {
#ifdef _WIN32
    file = fopenGzipped(file_name, "rb");
#else
    fprintf(stderr,"ERROR: cannot open gzipped file %s\n",file_name);
    exit(1);
#endif
  }
  else
  {
    file = fopen(file_name, "rb");
  }
  if (file == 0)
  {
    fprintf(stderr,"ERROR: cannot open %s\n",file_name);
    exit(1);
  }
  lasreader = new LASreader();
  if (lasreader->open(file) == false)
  {
    fprintf(stderr,"ERROR: no input\n");
    exit(1);
  }

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glViewport(0,0,WindowW,WindowH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0f,(float)WindowW/WindowH,0.0625f,5.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(DistX,DistY,DistZ, DistX,DistY,0, 0,1,0);

  glRotatef(Elevation,1,0,0);
  glRotatef(Azimuth,0,1,0);

  glTranslatef(boundingBoxTranslateX,boundingBoxTranslateY,boundingBoxTranslateZ*EXTRA_Z_SCALE);
  glScalef(boundingBoxScale,boundingBoxScale,boundingBoxScale*EXTRA_Z_SCALE);

  glEnable(GL_DEPTH_TEST);

  if (p_count == 0)
  {
    p_count = lasreader->npoints;
  }

  double point[3];
  glBegin(GL_POINTS);
  glColor3f(0,0,0);
  while (lasreader->p_count < p_count)
  {
    if (lasreader->read_point(point))
    {
      glVertex3dv(point);
    }
    else
    {
      glEnd();
      lasreader->close();
      fclose(file);
      delete lasreader;
      lasreader = 0;
      glutSwapBuffers();
      return;
    }
  }
  glEnd();

  glDisable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

static void myMouseFunc(int button, int state, int x, int y)
{
  OldX=x;
  OldY=y;        
  if (button == GLUT_LEFT_BUTTON)
  {
    LeftButtonDown = !state;
    MiddleButtonDown = 0;
    RightButtonDown = 0;
  }
  else if (button == GLUT_RIGHT_BUTTON)
  {
    LeftButtonDown = 0;
    MiddleButtonDown = 0;
    RightButtonDown = !state;
  }
  else if (button == GLUT_MIDDLE_BUTTON)
  {
    LeftButtonDown = 0;
    MiddleButtonDown = !state;
    RightButtonDown = 0;
  }
}

static void myMotionFunc(int x, int y)
{
  float RelX = (x-OldX) / (float)glutGet((GLenum)GLUT_WINDOW_WIDTH);
  float RelY = (y-OldY) / (float)glutGet((GLenum)GLUT_WINDOW_HEIGHT);
  OldX=x;
  OldY=y;
  if (LeftButtonDown) 
  { 
    if (InteractionMode == 0)
    {
      Azimuth += (RelX*180);
      Elevation += (RelY*180);
    }
    else if (InteractionMode == 1)
    {
      DistX-=RelX;
      DistY+=RelY;
    }
    else if (InteractionMode == 2)
    {
      DistZ-=RelY*DistZ;
    }
  }
  else if (MiddleButtonDown)
  {
    DistX-=RelX*1.0f;
    DistY+=RelY*1.0f;
  }
  glutPostRedisplay();
}

static void MyMenuFunc(int value);

static void myKeyboard(unsigned char Key, int x, int y)
{
  switch(Key)
  {
  case 'Q':
  case 'q':
  case 27:
    exit(0);
    break;
  case ' ': // rotate, translate, or zoom
    if (InteractionMode == 2)
    {
      InteractionMode = 0;
    }
    else
    {
      InteractionMode += 1;
    }
    glutPostRedisplay();
    break;
  case '>': //zoom in
    DistZ-=0.1f;
    break;
  case '<': //zoom out
    DistZ+=0.1f;
    break;
  case '_':
    break;
  case '+':
    break;
  case '-':
    POINT_SIZE -= 1;
    if (POINT_SIZE < 0) POINT_SIZE = 0;
    fprintf(stderr,"POINT_SIZE %d\n",POINT_SIZE);
    glutPostRedisplay();
    break;
  case '=':
    POINT_SIZE += 1;
    fprintf(stderr,"POINT_SIZE %d\n",POINT_SIZE);
    glutPostRedisplay();
    break;
  case '[':
    if (EXTRA_Z_SCALE > 1)
    {
      EXTRA_Z_SCALE = EXTRA_Z_SCALE >> 1;
      glutPostRedisplay();
    }
    fprintf(stderr,"EXTRA_Z_SCALE %d\n",EXTRA_Z_SCALE);
    break;
  case ']':
    EXTRA_Z_SCALE = EXTRA_Z_SCALE << 1;
    fprintf(stderr,"EXTRA_Z_SCALE %d\n",EXTRA_Z_SCALE);
    glutPostRedisplay();
    break;
  case 'O':
  case 'o':
    AnimationOn = 0;
    REPLAY_IT = 0;
    break;
  case 'B':
  case 'b':
    RENDER_BOUNDINGBOX = !RENDER_BOUNDINGBOX;
    fprintf(stderr,"RENDER_BOUNDINGBOX %d\n",RENDER_BOUNDINGBOX);
    break;
  case 'R':
  case 'r':
    if (file_name)
    {
      full_resolution_rendering();
    }
    else
    {
      fprintf(stderr,"WARNING: out-of-core rendering from file not possible in pipe mode.\n");
    }
    break;
  case 'C':
  case 'c':
    STREAM_COLORING += 1;
    if (STREAM_COLORING > 4) STREAM_COLORING = 0;
    fprintf(stderr,"STREAM_COLORING %d\n",STREAM_COLORING);
    glutPostRedisplay();
    break;
  case 'T':
    if (DIRTY_POINTS)
    {
      // works only in replay mode
      fprintf(stderr,"tiny steps only work during second play (replay)\n");
    }
    else
    {
      REPLAY_COUNT -= 1;
      if (REPLAY_COUNT < 0)
      {
        REPLAY_COUNT = 0;
      }
    }
    glutPostRedisplay();
    break;
  case 't':
    if (DIRTY_POINTS)
    {
      NEXT_STEP = lasreader->p_count;
      WorkingOn = vizContinue();
    }
    else
    {
      if (REPLAY_COUNT >= p_count)
      {
        REPLAY_COUNT = 0;
      }
      REPLAY_COUNT += 1;
    }
    glutPostRedisplay();
    break;
  case 'S':
    if (DIRTY_POINTS)
    {
      // works only in replay mode
      fprintf(stderr,"back stepping only work during second play (replay)\n");
    }
    else
    {
      NEXT_STEP = p_count / EXACTLY_N_STEPS;
      if (NEXT_STEP == 0) NEXT_STEP = 1;
      REPLAY_COUNT -= NEXT_STEP;
      if (REPLAY_COUNT < 0)
      {
        REPLAY_COUNT = 0;
      }
    }
    glutPostRedisplay();
    break;
  case 'P':
    if (file_name)
    {
      DIRTY_POINTS = 1;
    }
    else
    {
      fprintf(stderr,"WARNING: cannot replay from file when operating in pipe mode.\n");
    }
  case 'p':
    if (DIRTY_POINTS)
    {
      AnimationOn = !AnimationOn;
    }
    else
    {
      if (REPLAY_IT == 0)
      {
        if (REPLAY_COUNT >= p_count)
        {
          REPLAY_COUNT = 0;
        }
        NEXT_STEP = p_count / EXACTLY_N_STEPS;
        if (NEXT_STEP == 0) NEXT_STEP = 1;
        REPLAY_IT = 1;
      }
      else
      {
        REPLAY_IT = 0;
      }
    }
  case 's':
    if (DIRTY_POINTS)
    {
      if (WorkingOn == 0)
      {
        vizBegin();
        WorkingOn = vizContinue();
      }
      else
      {
        WorkingOn = vizContinue();
      }
      if (WorkingOn == 0)
      {
        vizEnd();
        AnimationOn = 0;
      }
    }
    else
    {
      if (REPLAY_COUNT >= p_count)
      {
        REPLAY_COUNT = 0;
      }
      NEXT_STEP = p_count / EXACTLY_N_STEPS;
      if (NEXT_STEP == 0) NEXT_STEP = 1;
      REPLAY_COUNT += NEXT_STEP;
    }
    glutPostRedisplay();
    break;
  case 'K':
  case 'k':
    printf("-kamera %g %g %g %g %g\n", Azimuth, Elevation, DistX,DistY,DistZ);
    break;
  };
}

static void MyMenuFunc(int value)
{
  if (value >= 100)
  {
    if (value <= 102)
    {
      InteractionMode = value - 100;
      glutPostRedisplay();
    }
    else if (value == 103)
    {
      myKeyboard('s',0,0);
    }
    else if (value == 104)
    {
      myKeyboard('p',0,0);
    }
    else if (value == 105)
    {
      myKeyboard('o',0,0);
    }
    else if (value == 109)
    {
      myKeyboard('q',0,0);
    }
    else if (value == 150)
    {
      myKeyboard('c',0,0);
    }
    else if (value == 151)
    {
      myKeyboard('=',0,0);
    }
    else if (value == 152)
    {
      myKeyboard('-',0,0);
    }
    else if (value == 153)
    {
      myKeyboard(']',0,0);
    }
    else if (value == 154)
    {
      myKeyboard('[',0,0);
    }
  }
  else if (value == 40)
  {
    EXACTLY_N_STEPS = 5;
  }
  else if (value == 41)
  {
    EXACTLY_N_STEPS = 10;
  }
  else if (value == 42)
  {
    EXACTLY_N_STEPS = 25;
  }
  else if (value == 43)
  {
    EXACTLY_N_STEPS = 50;
  }
  else if (value == 44)
  {
    EXACTLY_N_STEPS = 100;
  }
  else if (value == 45)
  {
    EXACTLY_N_STEPS = 250;
  }
  else if (value == 46)
  {
    EXACTLY_N_STEPS = 500;
  }
  else if (value == 47)
  {
    EXACTLY_N_STEPS = 1000;
  }
  else if (value == 48)
  {
    EXACTLY_N_STEPS = 10000;
  }
}

static void displayMessage()
{
  glColor3f( 0.7f, 0.7f, 0.7f );  // Set colour to grey
  glMatrixMode( GL_PROJECTION );
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D( 0.0f, 1.0f, 0.0f, 1.0f );
  glMatrixMode( GL_MODELVIEW );
  glPushMatrix();
  glLoadIdentity();
  glRasterPos2f( 0.03f, 0.95f );
  
  if( InteractionMode == 0 )
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'r');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'a');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'e');
  }
  else if( InteractionMode == 1 )
  {    
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'r');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'a');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'n');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 's');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'l');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'a');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 't');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'e');
  }
  else
  {
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'z');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'o');
    glutBitmapCharacter(GLUT_BITMAP_8_BY_13, 'm');
  }

  glPopMatrix();
  glMatrixMode( GL_PROJECTION );
  glPopMatrix();
}

static void myDisplay()
{
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glViewport(0,0,WindowW,WindowH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(30.0f,(float)WindowW/WindowH,0.0625f,5.0f);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(DistX,DistY,DistZ, DistX,DistY,0, 0,1,0);

  glRotatef(Elevation,1,0,0);
  glRotatef(Azimuth,0,1,0);

  glTranslatef(boundingBoxTranslateX,boundingBoxTranslateY,boundingBoxTranslateZ*EXTRA_Z_SCALE);
  glScalef(boundingBoxScale,boundingBoxScale,boundingBoxScale*EXTRA_Z_SCALE);

  int rendered_points;

  if (DIRTY_POINTS)
  {
    rendered_points = p_count;
  }
  else
  {
    if (REPLAY_COUNT > p_count)
    {
      rendered_points = p_count;
      REPLAY_IT = 0;
    }
    else
    {
      rendered_points = REPLAY_COUNT;
    }
  }

  // draw points

  glEnable(GL_DEPTH_TEST);

  if (p_count && POINT_SIZE)
  {
    glPointSize(POINT_SIZE);
    glBegin(GL_POINTS);
    if (STREAM_COLORING == 0)
    {
      glColor3f(0,0,0);
      for (int i = 0; i < rendered_points; i++)
      {
        glVertex3fv(&(point_buffer[i*3]));
      }
    }
    else if (STREAM_COLORING == 1)
    {
      float color;
      for (int i = 0; i < rendered_points; i++)
      {
        color = 0.1f+0.7f*i/p_count;
        glColor3f(color,color,color);
        glVertex3fv(&(point_buffer[i*3]));
      }
    }
    else if (STREAM_COLORING == 2)
    {
      for (int i = 0; i < rendered_points; i++)
      {
        if (i < p_count/3)
        {
          glColor3f(0.1f+0.7f*i/(p_count/3),0.1f,0.1f);
        }
        else if (i < 2*(p_count/3))
        {
          glColor3f(0.8f,0.1f+0.7f*(i-(p_count/3))/(p_count/3),0.1f);
        }
        else
        {
          glColor3f(0.8f, 0.8f, 0.1f+0.7f*(i-2*(p_count/3))/(p_count/3));
        }
        glVertex3fv(&(point_buffer[i*3]));
      }
    }
    else if (STREAM_COLORING == 3)
    {
      for (int i = 0; i < rendered_points; i++)
      {
        float height = point_buffer[i*3+2] - boundingBoxMin[2];
        if (height < (boundingBoxHeight/3))
        {
          glColor3f(0.1f+0.7f*height/(boundingBoxHeight/3),0.1f,0.1f);
        }
        else if (height < 2*(boundingBoxHeight/3))
        {
          glColor3f(0.8f,0.1f+0.7f*(height-(boundingBoxHeight/3))/(boundingBoxHeight/3),0.1f);
        }
        else
        {
          glColor3f(0.8f, 0.8f, 0.1f+0.7f*(height-2*(boundingBoxHeight/3))/(boundingBoxHeight/3));
        }
        glVertex3fv(&(point_buffer[i*3]));
      }
    }
    else
    {
      for (int i = 0; i < rendered_points; i++)
      {
        float height = point_buffer[i*3+2] - boundingBoxMin[2];
        if (height < (boundingBoxHeight/2))
        {
          glColor3f(0.1f+0.7f*height/(boundingBoxHeight/2),0.1f,0.1f+0.8f-0.7f*height/(boundingBoxHeight/2));
        }
        else if (height < 2*(boundingBoxHeight/2))
        {
          glColor3f(0.8f, 0.1f + 0.7f*(height-(boundingBoxHeight/2))/(boundingBoxHeight/2), 0.1f + 0.7f*(height-(boundingBoxHeight/2))/(boundingBoxHeight/2));
        }
        glVertex3fv(&(point_buffer[i*3]));
      }
    }
    glEnd();
  }

  if (RENDER_BOUNDINGBOX)
  {
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glColor3f(0,1,0);
    glLineWidth(1.0f);
    glBegin(GL_QUADS);
    if (RENDER_BOUNDINGBOX == 1)
    {
      glVertex3f(boundingBoxMin[0], boundingBoxMin[1], boundingBoxMin[2]);
      glVertex3f(boundingBoxMin[0], boundingBoxMax[1], boundingBoxMin[2]);
      glVertex3f(boundingBoxMax[0], boundingBoxMax[1], boundingBoxMin[2]);
      glVertex3f(boundingBoxMax[0], boundingBoxMin[1], boundingBoxMin[2]);
      glVertex3f(boundingBoxMin[0], boundingBoxMin[1], boundingBoxMax[2]);
      glVertex3f(boundingBoxMin[0], boundingBoxMax[1], boundingBoxMax[2]);
      glVertex3f(boundingBoxMax[0], boundingBoxMax[1], boundingBoxMax[2]);
      glVertex3f(boundingBoxMax[0], boundingBoxMin[1], boundingBoxMax[2]);
    }
    glEnd();
    glLineWidth(1.0f);
  }

  glDisable(GL_DEPTH_TEST);

  displayMessage();
  glutSwapBuffers();
}

int main(int argc, char *argv[])
{
  if (argc == 1 || strcmp(argv[1],"-h") == 0 || strcmp(argv[1],"-help") == 0) 
  {
    usage();
    exit(0);
  }
  else
  {
    glutInit(&argc, argv);

    for (int i = 1; i < argc; i++)
    {
      if (strcmp(argv[i],"-win") == 0)
      {
        i++;
        WindowW = atoi(argv[i]);
        i++;
        WindowH = atoi(argv[i]);
      }
      else if (strcmp(argv[i],"-steps") == 0)
      {
        i++;
        EXACTLY_N_STEPS = atoi(argv[i]);
      }
      else if (strcmp(argv[i],"-every") == 0)
      {
        i++;
        EVERY_NTH_STEP = atoi(argv[i]);
      }
      else if (strcmp(argv[i],"-points") == 0)
      {
        i++;
        EXACTLY_N_POINTS = atoi(argv[i]);
      }
      else if (strcmp(argv[i],"-kamera") == 0)
      {
        i++;
        sscanf(argv[i], "%f", &Azimuth);
        i++;
        sscanf(argv[i], "%f", &Elevation);
        i++;
        sscanf(argv[i], "%f", &DistX);
        i++;
        sscanf(argv[i], "%f", &DistY);
        i++;
        sscanf(argv[i], "%f", &DistZ);
      }
      else if (strcmp(argv[i],"-only_first") == 0)
      {
        only_first = true;
      }
      else if (strcmp(argv[i],"-only_last") == 0)
      {
        only_last = true;
      }
      else if (strcmp(argv[i],"-i") == 0)
      {
        i++;
        file_name = argv[i];
      }
      else if (i == argc-1)
      {
        file_name = argv[argc-1];
      }
      else
      {
        usage();
      }
    }
  }

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WindowW,WindowH);
  glutInitWindowPosition(180,100);
  glutCreateWindow("just a little LAS viewer");
  
  glShadeModel(GL_FLAT);
  
  InitColors();
  InitLight();
  
  glutDisplayFunc(myDisplay);
  glutReshapeFunc(myReshape);
  glutIdleFunc(myIdle);
  
  glutMouseFunc(myMouseFunc);
  glutMotionFunc(myMotionFunc);
  glutKeyboardFunc(myKeyboard);

  // steps sub menu
  int menuSteps = glutCreateMenu(MyMenuFunc);
  glutAddMenuEntry("in 5 steps", 40);
  glutAddMenuEntry("in 10 steps", 41);
  glutAddMenuEntry("in 25 steps", 42);
  glutAddMenuEntry("in 50 steps", 43);
  glutAddMenuEntry("in 100 steps", 44);
  glutAddMenuEntry("in 250 steps", 45);
  glutAddMenuEntry("in 500 steps", 46);
  glutAddMenuEntry("in 1000 steps", 47);
  glutAddMenuEntry("in 10000 steps", 48);

  // main menu
  glutCreateMenu(MyMenuFunc);
  glutAddSubMenu("steps ...", menuSteps);
  glutAddMenuEntry("", 0);
  glutAddMenuEntry("rotate <SPACE>", 100);
  glutAddMenuEntry("translate <SPACE>", 101);
  glutAddMenuEntry("zoom <SPACE>", 102);
  glutAddMenuEntry("", 0);
  glutAddMenuEntry("<s>tep", 103);
  glutAddMenuEntry("<p>lay", 104);
  glutAddMenuEntry("st<o>p", 105);
  glutAddMenuEntry("", 0);
  glutAddMenuEntry("<c>oloring mode", 150);
  glutAddMenuEntry("points large <=>", 151);
  glutAddMenuEntry("points small <->", 152);
  glutAddMenuEntry("height more <]>", 153);
  glutAddMenuEntry("height less <[>", 154);
  glutAddMenuEntry("", 0);
  glutAddMenuEntry("<Q>UIT", 109);
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  myKeyboard('p',0,0);

  glutMainLoop();

  return 0;
}
