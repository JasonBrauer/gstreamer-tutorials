#include <string>
#include <thread>
#include <vector>

#include "pose_handlers.hpp"


/**
 * spawns a new drone if a new unique identifier is passed
 * @param vehicle_interface pointer to vehicle interface
 * @param drone_id unique vehicle identifier 
*/
void spawn_unique_drone(
    SimulatorInterface::VehiclePose *vehicle_interface,
    uint16_t drone_id
) {
    static std::vector<uint16_t> unique_drones;

    // if drone_id not found
    if(std::find(
        unique_drones.begin(),
        unique_drones.end(),
        drone_id) == unique_drones.end()
    ) {
        unique_drones.push_back(drone_id);
        vehicle_interface->spawn_vehicle(std::to_string(drone_id));
    }
}


void PoseHandlers::set_drone_pose(
    SimulatorInterface::VehiclePose *vehicle_interface,
    PoseTransfer::PoseMessage *pose_message,
    std::mutex *mutex_pose_message,
    bool *exit_flag
) {
    uint64_t msg_count = 0;
    do {
        mutex_pose_message->lock();

        spawn_unique_drone(vehicle_interface, pose_message->drone_id);

        if (pose_message->message_counter > msg_count) {
            vehicle_interface->set_vehicle_pose(
                pose_message->drone,
                std::to_string(pose_message->drone_id)
            );
        }
        msg_count = pose_message->message_counter;
        mutex_pose_message->unlock();
        // update at ~200hz
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    while(*exit_flag);
}