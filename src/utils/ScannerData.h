#pragma once

class ScannerData{
    public:
        int co2; // In ppm
        float humidity; // In percentage %
        float temperature; // In degrees
        float light; // In percentage

        ScannerData(int co2,float humidity,float temperature,float light);
};