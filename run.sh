#!/bin/bash

# Navigate to the main_bus directory and run main_bus
cd ~/vscode_projects/git_things/VehicleComputingSimulator/main_bus/build
./main_bus &
main_bus_pid=$!

# Navigate to the hsm directory and run grpc_server
cd ~/vscode_projects/git_things/VehicleComputingSimulator/hsm/build
./grpc_server &
grpc_server_pid=$!

# Navigate to the control directory and run MainControl
cd ~/vscode_projects/git_things/VehicleComputingSimulator/control/build
./MainControl &
main_control_pid=$!

# Function to clean up and stop all processes
cleanup() {
    echo "Stopping all processes..."
    kill $main_bus_pid $grpc_server_pid $main_control_pid
    echo "All processes have been stopped."
}

# Trap SIGINT (Ctrl+C) and SIGTERM to run the cleanup function
trap cleanup SIGINT SIGTERM

# Wait for user input to stop all processes
read -p "Press any key to stop all processes..." -n1 -s
cleanup

# Wait for all processes to finish (in case of manual termination)
wait $main_bus_pid $grpc_server_pid $main_control_pid
