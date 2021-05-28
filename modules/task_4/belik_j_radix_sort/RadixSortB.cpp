// Copyright 2021 Belik Julia
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <iterator>
#include <utility>
#include <iostream>
#include "../../../modules/task_4/belik_j_radix_sort/RadixSortB.h"
#include "../../../3rdparty/unapproved/unapproved.h"
void RadixSort(double* vec, size_t len, double* vec2) {
    double* vec1 = new double[len];
    for (size_t i = 0; i < len; i++)
        vec1[i] = vec[i];
    for (size_t i = 0; i < 7; i++) {
        unsigned char* bvec = (unsigned char*)vec1;
        int countb[256] = { 0 };
        int offset[256] = { 0 };
        for (size_t j = 0; j < len; j++)
            countb[bvec[8 * j + i]]++;
        offset[0] = 0;
        for (int j = 1; j < 256; j++)
            offset[j] = offset[j - 1] + countb[j - 1];
        for (size_t j = 0; j < len; j++)
            vec2[offset[bvec[8 * j + i]]++] = vec1[j];
        for (size_t i = 0; i < len; i++)
            vec1[i] = vec2[i];
    }
    unsigned char* bvec = (unsigned char*)vec1;
    int countb[256] = { 0 };
    int offset[256] = { 0 };
    for (size_t i = 0; i < len; i++)
        countb[bvec[8 * i + 7]]++;
    offset[255] = offset[255] - 1;
    for (int i = 254; i >= 128; i--)
        offset[i] = offset[i + 1] + countb[i];
    offset[0] = offset[128] + 1;
    for (int i = 1; i < 128; i++)
        offset[i] = offset[i - 1] + countb[i - 1];
    for (size_t i = 0; i < len; i++) {
        if (bvec[8 * i + 7] >= 128) {
            vec2[offset[bvec[8 * i + 7]]--] = vec1[i];
        } else {
            vec2[offset[bvec[8 * i + 7]]++] = vec1[i];
        }
    }
    delete[] vec1;
}
std::vector<double> Vector(size_t n, double a, double b) {
    std::mt19937 gen;
    gen.seed(static_cast<unsigned int>(time(0)));
    std::uniform_real_distribution<double> rand(a, b);
    std::vector<double> vec(n);
    for (size_t i = 0; i < n; i++)
        vec[i] = rand(gen);
    return vec;
}
std::vector<double> MergeBatcherPar(std::vector<double> vec, size_t size, int thr) {
    const size_t len = size / thr;
    size_t* lens = new size_t[thr];
    double* vectmp = new double[size];
    for (size_t j = 0; j < size; j++)
        vectmp[j] = vec[j];
    for (int j = 0; j < thr; j++)
        lens[j] = len;
    lens[thr - 1] += size % thr;
    size_t* offsets = new size_t[thr];
    offsets[0] = 0;
    for (int j = 1; j < thr; j++)
        offsets[j] = offsets[j - 1] + lens[j - 1];
    std::vector<double> rez(size);
    double* reztmp = new double[size];
    if (thr == 1) {
        RadixSort(vectmp, size, reztmp);
        for (size_t j = 0; j < size; j++)
            rez[j] = reztmp[j];
        delete[] lens;
        delete[] offsets;
        delete[] vectmp;
        delete[] reztmp;
        return rez;
    }
    std::vector<std::thread> threadssort;
    for (int i = 0; i < thr; i++) {
        threadssort.push_back(std::thread([i, &vectmp, &offsets, &lens, &reztmp]() {
            RadixSort(vectmp + offsets[i], lens[i], reztmp + offsets[i]);
            Shuffle(reztmp + offsets[i], lens[i], vectmp + offsets[i]);
        }));
    }
    for (auto& t : threadssort)
        t.join();
    int nt = thr;
    int mergecount = 2, offset = 1;
    int nummerge = 1, pow = 2;
    while (pow < thr) {
        pow *= 2;
        nummerge++;
    }
    for (int h = 0; h < nummerge; h++) {
        std::vector<std::thread> threadsmp;
        for (int i = 0; i < thr; i++) {
            threadsmp.push_back(std::thread([i, &vectmp, &offsets, &lens, &reztmp, mergecount, offset, thr]() {
                if ((i % mergecount == 0) && (i + offset < thr)) {
                    size_t len1 = lens[i] / 2 + lens[i] % 2;
                    size_t len2 = lens[i + offset] / 2 + lens[i + offset] % 2;
                    PMerge(vectmp + offsets[i], vectmp + offsets[i + offset], reztmp + offsets[i], len1, len2);
                }
                if ((i - offset >= 0) && ((i - offset) % mergecount == 0)) {
                    size_t start3 = offsets[i - offset] + lens[i - offset] / 2 + lens[i - offset] % 2
                        + lens[i] / 2 + lens[i] % 2;
                    size_t start1 = offsets[i - offset] + lens[i - offset] / 2 + lens[i - offset] % 2;
                    size_t start2 = offsets[i] + lens[i] / 2 + lens[i] % 2;
                    PMerge(vectmp + start1, vectmp + start2, reztmp + start3, lens[i - offset] / 2, lens[i] / 2);
                }
            }));
        }
        for (auto& t : threadsmp)
            t.join();
        std::vector<std::thread> threadsm;
        for (int i = 0; i < thr; i++) {
            threadsm.push_back(std::thread([i, &vectmp, &offsets, &lens,
                &reztmp, mergecount, offset, nummerge, thr, h]() {
                if ((i % mergecount == 0) && (i + offset < thr)) {
                    size_t evencount = lens[i] / 2 + lens[i + offset] / 2 + lens[i] % 2 + lens[i + offset] % 2;
                    size_t oddcount = lens[i] / 2 + lens[i + offset] / 2;
                    PMerge(reztmp + offsets[i], reztmp + offsets[i] + evencount,
                        vectmp + offsets[i], evencount, oddcount);
                    if (h != nummerge - 1) {
                        Shuffle(vectmp + offsets[i], lens[i] + lens[i + offset], reztmp + offsets[i]);
                    }
                    lens[i] += lens[i + offset];
                    lens[i + offset] = 0;
                }
            }));
        }
        for (auto& t : threadsm)
            t.join();
        if (h != nummerge - 1) {
            for (size_t j = 0; j < size; j++)
                vectmp[j] = reztmp[j];
        }
        nt /= 2;
        mergecount *= 2;
        offset *= 2;
        for (int j = 1; j < thr; j++)
            offsets[j] = offsets[j - 1] + lens[j - 1];
    }
    delete[] lens;
    delete[] offsets;
    for (size_t j = 0; j < size; j++)
        vec[j] = vectmp[j];
    delete[] vectmp;
    delete[] reztmp;
    return vec;
}
std::vector<double> MergeBatcherSeq(std::vector<double> vec, size_t size, int thr) {
    const size_t len = size / thr;
    size_t* lens = new size_t[thr];
    double* vectmp = new double[size];
    for (size_t j = 0; j < size; j++)
        vectmp[j] = vec[j];
    for (int j = 0; j < thr; j++)
        lens[j] = len;
    lens[thr - 1] += size % thr;
    size_t* offsets = new size_t[thr];
    offsets[0] = 0;
    for (int j = 1; j < thr; j++)
        offsets[j] = offsets[j - 1] + lens[j - 1];
    std::vector<double> rez(size);
    double* reztmp = new double[size];
    if (thr == 1) {
        RadixSort(vectmp, size, reztmp);
        for (size_t j = 0; j < size; j++)
            rez[j] = reztmp[j];
        delete[] lens;
        delete[] offsets;
        delete[] vectmp;
        delete[] reztmp;
        return rez;
    }
    for (int i = 0; i < thr; i++) {
        RadixSort(vectmp + offsets[i], lens[i], reztmp + offsets[i]);
        Shuffle(reztmp + offsets[i], lens[i], vectmp + offsets[i]);
    }
    int nt = thr;
    int mergecount = 2, offset = 1;
    int nummerge = 1, pow = 2;
    while (pow < thr) {
        pow *= 2;
        nummerge++;
    }
    for (int h = 0; h < nummerge; h++) {
        for (int i = 0; i < thr; i++) {
            if ((i % mergecount == 0) && (i + offset < thr)) {
                size_t len1 = lens[i] / 2 + lens[i] % 2;
                size_t len2 = lens[i + offset] / 2 + lens[i + offset] % 2;
                PMerge(vectmp + offsets[i], vectmp + offsets[i + offset], reztmp + offsets[i], len1, len2);
            }
            if ((i - offset >= 0) && ((i - offset) % mergecount == 0)) {
                size_t start3 = offsets[i - offset] + lens[i - offset] / 2
                    + lens[i - offset] % 2 + lens[i] / 2 + lens[i] % 2;
                size_t start1 = offsets[i - offset] + lens[i - offset] / 2 + lens[i - offset] % 2;
                size_t start2 = offsets[i] + lens[i] / 2 + lens[i] % 2;
                PMerge(vectmp + start1, vectmp + start2, reztmp + start3, lens[i - offset] / 2, lens[i] / 2);
            }
        }
        for (int i = 0; i < thr; i++) {
            if ((i % mergecount == 0) && (i + offset < thr)) {
                size_t evencount = lens[i] / 2 + lens[i + offset] / 2 + lens[i] % 2 + lens[i + offset] % 2;
                size_t oddcount = lens[i] / 2 + lens[i + offset] / 2;
                PMerge(reztmp + offsets[i], reztmp + offsets[i] + evencount, vectmp + offsets[i], evencount, oddcount);
                if (h != nummerge - 1) {
                    Shuffle(vectmp + offsets[i], lens[i] + lens[i + offset], reztmp + offsets[i]);
                }
                lens[i] += lens[i + offset];
                lens[i + offset] = 0;
            }
        }
        if (h != nummerge - 1) {
            for (size_t j = 0; j < size; j++)
                vectmp[j] = reztmp[j];
        }
        nt /= 2;
        mergecount *= 2;
        offset *= 2;
        for (int j = 1; j < thr; j++)
            offsets[j] = offsets[j - 1] + lens[j - 1];
    }
    delete[] lens;
    delete[] offsets;
    for (size_t j = 0; j < size; j++)
        vec[j] = vectmp[j];
    delete[] vectmp;
    delete[] reztmp;
    return vec;
}
void Shuffle(double* vec, size_t len, double* vec2) {
    for (size_t i = 0; i < len; i++)
        vec2[i / 2 + (i % 2) * (len / 2 + len % 2)] = vec[i];
}
void PMerge(double* start1, double* start2, double* start3, size_t len1, size_t len2) {
    size_t i = 0, j = 0, k = 0;
    while ((i < len1) && (j < len2)) {
        if (start1[i] < start2[j])
            start3[k++] = start1[i++];
        else
            start3[k++] = start2[j++];
    }
    while (i < len1)
        start3[k++] = start1[i++];
    while (j < len2)
        start3[k++] = start2[j++];
}