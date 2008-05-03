//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2006 Ulrich von Zadow
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

#include "RasterNode.h"

#include "NodeDefinition.h"
#include "DisplayEngine.h"

#include "../base/MathHelper.h"
#include "../base/Logger.h"
#include "../base/XMLHelper.h"
#include "../base/Exception.h"

using namespace std;

namespace avg {

NodeDefinition RasterNode::getNodeDefinition()
{
    return NodeDefinition("rasternode")
        .extendDefinition(Node::getNodeDefinition())
        .addArg(Arg<int>("maxtilewidth", -1, false, offsetof(RasterNode, m_MaxTileSize.x)))
        .addArg(Arg<int>("maxtileheight", -1, false, offsetof(RasterNode, m_MaxTileSize.y)))
        .addArg(Arg<string>("blendmode", "blend", false, offsetof(RasterNode, m_sBlendMode)));
}

RasterNode::RasterNode (Player * pPlayer)
    : Node(pPlayer),
      m_pSurface(0),
      m_MaxTileSize(IntPoint(-1,-1)),
      m_sBlendMode("blend")
{
}

RasterNode::~RasterNode()
{
    if (m_pSurface) {
        delete m_pSurface;
        m_pSurface = 0;
    }
}

void RasterNode::setArgs(const ArgList& Args)
{
    Node::setArgs(Args);
    if ((!ispow2(m_MaxTileSize.x) && m_MaxTileSize.x != -1)
            || (!ispow2(m_MaxTileSize.y) && m_MaxTileSize.y != -1)) 
    {
        throw Exception(AVG_ERR_OUT_OF_RANGE, 
                "maxtilewidth and maxtilehight must be powers of two.");
    }
    setBlendModeStr(m_sBlendMode);
}

void RasterNode::setRenderingEngines(DisplayEngine * pDisplayEngine, AudioEngine * pAudioEngine)
{
    Node::setRenderingEngines(pDisplayEngine, pAudioEngine);

    if (m_MaxTileSize != IntPoint(-1, -1)) {
        OGLSurface * pOGLSurface = 
            dynamic_cast<OGLSurface*>(getSurface());
        pOGLSurface->setMaxTileSize(m_MaxTileSize);
    }
    setBlendModeStr(m_sBlendMode);
}

void RasterNode::disconnect()
{
    if (m_pSurface) {
        delete m_pSurface;
        m_pSurface = 0;
    }
    Node::disconnect();
}

VertexGrid RasterNode::getOrigVertexCoords()
{
    OGLSurface * pOGLSurface = getOGLSurface();
    return pOGLSurface->getOrigVertexCoords();
}

VertexGrid RasterNode::getWarpedVertexCoords() 
{
    OGLSurface * pOGLSurface = getOGLSurface();
    return pOGLSurface->getWarpedVertexCoords();
}

void RasterNode::setWarpedVertexCoords(const VertexGrid& Grid)
{
    OGLSurface * pOGLSurface = getOGLSurface();
    pOGLSurface->setWarpedVertexCoords(Grid);
}

const std::string& RasterNode::getBlendModeStr() const
{
    return m_sBlendMode;
}

void RasterNode::setBlendModeStr(const std::string& sBlendMode)
{
    m_sBlendMode = sBlendMode;
    if (m_sBlendMode == "blend") {
        m_BlendMode = DisplayEngine::BLEND_BLEND;
    } else if (m_sBlendMode == "add") {
        m_BlendMode = DisplayEngine::BLEND_ADD;
    } else if (m_sBlendMode == "min") {
        m_BlendMode = DisplayEngine::BLEND_MIN;
    } else if (m_sBlendMode == "max") {
        m_BlendMode = DisplayEngine::BLEND_MAX;
    } else {
        // TODO: throw exception here
    }
}

NodePtr RasterNode::getElementByPos (const DPoint & pos)
{
    // Node isn't pickable if it's warped.
    if (m_MaxTileSize == IntPoint(-1, -1)) {
        return Node::getElementByPos(pos);
    } else {
        return NodePtr();
    }
}

Bitmap* RasterNode::getBitmap()
{
    Bitmap * pBmp = 0;
    if (m_pSurface) {
        pBmp = new Bitmap(*(m_pSurface->lockBmp()));
        m_pSurface->unlockBmps();
    }
    return pBmp;
}

/*
string RasterNode::getImageFormat()
{
    PixelFormat pf = m_pSurface->lockBmp()->getPixelFormat();
    m_pSurface->unlockBmps();
    switch(pf) {
        case B8G8R8:
            return "RGB";
        case B8G8R8A8:
            return "RGBA";
        case B8G8R8X8:
            return "RGBX";
        case A8B8G8R8:
            return "ABGR";
        case X8B8G8R8:
            return "XBGR";
        case R8G8B8:
            return "RGB";
        case R8G8B8A8:
            return "RGBA";
        case R8G8B8X8:
            return "RGBX";
        case A8R8G8B8:
            return "ARGB";
        case X8R8G8B8:
            return "XRGB";
        case I8:
            return "L";
        case YCbCr422:
            return "YCbCr";
        default:
            return Bitmap::getPixelFormatString(pf);
    }
}
*/

OGLSurface * RasterNode::getOGLSurface()
{
    OGLSurface * pOGLSurface = dynamic_cast<OGLSurface *>(getSurface());
    return pOGLSurface; 
}

DisplayEngine::BlendMode RasterNode::getBlendMode() const
{
    return m_BlendMode;
}

ISurface * RasterNode::getSurface()
{
    if (!m_pSurface) {
        m_pSurface = getDisplayEngine()->createSurface();
    }
    return m_pSurface;
}

string RasterNode::getTypeStr ()
{
    return "RasterNode";
}

}
