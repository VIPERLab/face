/****************************************************************************
*                                                                           *
*   IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.       *
*                                                                           *
*   By downloading, copying, installing or using the software you agree to  *
*   this license. If you do not agree to this license, do not download,     *
*   install, copy or use the software.                                      *
*                                                                           *
*                           License Agreement                               *
*                   For Vision Open Statistical Models                      *
*                                                                           *
*   Copyright (C):      2006~2012 by JIA Pei, all rights reserved.          *
*                                                                           *
*   VOSM is free software under the terms of the GNU Lesser General Public  *
*   License (GNU LGPL) as published by the Free Software Foundation; either *
*   version 3.0 of the License, or (at your option) any later version.      *
*   You can use it, modify it, redistribute it, etc; and redistribution and *
*   use in source and binary forms, with or without modification, are       *
*   permitted provided that the following conditions are met:               *
*                                                                           *
*   a) Redistribution's of source code must retain this whole paragraph of  *
*   copyright notice, including this list of conditions and all the         *
*   following contents in this  copyright paragraph.                        *
*                                                                           *
*   b) Redistribution's in binary form must reproduce this whole paragraph  *
*   of copyright notice, including this list of conditions and all the      *
*   following contents in this copyright paragraph, and/or other materials  *
*   provided with the distribution.                                         *
*                                                                           *
*   c) The name of the copyright holders may not be used to endorse or      *
*   promote products derived from this software without specific prior      *
*   written permission.                                                     *
*                                                                           *
*   Any publications based on this code must cite the following five papers,*
*   technical reports and on-line materials.                                *
*   1) P. JIA, 2D Statistical Models, Technical Report of Vision Open       *
*   Working Group, 2st Edition, October 21, 2010.                           *
*   http://www.visionopen.com/members/jiapei/publications/pei_sm2dreport2010.pdf*
*   2) P. JIA. Audio-visual based HMI for an Intelligent Wheelchair.        *
*   PhD thesis, University of Essex, February, 2011.                        *
*   http://www.visionopen.com/members/jiapei/publications/pei_phdthesis2010.pdf*
*   3) T. Cootes and C. Taylor. Statistical models of appearance for        *
*   computer vision. Technical report, Imaging Science and Biomedical       *
*   Engineering, University of Manchester, March 8, 2004.                   *
*   http://www.isbe.man.ac.uk/~bim/Models/app_models.pdf                    *
*   4) I. Matthews and S. Baker. Active appearance models revisited.        *
*   International Journal of Computer Vision, 60(2):135--164, November 2004.*
*   http://www.ri.cmu.edu/pub_files/pub4/matthews_iain_2004_2/matthews_iain_2004_2.pdf*
*   5) M. B. Stegmann, Active Appearance Models: Theory, Extensions and     *
*   Cases, 2000.                                                            *
*   http://www2.imm.dtu.dk/~aam/main/                                       *
*                                                                           *
* Version:          0.4                                                     *
* Author:           JIA Pei                                                 *
* Contact:          jp4work@gmail.com                                       *
* URL:              http://www.visionopen.com                               *
* Create Date:      2008-04-03                                              *
* Revise Date:      2012-03-22                                              *
*****************************************************************************/


#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <boost/algorithm/string.hpp>

#include "VO_FaceParts.h"


ostream& operator<<(ostream& os, const VO_FaceParts& faceparts)
{
    os << "#FaceParts" << endl;
    os << faceparts.m_vChin << endl;
    os << faceparts.m_vLeftEyebrow << endl;
    os << faceparts.m_vRightEyebrow << endl;
    os << faceparts.m_vLeftEye << endl;
    os << faceparts.m_vRightEye << endl;
    os << faceparts.m_vNose << endl;
    os << faceparts.m_vNostril << endl;
    os << faceparts.m_vNoseTip << endl;
    os << faceparts.m_vLipOuterLine << endl;
    os << faceparts.m_vLipInnerLine << endl;
    os << faceparts.m_vLipUpperOuterLine << endl;
    os << faceparts.m_vLipUpperInnerLine << endl;
    os << faceparts.m_vLipLowerOuterLine << endl;
    os << faceparts.m_vLipLowerInnerLine << endl;
    os << faceparts.m_vLeftSidePoints << endl;
    os << faceparts.m_vRightSidePoints << endl;
    os << faceparts.m_vMidlinePoints << endl;
    os << faceparts.m_vEyeCornerPoints << endl;
    os << faceparts.m_vMouthCornerPoints << endl;
    os << faceparts.m_vPitchAxisLinePoints << endl;
    os << faceparts.m_vLeftIris << endl;
    os << faceparts.m_vRightIris << endl;
    os << faceparts.m_vLeftEar << endl;
    os << faceparts.m_vRightEar << endl;

    return os;
}


istream& operator>>(istream& is, VO_FaceParts& faceparts)
{
    VO_FacePart onePart;
    string line;
    getline(is, line);

    bool isContainTriangleIndex = false;
    
    while ( getline(is, line) )
    {
        boost::trim(line);
        if(line == "#FaceParts")
            break;
    }
    while ( getline(is, line) )
    {
        cout << line << endl;
        if(line == "TriangleIndexes:")
        {
            isContainTriangleIndex = true;
            break;
        }
        istringstream iss(line);
        iss >> onePart;

        faceparts.VO_SetOneFacePart(onePart);
        onePart.clear();
    }
    if(isContainTriangleIndex)
    {
        int size = 0;
        is >> line;
        istringstream iss(line);
        iss >> size;
    }

    return is;
}


/**
 */
VO_FacePart VO_FaceParts::VO_GetOneFacePart(unsigned int partIdx) const
{
    switch(partIdx )
    {
    case VO_FacePart::WHOLEFACE:
        break;
    case VO_FacePart::CHIN:
        return this->m_vChin;
    case VO_FacePart::LEFTEYEBROW:
        return this->m_vLeftEyebrow;
    case VO_FacePart::RIGHTEYEBROW:
        return this->m_vRightEyebrow;
    case VO_FacePart::LEFTEYE:
        return this->m_vLeftEye;
    case VO_FacePart::RIGHTEYE:
        return this->m_vRightEye;
    case VO_FacePart::NOSE:
        return this->m_vNose;
    case VO_FacePart::NOSTRIL:
        return this->m_vNostril;
    case VO_FacePart::NOSETIP:
        return this->m_vNoseTip;
    case VO_FacePart::LIPOUTERLINE:
        return this->m_vLipOuterLine;
    case VO_FacePart::LIPINNERLINE:
        return this->m_vLipInnerLine;
    case VO_FacePart::LIPUPPEROUTERLINE:
        return this->m_vLipUpperOuterLine;
    case VO_FacePart::LIPUPPERINNERLINE:
        return this->m_vLipUpperInnerLine;
    case VO_FacePart::LIPDOWNEROUTERLINE:
        return this->m_vLipLowerOuterLine;
    case VO_FacePart::LIPDOWNERINNERLINE:
        return this->m_vLipLowerInnerLine;
    case VO_FacePart::LEFTSIDEPOINTS:
        return this->m_vLeftSidePoints;
    case VO_FacePart::RIGHTSIDEPOINTS:
        return this->m_vRightSidePoints;
    case VO_FacePart::MIDLINEPOINTS:
        return this->m_vMidlinePoints;
    case VO_FacePart::EYECORNERPOINTS:
        return this->m_vEyeCornerPoints;
    case VO_FacePart::MOUTHCORNERPOINTS:
        return this->m_vMouthCornerPoints;
    case VO_FacePart::PITCHAXISLINEPOINTS:
        return this->m_vPitchAxisLinePoints;
    case VO_FacePart::LEFTIRIS:
        return this->m_vLeftIris;
    case VO_FacePart::RIGHTIRIS:
        return this->m_vRightIris;
    case VO_FacePart::LEFTEAR:
        return this->m_vLeftEar;
    case VO_FacePart::RIGHTEAR:
        return this->m_vRightEar;
    case VO_FacePart::WHOLEMOUTH:
        break;
    case VO_FacePart::NOSECENTRALAREA:
        break;
    }
}


/**
 */ 
void VO_FaceParts::VO_SetOneFacePart(const VO_FacePart& iFacePart)
{
    switch(iFacePart.GetType() )
    {
    case VO_FacePart::CHIN:
        this->m_vChin = iFacePart;
        break;
    case VO_FacePart::LEFTEYEBROW:
        this->m_vLeftEyebrow = iFacePart;
        break;
    case VO_FacePart::RIGHTEYEBROW:
        this->m_vRightEyebrow = iFacePart;
        break;
    case VO_FacePart::LEFTEYE:
        this->m_vLeftEye = iFacePart;
        break;
    case VO_FacePart::RIGHTEYE:
        this->m_vRightEye = iFacePart;
        break;
    case VO_FacePart::NOSE:
        this->m_vNose = iFacePart;
        break;
    case VO_FacePart::NOSTRIL:
        this->m_vNostril = iFacePart;
        break;
    case VO_FacePart::NOSETIP:
        this->m_vNoseTip = iFacePart;
        break;
    case VO_FacePart::LIPOUTERLINE:
        this->m_vLipOuterLine = iFacePart;
        break;
    case VO_FacePart::LIPINNERLINE:
        this->m_vLipInnerLine = iFacePart;
        break;
    case VO_FacePart::LIPUPPEROUTERLINE:
        this->m_vLipUpperOuterLine = iFacePart;
        break;
    case VO_FacePart::LIPUPPERINNERLINE:
        this->m_vLipUpperInnerLine = iFacePart;
        break;
    case VO_FacePart::LIPDOWNEROUTERLINE:
        this->m_vLipLowerOuterLine = iFacePart;
        break;
    case VO_FacePart::LIPDOWNERINNERLINE:
        this->m_vLipLowerInnerLine = iFacePart;
        break;
    case VO_FacePart::LEFTSIDEPOINTS:
        this->m_vLeftSidePoints = iFacePart;
        break;
    case VO_FacePart::RIGHTSIDEPOINTS:
        this->m_vRightSidePoints = iFacePart;
        break;
    case VO_FacePart::MIDLINEPOINTS:
        this->m_vMidlinePoints = iFacePart;
        break;
    case VO_FacePart::EYECORNERPOINTS:
        this->m_vEyeCornerPoints = iFacePart;
        break;
    case VO_FacePart::MOUTHCORNERPOINTS:
        this->m_vMouthCornerPoints = iFacePart;
        break;
    case VO_FacePart::PITCHAXISLINEPOINTS:
        this->m_vPitchAxisLinePoints = iFacePart;
        break;
    case VO_FacePart::LEFTIRIS:
        this->m_vLeftIris = iFacePart;
        break;
    case VO_FacePart::RIGHTIRIS:
        this->m_vRightIris = iFacePart;
        break;
    case VO_FacePart::LEFTEAR:
        this->m_vLeftEar = iFacePart;
        break;
    case VO_FacePart::RIGHTEAR:
        this->m_vRightEar = iFacePart;
        break;
    }
}

