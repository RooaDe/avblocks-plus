#include <primo/avblocks/avb++.h>

#include "util.h"
#include "options.h"

using namespace primo::avblocks::modern;
using namespace std;

namespace pc = primo::codecs;

bool decode(Options &opt)
{
    // transcoder will fail if output exists (by design)
    deleteFile(primo::ustring(opt.outputFile));

    try {
        // Create decoder transcoder - input from file, output PCM without file
        Transcoder decoder;
        decoder.allowDemoMode(true)
            .addInput(
                MediaSocket()
                    .file(opt.inputFile)
            )
            .addOutput(
                MediaSocket()
                    .streamType(pc::StreamType::LPCM)
                    .addPin(
                        MediaPin()
                            .audioStreamType(pc::StreamType::LPCM)
                            .channels(2)
                            .sampleRate(48000)
                            .bitsPerSample(16)
                    )
            )
            .open();

        // Create WAV writer transcoder - input PCM without file, output to file
        Transcoder wavWriter;
        wavWriter.allowDemoMode(true)
            .addInput(
                MediaSocket()
                    .streamType(pc::StreamType::LPCM)
                    .addPin(
                        MediaPin()
                            .audioStreamType(pc::StreamType::LPCM)
                            .channels(2)
                            .sampleRate(48000)
                            .bitsPerSample(16)
                    )
            )
            .addOutput(
                MediaSocket()
                    .file(opt.outputFile)
                    .streamType(pc::StreamType::WAVE)
                    .addPin(
                        MediaPin()
                            .audioStreamType(pc::StreamType::LPCM)
                            .channels(2)
                            .sampleRate(48000)
                            .bitsPerSample(16)
                    )
            )
            .open();

        // Pull-push decoding loop
        int32_t decoderOutputIndex = 0;
        MediaSample pcmSample;

        bool decoderEos = false;
        while (!decoderEos) {
            // Pull PCM sample from decoder
            if (decoder.pull(decoderOutputIndex, pcmSample)) {
                // Push PCM sample to WAV writer
                if (!wavWriter.push(0, pcmSample)) {
                    printError("WAV Writer push", wavWriter.error());
                    return false;
                }
                continue;
            }

            // No more PCM data from decoder
            const auto* error = decoder.error();
            if (error->facility() == primo::error::ErrorFacility::Codec &&
                error->code() == pc::CodecError::EOS) {
                // Push null sample to signal EOS to WAV writer
                MediaSample nullSample;
                wavWriter.push(0, nullSample);
                decoderEos = true;
                continue;
            }

            printError("Decoder pull", error);
            return false;
        }

        decoder.close();
        wavWriter.close();

        return true;
    }
    catch (const AVBlocksException& ex) {
        cout << "AVBlocks error: " << ex.what() << endl;
        return false;
    }
}

int main(int argc, char *argv[])
{
    Options opt;

    switch (prepareOptions(opt, argc, argv))
    {
    case Command:
        return 0;
    case Error:
        return 1;
    case Parsed:
        break;
    }

    Library library;
    bool result = decode(opt);
    return result ? 0 : 1;
}