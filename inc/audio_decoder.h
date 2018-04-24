#pragma once

extern "C" {
#include "libavcodec/avcodec.h"
}

class AudioDecoder
{
public:
	AudioDecoder(AVCodecParameters* codec_parameters);
	~AudioDecoder();
	bool send(AVPacket* packet);
	bool receive(AVFrame* frame);
	void save_auido_to_pcm(AVFrame *frame);

private:
	AVCodecContext * codec_context_{};
	AVCodecParserContext *parser_{};
	FILE *out_pcm_file_{};
};

