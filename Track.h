#ifndef TRACK_H
#define TRACK_H

#include "Resources.h"

struct TrackSegment {
    float x1, y1, x2, y2;
};

class Track {
private:
    std::vector<TrackSegment> segments;

public:
    Track();
    const std::vector<TrackSegment>& getSegments() const;
    float getYAtPosition(float x) const;
};

#endif 
