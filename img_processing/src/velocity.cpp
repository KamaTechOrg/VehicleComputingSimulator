#include "velocity.h"

using namespace std;
using namespace cv;

void Velocity::init(double frameTimeDiff)
{
    this->frameTimeDiff = frameTimeDiff;
}
void Velocity::returnVelocities(vector<ObjectInformation> &objects)
{
    for (auto &object : objects) {
        calculateVelocity(object);
    }
}
void Velocity::calculateVelocity(ObjectInformation &object)
{
    float distanceAvg = averageDistanceChange(object);
    if (distanceAvg != -1) {
        float velocity = distanceAvg / this->frameTimeDiff;
        updateVelocity(velocity, object);
    }
}

float Velocity::averageDistanceChange(ObjectInformation obj) const
{
    if (obj.prevDistances.size() < 2)
        return -1;
    float totalChange = 0.0;
    for (size_t i = 1; i < obj.prevDistances.size(); ++i) {
        totalChange += (obj.prevDistances[i] - obj.prevDistances[i - 1]);
    }
    return totalChange / (obj.prevDistances.size() - 1);
}

//Function to update velocity while maintaining the last two velocities
void Velocity::updateVelocity(float newVelocity, ObjectInformation &obj)
{
    // If we have at least one previous velocity
    if (!obj.prevVelocities.empty()) {
        float lastVelocity =
            obj.prevVelocities.back();  // Get the last velocity
        // Check if the sign of the new velocity matches the last one
        if ((newVelocity >= 0 && lastVelocity >= 0) ||
            (newVelocity < 0 && lastVelocity < 0)) {
            // If the signs are the same, we accept the new velocity as valid
            obj.velocity = newVelocity;
        }
        else {
            // If the signs are different, we need to investigate further
            if (obj.prevVelocities.size() > 1) {
                // We look at the second-last velocity to check for consistency
                float secondLastVelocity =
                    obj.prevVelocities[obj.prevVelocities.size() - 2];

                // Check if the new velocity matches the sign of the second-last velocity
                if ((newVelocity >= 0 && secondLastVelocity >= 0) ||
                    (newVelocity < 0 && secondLastVelocity < 0)) {
                    // If the new velocity's sign matches the second-last velocity, we accept it
                    obj.velocity = newVelocity;
                }
                else {
                    // If the signs don’t match either, we keep the last velocity
                    obj.velocity = lastVelocity;
                }
            }
            else {
                // If there's only one previous velocity, we keep it as it’s unclear if the new one is valid
                obj.velocity = lastVelocity;
            }
        }
    }
    else {
        // If there are no previous velocities, just accept the new velocity
        obj.velocity = newVelocity;
    }
    // Add the new velocity to the deque for history tracking
    obj.prevVelocities.push_back(newVelocity);
    // Keep only the last two velocities in the deque
    if (obj.prevVelocities.size() > MAX_PREV_VELOCITIES_SIZE) {
        obj.prevVelocities
            .pop_front();  // Remove the oldest velocity if the deque exceeds the limit
    }
}

void Velocity::drawVelocity(std::shared_ptr<Mat> image,
                            std::vector<ObjectInformation> &objects)
{
    // Font properties
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.6;
    int thickness = 2;
    int baseline = 0;

    // Calculate text sizes
    Size velocityTextSize =
        getTextSize("velocity", fontFace, fontScale, thickness, &baseline);

    for (auto &obj : objects) {
        // Check if velocity has a value
        if (obj.velocity.has_value()) {
            std::stringstream ssVelocity;
            ssVelocity << std::fixed << std::setprecision(2)
                       << obj.velocity.value();

            Point velocityTextOrg(obj.position.x + 5, obj.position.y - 7);

            // Draw outline for velocity text
            putText(*image, ssVelocity.str(), velocityTextOrg, fontFace,
                    fontScale, Scalar(0, 0, 0), thickness + 3);
            // Write the velocity text
            putText(*image, ssVelocity.str(), velocityTextOrg, fontFace,
                    fontScale, Scalar(255, 255, 0), thickness);
        }
        else {
            // Optional: Handle the case where velocity is not set
            // For example, you can draw a placeholder or skip drawing.
            Point velocityTextOrg(obj.position.x + 5, obj.position.y - 7);
            putText(*image, "N/A", velocityTextOrg, fontFace, fontScale,
                    Scalar(255, 0, 0), thickness);  // Draw "N/A" in red
        }
    }
}