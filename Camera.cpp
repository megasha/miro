#include <stdio.h>
#include <stdlib.h>
#include "Miro.h"
#include "Camera.h"
#include "Image.h"
#include "Scene.h"
#include "Console.h" 
#include "OpenGL.h"

Camera * g_camera = 0;

static bool firstRayTrace = true; 

const float HalfDegToRad = DegToRad/2.0f;

Camera::Camera() :
    m_bgColor(0,0,0),
    m_renderer(RENDER_OPENGL),
    m_eye(0,0,0),
    m_viewDir(0,0,-1),
    m_up(0,1,0),
    m_lookAt(FLT_MAX, FLT_MAX, FLT_MAX),
	m_lookAtOrig(FLT_MAX, FLT_MAX, FLT_MAX),
    m_fov((45.)*(PI/180.))
{
    calcLookAt();
}


Camera::~Camera()
{

}


void
Camera::click(Scene* pScene, Image* pImage)
{
    calcLookAt();
    static bool firstRayTrace = false;

    if (m_renderer == RENDER_OPENGL)
    {
        glDrawBuffer(GL_BACK);
        pScene->openGL(this);
        firstRayTrace = true;
    }
    else if (m_renderer == RENDER_RAYTRACE)
    {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        glDrawBuffer(GL_FRONT);
        if (firstRayTrace)
        {
            pImage->clear(bgColor());
            pScene->raytraceImage(this, g_image);
            firstRayTrace = false;
        }
        
        g_image->draw();
    }
}


void
Camera::calcLookAt()
{
    // this is true when a "lookat" is not used in the config file
    if (m_lookAt.x != FLT_MAX)
    {
        setLookAt(m_lookAt);
        m_lookAt.set(FLT_MAX, FLT_MAX, FLT_MAX);
    }
}


void
Camera::drawGL()
{
    // set up the screen with our camera parameters
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov(), g_image->width()/(float)g_image->height(),
                   0.01, 10000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    Vector3 vCenter = eye() + viewDir();
    gluLookAt(eye().x, eye().y, eye().z,
              vCenter.x, vCenter.y, vCenter.z,
              up().x, up().y, up().z);
}

void
Camera::PrintCamera()
{
	std::cout << "\nEye: " << m_eye << std::endl;
	std::cout << "LookAt: " << m_lookAt << std::endl;
	std::cout << "Up: " << m_up << std::endl;
	std::cout << "FOV: " << m_fov << std::endl;
	std::cout << "ViewDir: " << m_viewDir << std::endl;
}


Ray
Camera::eyeRay(int x, int y, int imageWidth, int imageHeight)
{

    // first compute the camera coordinate system 
    // ------------------------------------------

    // wDir = e - (e+m_viewDir) = -m_vView
    const Vector3 wDir = Vector3(-m_viewDir).normalize(); 
    const Vector3 uDir = cross(m_up, wDir).normalize(); 
    const Vector3 vDir = cross(wDir, uDir);    

	const float focalDist = (m_lookAtOrig - m_eye).length();

    // next find the corners of the image plane in camera space
    // --------------------------------------------------------

    const float aspectRatio = (float)imageWidth/(float)imageHeight; 
	Vector3 xApertureRadius = uDir * 0.09f;
	Vector3 yApertureRadius = vDir * 0.09f;


    const float top     = tan(m_fov*HalfDegToRad); 
    const float right   = aspectRatio*top; 

    const float bottom  = -top; 
    const float left    = -right; 

	float xRan = ((float)rand() / (RAND_MAX));
	float yRan = ((float)rand() / (RAND_MAX));

	/*
	float xRanDof = ((float)rand() / (RAND_MAX));
	float yRanDof = ((float)rand() / (RAND_MAX));

	int temp0, temp1, temp2;
	temp0 = rand() % 2;
	temp1 = rand() % 2;

	if (temp0) xRanDof = -xRanDof;
	if (temp1) yRanDof = -yRanDof;
	*/



	//std::cout << "\nxRan: " << xRan << std::endl;
	//std::cout << "yRan: " << yRan << std::endl;



    // transform x and y into camera space 
    // -----------------------------------

	/*
    const float imPlaneUPos = left   + (right - left)*(((float)x+0.5f)/(float)imageWidth); 
    const float imPlaneVPos = bottom + (top - bottom)*(((float)y+0.5f)/(float)imageHeight); 
	*/

	
	const float imPlaneUPos = left + (right - left)*(((float)x + xRan) / (float)imageWidth);
	const float imPlaneVPos = bottom + (top - bottom)*(((float)y + yRan) / (float)imageHeight);
	
	//Vector3 newEye = m_eye + xRanDof* xApertureRadius + yRanDof* yApertureRadius;

    return Ray(m_eye, (imPlaneUPos*uDir + imPlaneVPos*vDir - wDir).normalize());
}
