#include "audio_decoder.h"
#include "ffmpeg.h"

AudioDecoder::AudioDecoder(AVCodecParameters * codec_parameters)
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

	out_pcm_file_ = fopen("out.pcm", "w+");
}

void AudioDecoder::save_auido_to_pcm(AVFrame *frame)
{
	int data_size = av_get_bytes_per_sample(codec_context_->sample_fmt);
	if (data_size < 0) {
		/* This should not occur, checking just for paranoia */
		fprintf(stderr, "Failed to calculate data size\n");
		throw std::logic_error{ "Failed to calculate data size" };
	}
	for (int i = 0; i < frame->nb_samples; i++)
		for (int ch = 0; ch < codec_context_->channels; ch++)
			fwrite(frame->data[ch] + data_size * i, 1, data_size, out_pcm_file_);
}

AudioDecoder::~AudioDecoder()
{
	avcodec_free_context(&codec_context_);
	fclose(out_pcm_file_);
}

bool AudioDecoder::send(AVPacket * packet)
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

bool AudioDecoder::receive(AVFrame * frame)
{
	auto ret = avcodec_receive_frame(codec_context_, frame);
	if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
		return false;
	}
	else {
		save_auido_to_pcm(frame);
		ffmpeg::check(ret);
		return true;
	}
}
