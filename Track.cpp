#include "Track.h"

Track::Track() {
    std::vector<float> controlPoints;
    const int numPoints = 200;
    const int segmentLength = 50;

    float prevY = SCREEN_H * 3 / 4;
    controlPoints.push_back(prevY);

    // controlled randomness
    for (int i = 1; i < numPoints; ++i) {
        float offset = (rand() % 60 - 30);
        float newY = clamp(controlPoints.back() + offset, 100.0f, (float)SCREEN_H - 100);
        controlPoints.push_back(newY);
    }

    for (int i = 0; i < (int)controlPoints.size() - 1; ++i) {
        float x_start = i * segmentLength;
        float y1 = controlPoints[i];
        float y2 = controlPoints[i + 1];

        for (int j = 0; j < segmentLength; ++j) {
            float t = (float)j / segmentLength;
            float t2 = (1 - cos(t * ALLEGRO_PI)) / 2; 
            float y = y1 * (1 - t2) + y2 * t2;

            float x1 = x_start + j;
            float x2 = x_start + j + 1;

            float next_t = (float)(j + 1) / segmentLength;
            float next_t2 = (1 - cos(next_t * ALLEGRO_PI)) / 2;
            float y_next = y1 * (1 - next_t2) + y2 * next_t2;

            segments.push_back({ x1, y, x2, y_next });
        }
    }
}

const std::vector<TrackSegment>& Track::getSegments() const {
    return segments;
}

float Track::getYAtPosition(float x) const {
    for (const auto& segment : segments) {
        if (x >= segment.x1 && x <= segment.x2) {
            float t = (x - segment.x1) / (segment.x2 - segment.x1);
            return segment.y1 + t * (segment.y2 - segment.y1);
        }
    }
    return SCREEN_H; 
}
