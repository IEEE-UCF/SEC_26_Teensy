/*
QuadEncoderHandler.h - Interfacing class for using the Quadrature Encoder library

Edit history
5/8/25 - Begin tracking - Aldem Pido
*/
#ifndef QUADENCODERHANDLER_h
#define QUADENCODERHANDLER_h

#include <Arduino.h>
#include "QuadEncoder.h"
#include <memory>
#include <algorithm>

class QuadEncoder; // Fwd declaration

/**
 * Setup for QuadEncoder. Workmode is 0 for normal use, 1 for FG and direction.
 */
struct QuadEncoderSetup
{
    int kENCA = -1;
    int kENCB = -1;
    int workMode = 0; // 0 for normal use, 1 for FG and direction
    bool rev = false;
    constexpr QuadEncoderSetup(int kA, int kB, int mode = 0, bool rev = false)
        : kENCA(kA), kENCB(kB), workMode(mode), rev(rev) {}
};

class QuadEncoderHandler
{
public:
    explicit QuadEncoderHandler(const QuadEncoderSetup &config, Print &output);
    virtual ~QuadEncoderHandler() = default;
    void Begin();
    void UpdateEnc();
    long GetEnc();
    void PrintInfo(Print &output, bool printConfig = false) const;
    friend Print &operator<<(Print &output, const QuadEncoderHandler &enc);

private:
    const QuadEncoderSetup &config;
    Print &output;
    std::unique_ptr<QuadEncoder> encoder;
    long enc;
    static int encoderNum;
};

#endif