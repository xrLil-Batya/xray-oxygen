/*********************************************************
* Copyright (C) X-Ray Oxygen, 2018. All rights reserved.
* X-Ray Oxygen - open-source X-Ray fork
* Apache License
**********************************************************
* Module Name: XAudio Target
**********************************************************
* XAudioTarget.h
* Main methods for XAudio implementation
*********************************************************/
#pragma once

#include "stdafx.h"

class CSoundRender_TargetB : public CSoundRender_Target
{
	typedef CSoundRender_Target	inherited;

	// XAudio2
	LPVOID pSource;
	LPVOID pBuffers[sdef_target_count];
	float GainCache;

	u32 BufBlock;

private:
	void fill_block(u32 BufferID);

public:
	CSoundRender_TargetB();
	virtual ~CSoundRender_TargetB();

	//void source_changed();

	virtual bool _initialize();
	virtual void _destroy();
	virtual void _restart();

	virtual void start(CSoundRender_Emitter* E);
	virtual void render();
	virtual void rewind();
	virtual void stop();
	virtual void update();
	virtual void fill_parameters();
	virtual	void alAuxInit(u32 slot);
};
