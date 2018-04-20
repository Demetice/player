#include "audio_decoder.h"
#include "ffmpeg.h"

audio_decoder::audio_decoder(AVCodecParameters * codec_parameters)
{
//	avcodec_register_all();
	const auto codec = avcodec_find_decoder(codec_parameters->codec_id);
	if (!codec) {
		throw ffmpeg::Error{ "Unsupported audio codec" };
	}
	codec_context_ = avcodec_alloc_context3(codec);
	if (!codec_context_) {
		throw ffmpeg::Error{ "Couldn't allocate video codec context" };
	}
	ffmpeg::check(avcodec_parameters_to_context(
		codec_context_, codec_parameters));
	
	parser_ = av_parser_init(codec->id);
	if (!parser_) {
		fprintf(stderr, "Parser not found\n");
		exit(1);
	}

	ffmpeg::check(avcodec_open2(codec_context_, codec, nullptr));
}

audio_decoder::~audio_decoder()
{
	avcodec_free_context(&codec_context_);
}

bool audio_decoder::send(AVPacket * packet)
{
	auto ret = avcodec_send_packet(codec_context_, packet);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
		return false;
	}
	else {
		ffmpeg::check(ret);
		return true;
	}
}

bool audio_decoder::receive(AVFrame * frame)
{
	auto ret = avcodec_receive_frame(codec_context_, frame);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
		return false;
	}
	else {
		ffmpeg::check(ret);
		return true;
	}
}
