//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2011 Ulrich von Zadow
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Current versions can be found at www.libavg.de
//

#include "GPUBandpassFilter.h"
#include "Bitmap.h"
#include "ShaderRegistry.h"

#include "../base/ObjectCounter.h"
#include "../base/Exception.h"

#include <iostream>

#define SHADERID "bandpass"

using namespace std;

namespace avg {

GPUBandpassFilter::GPUBandpassFilter(const IntPoint& size, PixelFormat pfSrc, 
        float min, float max, float postScale, bool bInvert, bool bStandalone)
    : GPUFilter(pfSrc, B8G8R8A8, bStandalone),
      m_PostScale(postScale),
      m_bInvert(bInvert),
      m_MinFilter(size, pfSrc, R32G32B32A32F, min, true, false),
      m_MaxFilter(size, pfSrc, R32G32B32A32F, max, true, false)
{
    ObjectCounter::get()->incRef(&typeid(*this));
    setDimensions(size);
    initShader();
}

GPUBandpassFilter::~GPUBandpassFilter()
{
    ObjectCounter::get()->decRef(&typeid(*this));
}

void GPUBandpassFilter::applyOnGPU(GLTexturePtr pSrcTex)
{
    m_MinFilter.apply(pSrcTex);
    m_MaxFilter.apply(pSrcTex);

    getFBO()->activate();
    OGLShaderPtr pShader = getShader(SHADERID);
    pShader->activate();
    pShader->setUniformIntParam("minTex", 0);
    pShader->setUniformIntParam("maxTex", 1);
    pShader->setUniformFloatParam("postScale", float(m_PostScale));
    pShader->setUniformIntParam("bInvert", m_bInvert);
    m_MaxFilter.getDestTex()->activate(GL_TEXTURE1);
    draw(m_MinFilter.getDestTex());

    glproc::UseProgramObject(0);
}

void GPUBandpassFilter::initShader()
{
    createShader(SHADERID);
}

}
