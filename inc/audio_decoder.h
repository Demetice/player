#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
}

class audio_decoder
{
public:
	audio_decoder(AVCodecParameters* codec_parameters);
	~audio_decoder();
	bool send(AVPacket* packet);
	bool receive(AVFrame* frame);

private:
	AVCodecContext * codec_context_{};
	AVCodecParserContext *parser_{};
};

