import subprocess
import concurrent.futures
import time

# Function to execute the shell script and return its output
def execute_shell_script():
    result = subprocess.run(['bash', './gen_random_patient.sh'], capture_output=True, text=True)
    return result.stdout.strip()

# Function to send curl request with provided data
def send_curl_request(data):
    start_time = time.time()
    try:
        # Execute the curl command with provided data
        subprocess.run(["curl", "--silent", "-o", "/dev/null", "-X", "POST", "-H", "Content-Type: application/json", "-d", data, "http://172.20.0.2:8080/v1/patient"], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error executing curl command: {e}")
    return time.time() - start_time

def main():
    # Number of parallel executions
    num_executions = 10000
    max_workers = 4  # Limit to 16 cores

    # Measure time for parallel executions
    start_total_time = time.time()
    with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
        # Retrieve output from shell script once
        payload_data = execute_shell_script()

        # Submit tasks for parallel execution
        futures = [executor.submit(send_curl_request, payload_data) for _ in range(num_executions)]

        # Wait for all tasks to complete
        concurrent.futures.wait(futures)

    end_total_time = time.time()

    # Calculate total time taken
    total_execution_time = end_total_time - start_total_time

    # Print results
    print(f"Total time for {num_executions} parallel executions using {max_workers} cores: {total_execution_time:.2f} seconds")
    print(f"Average time per request: {total_execution_time / num_executions:.4f} seconds")

if __name__ == "__main__":
    main()

