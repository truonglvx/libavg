//
//  libavg - Media Playback Engine. 
//  Copyright (C) 2003-2014 Ulrich von Zadow
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

#ifndef _SoundNode_H_
#define _SoundNode_H_

#include "../api.h"

#include "AreaNode.h"

#include "../base/IFrameEndListener.h"
#include "../base/UTF8String.h"

namespace avg {

class AsyncVideoDecoder;

class AVG_API SoundNode : public AreaNode, IFrameEndListener
{
    public:
        static void registerType();

        SoundNode(const ArgList& args, const std::string& sPublisherName="Node");
        virtual ~SoundNode();

        virtual void connectDisplay();
        virtual void connect(CanvasPtr pCanvas);
        virtual void disconnect(bool bKill);

        void play();
        void stop();
        void pause();

        const UTF8String& getHRef() const;
        void setHRef(const UTF8String& href);
        float getVolume();
        void setVolume(float volume);
        void checkReload();

        long long getDuration() const;
        std::string getAudioCodec() const;
        int getAudioSampleRate() const;
        int getNumAudioChannels() const;

        long long getCurTime() const;
        void seekToTime(long long Time);
        bool getLoop() const;
        void setEOFCallback(PyObject * pEOFCallback);

        virtual void onFrameEnd();

    private:
        void seek(long long destTime);
        void onEOF();

        enum SoundState {Unloaded, Paused, Playing};
        void changeSoundState(SoundState newSoundState);
        void open();
        void startDecoding();
        void close();
        void exceptionIfUnloaded(const std::string& sFuncName) const;

        UTF8String m_href;
        std::string m_Filename;
        bool m_bLoop;
        PyObject * m_pEOFCallback;
        long long m_SeekBeforeCanRenderTime;

        long long m_StartTime;
        long long m_PauseTime;
        long long m_PauseStartTime;

        AsyncVideoDecoder* m_pDecoder;
        float m_Volume;
        SoundState m_State;
        int m_AudioID;
};

}

#endif 

