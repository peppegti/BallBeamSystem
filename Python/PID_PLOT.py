import matplotlib.pyplot as plt

# Function to read the log file
def read_log(file):
    distances = []
    outputs = []
    times = []

    with open(file, 'r') as f:
        for line in f:
            if line.strip():  # Ignore empty lines
                parts = line.split(',')
                if len(parts) == 7:  # Check that there are 7 elements
                    try:
                        timestamp = int(parts[0].split(':')[1])
                        distance = float(parts[1].split(':')[1])
                        output = float(parts[2].split(':')[1])
                        Kp = float(parts[3].split(':')[1])
                        Ki = float(parts[4].split(':')[1])
                        Kd = float(parts[5].split(':')[1])
                        setpoint = float(parts[6].split(':')[1])

                        distances.append(distance)
                        outputs.append(output)
                        times.append(timestamp)
                    except (ValueError, IndexError):
                        print(f"Error parsing line: {line.strip()}")
                        continue
                else:
                    print(f"Incorrect line format: {line.strip()}")

    return distances, outputs, times, Kp, Ki, Kd, setpoint

# Log file name
log_file = 'putty.log'

# Read data from the log file
distances, outputs, times, Kp, Ki, Kd, setpoint = read_log(log_file)

# Check if the data was read correctly
if distances and outputs and times:
    # Calculate the time until the ball reaches the setpoint
    start_time_setpoint = None
    for idx, distance in enumerate(distances):
        if -1.5 <= distance - setpoint <= 1.5:
            start_time_setpoint = times[idx]
            break

    if start_time_setpoint is not None:
        stabilization_start_time = start_time_setpoint
        for idx, time in enumerate(times):
            if time - start_time_setpoint > 500:
                stabilization_end_time = time
                break

        stabilization_time = stabilization_end_time - stabilization_start_time
    else:
        print("The ball did not reach the position within -1.5 to 1.5 of the setpoint.")

    # Calculate the time when the ball stays still for at least 1000 ms
    start_time_stopped = None
    for idx, distance in enumerate(distances):
        if -1.5 <= distance - setpoint <= 1.5:
            if start_time_stopped is None:
                start_time_stopped = times[idx]
        else:
            start_time_stopped = None
        
        if start_time_stopped is not None and times[idx] - start_time_stopped >= 1000:
            stabilization_timestamp = times[idx]
            time_stopped = stabilization_timestamp - times[0]  # Calculate stabilization time from start
            distance_at_time_stopped = distances[idx]
            break
    else:
        time_stopped = None

    if time_stopped is not None:
        difference = setpoint - distance_at_time_stopped
        print(f"Time when the ball stays still for at least 1000 ms: {time_stopped} ms")
        print(f"Difference between the setpoint and distance at timestamp {stabilization_timestamp}: {difference} mm")
    else:
        print("The ball did not stay still for at least 1000 ms.")

    # Create the plot
    plt.figure(figsize=(12, 8))

    # Plot distance over time
    plt.subplot(3, 1, 1)
    plt.plot(times, [distance - setpoint for distance in distances], marker='o')  # Plot distance relative to setpoint
    plt.axhline(y=0, color='r', linestyle='--', label=f'Setpoint: {setpoint} mm')
    plt.title('Distance relative to Setpoint')
    plt.xlabel('Time (ms)')
    plt.ylabel('Distance - Setpoint (mm)')
    plt.legend()

    # Plot error over time
    errors = [distance - setpoint for distance in distances]  # Calculate errors as distance - setpoint
    plt.subplot(3, 1, 2)
    plt.plot(times, errors, marker='o', color='orange')
    plt.title('Error over Time')
    plt.xlabel('Time (ms)')
    plt.ylabel('Error (mm)')

    # Add a table with Kp, Ki, Kd, times, and final error below the plots
    plt.subplot(3, 1, 3)
    plt.axis('off')  # Hide the axes of the third subplot
    table_text = f"""
    Kp: {Kp}
    Ki: {Ki}
    Kd: {Kd}
    Time when the ball reaches near the setpoint: {start_time_setpoint} ms
    Stabilization time (over 500 ms within +/- 1.5 mm of the setpoint): {stabilization_time} ms
    Time when the ball stays still for at least 1000 ms: {time_stopped} ms
    """
    plt.text(0.1, 0.5, table_text, fontsize=12, verticalalignment='center')

    # Add space between plots
    plt.subplots_adjust(hspace=0.5)

    plt.show()  # Show the plot
